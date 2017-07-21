#include <stdio.h>
#include <stdlib.h>
#include "asn2.h"
#include "libpnm.h"

//Struct to store huffman codes as they are generated
struct huffCode
{
    int symbol;
    int length;
    unsigned int code;
};

//Bitstream struct to help with writing to file
struct bitstream
{
    unsigned char *data;
    long int data_size;           // size of 'data' array
    long int last_bit_offset;     // last bit in the stream
    long int current_data_offset; // position in 'data', i.e. data[current_data_offset] is current reading/writing byte
    int current_bit_offset;       // which bit we are currently reading/writing
};

/*
//
//Returns a long int array of pixel frequency 
//
*/
long int *generate_pixel_frequency(struct PGM_Image *input_pgm_image, int *number_of_non_zero_values_in_the_frequency_array)
{
    //init array size 
    int arraySize = input_pgm_image->maxGrayValue + 1;

    //Allocate array mem
    long int *freqArray = malloc(arraySize * sizeof(long int));

    //Init array to 0
    int i;
    for (i = 0; i < arraySize; i++)
    {
        freqArray[i] = 0;
    }

    //loop through counting values
    int row, col, total_nonZero_values = 0;
    for (row = 0; row < input_pgm_image->height; row++)
    {
        for (col = 0; col < input_pgm_image->width; col++)
        {
            //If fA is 0 increment nonZero values
            if (freqArray[input_pgm_image->image[row][col]] == 0)
            {
                total_nonZero_values++;
            }
            //Incrememnt freq array
            freqArray[input_pgm_image->image[row][col]]++;
        }
    }

    *number_of_non_zero_values_in_the_frequency_array = total_nonZero_values;

    return freqArray;
}


//Function to generate hufman nodes
struct node *generate_Huffman_nodes(long int *pixel_frequency, int max_gray_value, int
        number_of_non_zero_values_in_the_frequency_array)
{

    //Init node array
    struct node *nodeArray = malloc((number_of_non_zero_values_in_the_frequency_array - 1) * sizeof(struct node));

    int i, first, second;
    for (i = 0; i < number_of_non_zero_values_in_the_frequency_array - 1; i++)
    {
        //get the lowest two values
        get_lowest_values(&first, &second, pixel_frequency, max_gray_value + 1);
        //create node

        if (first < second)
        {
            nodeArray[i].first_value = first;
            nodeArray[i].second_value = second;
            pixel_frequency[first] = pixel_frequency[first] + pixel_frequency[second];
            pixel_frequency[second] = 0;
        }
        else
        {
            nodeArray[i].first_value = second;
            nodeArray[i].second_value = first;
            pixel_frequency[second] = pixel_frequency[first] + pixel_frequency[second];
            pixel_frequency[first] = 0;
        }
    }

    return nodeArray;
}

//Returns the index of the two lowest NONZERO values in the pixel freq array
void get_lowest_values(int *first, int *second, long int *pixel_frequency, int arraySize)
{

    int index1 = 0;
    int index2 = 1;

    //if index 2 is zero find another value
    if (pixel_frequency[index2] == 0)
    {
        while (pixel_frequency[index2] == 0 && index2 < arraySize)
        {
            index2++;
            if (index1 == index2)
                index2++;
        }
    }

    //if index 1 is zero find another value
    if (pixel_frequency[index1] == 0)
    {
        while (pixel_frequency[index1] == 0 && index1 < arraySize)
        {
            index1++;
            if (index1 == index2)
                index1++;
        }
    }

    //Swap if 1 is larger
    if (pixel_frequency[index2] < pixel_frequency[index1])
    {
        int temp = index1;
        index1 = index2;
        index2 = temp;
    }

    //Check all values
    int i;
    for (i = 0; i < arraySize; i++)
    {

        if (pixel_frequency[i] != 0 && i != index1 && i != index2)
        {

            if (pixel_frequency[i] < pixel_frequency[index1])
            {
                index2 = index1;
                index1 = i;
            }
            else if (pixel_frequency[i] < pixel_frequency[index2])
            {
                index2 = i;
            }
        }
    }

    *first = index1;
    *second = index2;
}

//returns the index of the symbol in the huffCode Array or -1 if it isnt in there
int contains(struct huffCode *huffcodeArray, int length, int symbol)
{
    int i;
    for (i = 0; i < length; i++)
    {
        if (huffcodeArray[i].symbol == symbol)
            return i;
    }

    return -1;
}

//This function generates the huffman codes and places them in an array
void generate_huffman_codes(struct huffCode *huffcodeArray, int arrayLen, int first, int second)
{
    //Find code values in the array
    int indexFirst = contains(huffcodeArray, arrayLen, first);
    int indexSecond = contains(huffcodeArray, arrayLen, second);

    //If they are not in the array assign them a new node
    if (indexFirst == -1)
    {
        indexFirst = contains(huffcodeArray, arrayLen, -1);
        huffcodeArray[indexFirst].symbol = first;
    }

    if (indexSecond == -1)
    {
        indexSecond = contains(huffcodeArray, arrayLen, -1);
        huffcodeArray[indexSecond].symbol = second;
    }

    //if one alread has a code, place the longer code on the in the shorter node
    if (huffcodeArray[indexFirst].length > huffcodeArray[indexSecond].length)
    {
        huffcodeArray[indexSecond].code = huffcodeArray[indexFirst].code;
        huffcodeArray[indexSecond].length = huffcodeArray[indexFirst].length;
    }
    else if (huffcodeArray[indexFirst].length < huffcodeArray[indexSecond].length)
    {
        huffcodeArray[indexFirst].code = huffcodeArray[indexSecond].code;
        huffcodeArray[indexFirst].length = huffcodeArray[indexSecond].length;
    }

    //Shift first and ++ length
    huffcodeArray[indexFirst].code = huffcodeArray[indexFirst].code << 1;
    huffcodeArray[indexFirst].length++;
    //Shift second set LSB to 1 and ++ length
    huffcodeArray[indexSecond].code = huffcodeArray[indexSecond].code << 1;
    huffcodeArray[indexSecond].code |= 1;
    huffcodeArray[indexSecond].length++;
}

