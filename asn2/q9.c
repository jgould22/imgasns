#include <stdio.h>
#include <stdlib.h>
#include "asn2.h"
#include "libpnm.h"

//Tree node to create huffman tree
struct treeNode
{
    int symbol;
    int left;
    int right;
};

//Bitstream to help with data writing
struct bitstream
{
    unsigned char *data;
    long int data_size;           // size of 'data' array
    long int last_bit_offset;     // last bit in the stream
    long int current_data_offset; // position in 'data', i.e. data[current_data_offset] is current reading/writing byte
    int current_bit_offset;       // which bit we are currently reading/writing
};

//Returns index of the node with the symbol param, or -1 if it doesnt exit
int findNode(struct treeNode *tree,int symbol,int treeSize)
{

    int i;
    for(i = 0;i<treeSize;i++)
    {
        if(tree[i].symbol == symbol)
            return i;

    }

    return -1;

}

//returns an empty node
int getEmptyNode(struct treeNode *tree,int treeSize)
{

    int i;
    for(i = 1;i<treeSize;i++)
    {
        if(tree[i].symbol == -1 && tree[i].left == -1 && tree[i].right == -1)
            return i;

    }

    return -1;

}

//Creates huffman tree and returns root node
struct treeNode *generate_Huffman_tree(struct node *Huffman_node,int lengthOfNodeArray)
{

    //Create tree Array
    int treeSize = (lengthOfNodeArray+1)+(lengthOfNodeArray);
    struct treeNode *tree = malloc(treeSize*sizeof(struct treeNode));

    //init leaf array
    int i;
    for(i = treeSize-1;i>=0;i--)
    {
        tree[i].symbol = -1;
        tree[i].left = -1;
        tree[i].right = -1;
    }

    //root node is always at 0 init first nodes
    int leftNodeIndex = getEmptyNode(tree,treeSize);
    tree[0].left = leftNodeIndex;
    tree[leftNodeIndex].symbol = Huffman_node[lengthOfNodeArray-1].first_value;
    int rightNodeIndex = getEmptyNode(tree,treeSize);
    tree[0].right = rightNodeIndex;
    tree[rightNodeIndex].symbol = Huffman_node[lengthOfNodeArray-1].second_value;

    //Loop through all values except root
    for(i = lengthOfNodeArray-2;i>=0;i--)
    {

        //first find first value
        int firstIndex = findNode(tree,Huffman_node[i].first_value,treeSize);

        //get newEmpty Node left index
        leftNodeIndex = getEmptyNode(tree,treeSize);
        tree[firstIndex].left = leftNodeIndex;

        //Set symbol left
        tree[leftNodeIndex].symbol = Huffman_node[i].first_value;

        //get new Empty node right
        rightNodeIndex = getEmptyNode(tree,treeSize);
        tree[firstIndex].right = rightNodeIndex;

        //Set Symbol right
        tree[rightNodeIndex].symbol = Huffman_node[i].second_value;

        //set new found node to -1
        tree[firstIndex].symbol = -1;

    }

    return tree;

}

//Reads encoded file and returns values
unsigned char *read_Huffman_encoded_data(char *compressed_file_name_ptr, int *image_width, int *image_height, int
*max_gray_value, int *number_of_nodes, struct node **Huffman_node, long int *length_of_encoded_image)
{

    //open a file
    FILE *file;
    if ((file=fopen(compressed_file_name_ptr,"r")) == NULL)
    {   
        printf("File %s failed to open",compressed_file_name_ptr);
        exit(1);
    }

    //Read in image data
    fscanf(file, "%d", image_width);
    fscanf(file, "%d", image_height);
    fscanf(file, "%d", max_gray_value);
    fscanf(file, "%d", number_of_nodes);

    struct node * nodeArray = malloc(*number_of_nodes * sizeof(struct node)); 

    //Read in nodes
    int i;
    for(i = 0;i<*number_of_nodes;i++)
    {
       // Huffman_node[i] = malloc(sizeof(struct node));
        fscanf(file, "%d", &nodeArray[i].first_value);
        fscanf(file, "%d", &nodeArray[i].second_value);
    }

    *Huffman_node = nodeArray;

    fscanf(file, "%ld", length_of_encoded_image);

    unsigned char * encoded_image = malloc(*length_of_encoded_image * sizeof(unsigned char));

    //Advance one byte because fscanf sits on \n
    fseek(file,1,SEEK_CUR);
    fread(encoded_image, 1,*length_of_encoded_image ,file);

    fclose(file);

    return encoded_image;

}

//Reads in the data, traversing the tree and returns the value when it reaches a leaf node
int readData(struct bitstream *stream, struct treeNode *tree)
{

    int currentNode = 0;

    //loop through code
    while(stream->current_data_offset<stream->data_size)
    {
        while(stream->current_bit_offset>=0){

            //if the current bit is 1 go left
            if (((stream->data[stream->current_data_offset] >> stream->current_bit_offset) & 0x01)) //value is one
            {
                //go left
                currentNode = tree[currentNode].right;
                //check if leaf Node
                stream->current_bit_offset--;
                if(tree[currentNode].left==-1&&tree[currentNode].right==-1)
                {
                    return tree[currentNode].symbol;
                }

            }
            else //value must be 0
            {
                //go right
                currentNode = tree[currentNode].left;
                //check if leaf Node
                stream->current_bit_offset--;
                if(tree[currentNode].left==-1&&tree[currentNode].right==-1)
                {
                    return tree[currentNode].symbol;
                }

            }


        }

        stream->current_data_offset++;
        stream->current_bit_offset = 7;
    }

    exit(1);
    
}

//Decodes the image and return a pgm struct 
struct PGM_Image *Huffman_decode_image( int image_width, int image_height, int max_gray_value, int
number_of_nodes, struct node *Huffman_node, long int length_of_encoded_image, unsigned char *encoded_image) 
{
    //Allocate mem for pic
    struct PGM_Image *pic1 = malloc(sizeof(struct PGM_Image));
    create_PGM_Image(pic1, image_width,image_height,max_gray_value);

    struct treeNode *root = generate_Huffman_tree(Huffman_node,number_of_nodes);

    //Init bit stream
    struct bitstream stream;
    stream.data = encoded_image ;
    stream.data_size = length_of_encoded_image; // size of 'data' array
    stream.last_bit_offset = 0L;                // last bit in the stream
    stream.current_data_offset = 0L;                                       // position in 'data', i.e. data[current_data_offset] is current reading/writing byte
    stream.current_bit_offset = 7;    

    //Loop through picutre 
    int row, col;
    for (row = 0; row < pic1->height; row++)
    {
        for (col = 0; col < pic1->width; col++)
        {

            pic1->image[row][col] = readData(&stream, root);

        }
    }

    return pic1;
}

//main function, gets params and calls program
int main(int argc, char *argv[])
{
    //Declare pic vars
    int image_width, image_height, max_gray_value, number_of_nodes;
    struct node *Huffman_node;
    long int length_of_encoded_image;

    //Read file data
    unsigned char * encoded_image = read_Huffman_encoded_data(argv[1], &image_width, &image_height, &max_gray_value, &number_of_nodes, 
        &Huffman_node, &length_of_encoded_image);

    //Build a tree and decode image
    struct PGM_Image *pic1 = Huffman_decode_image(image_width, image_height, max_gray_value, 
        number_of_nodes, Huffman_node, length_of_encoded_image,encoded_image);

    //Save image
    save_PGM_Image(pic1, argv[2],true);

    exit(0);

}