//Writes data to a char array in bitstream struct so it can be written with bin writ later
void writeData(struct bitstream *stream, struct huffCode *codeStruct)
{
    //loop through code
    int i;
    for (i = codeStruct->length - 1; 0 <= i; i--)
    {
        //This byte is out of bites, advance to next byte
        if (stream->current_bit_offset < 0)
        {
            stream->current_data_offset++;
            stream->data[stream->current_data_offset] = 0x00;
            stream->current_bit_offset = 7;
        }

        //If code bit is 1 write it to byte, otherwise leave it as 0
        if (((codeStruct->code >> i) & 0x01))
        {
            stream->data[stream->current_data_offset] |= 0x01 << stream->current_bit_offset;
            stream->current_bit_offset--;
            stream->last_bit_offset++;
        }
        else //leave it as a zero
        {
            stream->current_bit_offset--;
            stream->last_bit_offset++;
        }
    }
}

//encodes the image using the generated huffman codes
unsigned char *Huffman_encode(struct PGM_Image *input_pgm_image, struct node *Huffman_node, int number_of_nodes,
                              unsigned long int *length_of_encoded_array)
{

    //Decalre an array to store the codes, their symbol and length```
    int huffCodeArrLen = number_of_nodes + 1;
    struct huffCode *huffcodeArray = malloc((huffCodeArrLen) * sizeof(struct huffCode));

    //init node array
    int i;
    for (i = number_of_nodes; i >= 0; i--)
    {
        huffcodeArray[i].symbol = -1;
        huffcodeArray[i].length = 0;
        huffcodeArray[i].code = 0x0;
    }

    //Generate Huffancode from nodes in tree
    for (i = number_of_nodes - 1; i >= 0; i--)
    {
        generate_huffman_codes(huffcodeArray, huffCodeArrLen, Huffman_node[i].first_value, Huffman_node[i].second_value);
    }


    //encode picture
    int row, col;
    struct bitstream stream;
    stream.data = malloc((input_pgm_image->height * input_pgm_image->width) * sizeof(unsigned char));
    stream.data_size = (input_pgm_image->height * input_pgm_image->width); // size of 'data' array
    stream.last_bit_offset = 0L;                                           // last bit in the stream
    stream.current_data_offset = 0L;                                       // position in 'data', i.e. data[current_data_offset] is current reading/writing byte
    stream.current_bit_offset = 7;                                         // which bit we are currently reading/writing

    for (row = 0; row < input_pgm_image->height; row++)
    {
        for (col = 0; col < input_pgm_image->width; col++)
        {
            //get the code for this symbol
            struct huffCode code = huffcodeArray[contains(huffcodeArray, huffCodeArrLen, input_pgm_image->image[row][col])];
            writeData(&stream, &code);
        }
    }

    *length_of_encoded_array = stream.current_data_offset + 1;

    return stream.data;
}

void store_Huffman_encoded_data(char *compressed_file_name_ptr, int image_width, int image_height, int max_gray_value,
    int number_of_nodes, struct node *Huffman_node, long int length_of_encoded_image, unsigned char                                                                                                              *encoded_image)
{

    //open a file
    FILE *file;
    if ((file = fopen(compressed_file_name_ptr, "w")) == NULL)
    {
        printf("File %s failed to open\n", compressed_file_name_ptr);
        exit(1);
    }

    //write file stats
    fprintf(file, "%d\n", image_width);
    fprintf(file, "%d\n", image_height);
    fprintf(file, "%d\n", max_gray_value);
    fprintf(file, "%d\n", number_of_nodes);

    //write in nodes
    int i;
    for (i = 0; i < number_of_nodes; i++)
    {
        fprintf(file, "%d %d\n", Huffman_node[i].first_value, Huffman_node[i].second_value);
    }
    //Write encoded image length and image data
    fprintf(file, "%ld\n", length_of_encoded_image);

    fwrite(encoded_image, sizeof(unsigned char), length_of_encoded_image, file);

    fclose(file);
}

//main function, gets params and calls program
int main(int argc, char *argv[])
{

    //load pgm image
    struct PGM_Image pic1;
    load_PGM_Image(&pic1, argv[1]);

    //Init array length values
    int numNonZeroValues;
    unsigned long int encodeArrayLength;

    //Generate Pixel Freq
    long int *freqArray = generate_pixel_frequency(&pic1, &numNonZeroValues);

    //Generate huffman tree / nodes
    struct node *nodeArr = generate_Huffman_nodes(freqArray, pic1.maxGrayValue, numNonZeroValues);

    //encode image
    unsigned char *encodedPic = Huffman_encode(&pic1, nodeArr, numNonZeroValues - 1, &encodeArrayLength);

    //Save file
    store_Huffman_encoded_data(argv[2], pic1.width, pic1.height, pic1.maxGrayValue, numNonZeroValues - 1,
                               nodeArr, encodeArrayLength, encodedPic);

    exit(0);

}
