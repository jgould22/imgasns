#include "libpnm.h"
#include <time.h>
#include "asn3.h"

void Encode_Using_LZ77(char *in_PGM_filename_Ptr, unsigned int searching_buffer_size, char
*compressed_filename_Ptr)
{
    //Read in file 
    struct PGM_Image pgmPic;
    load_PGM_Image(&pgmPic, in_PGM_filename_Ptr);
    long int imageArrayLength = pgmPic.width*pgmPic.height;

    //Copy image to 1D array because for some reason pgm.image can not be accessed with pointer math and it is easer to code in 1d 
    unsigned char *image = malloc(sizeof(unsigned char)*imageArrayLength);
    long int counter = 0;
    int i,j;
    for(i =0;i<pgmPic.height;i++)
    {
       for(j =0;j<pgmPic.width;j++)
        {
            image[counter++] = pgmPic.image[i][j];
        }   
    }

    long int searchPointer = 0;
    unsigned char *imagePointer = &image[0];

    //allocate array of pointers enough for no duplicate patterns
    encodeToken** tokenArray = malloc(sizeof(encodeToken*)*imageArrayLength);
    struct searchBuffer window;
    window.max_length = searching_buffer_size;
    window.current_length = 0;
    long int tokenCounter = 0;

    //encode image 
    while(searchPointer<imageArrayLength)
    {

        tokenArray[tokenCounter] = findToken(&window,imagePointer+searchPointer,imageArrayLength,searchPointer);
        searchPointer += tokenArray[tokenCounter]->length+1;
        tokenCounter++;
    }

    //write out file in ascci for debug and histo values
    FILE * f;

    if ( !(f = fopen(compressed_filename_Ptr, "w")) )
    {
        printf("File Error\n");
        exit(1);
    }
    fprintf(f, "%d\n", pgmPic.height);
    fprintf(f, "%d\n", pgmPic.width);
    fprintf(f, "%d\n", pgmPic.maxGrayValue);
    fprintf(f, "%ld\n", tokenCounter);

    //write in offsets
    for (i = 0; i < tokenCounter; i++)
    {
        fprintf(f, "%d ", tokenArray[i]->offset);
    }
    fprintf(f, "\n");

    //write in lengths
    for (i = 0; i < tokenCounter; i++)
    {
        fprintf(f, "%d ", tokenArray[i]->length);
    }
    fprintf(f, "\n");

    //write in nextValue
    for (i = 0; i < tokenCounter; i++)
    {
        fprintf(f, "%d ", tokenArray[i]->nextValue);
    }

    fclose(f);

}

encodeToken* findToken (struct searchBuffer *searchWindow,unsigned char *imagePointer,long int imageArrayLength,long int searchPointer)
{
    //Allocate token
    int backOffset = searchWindow->current_length;
    int matchLocation = 0;
    int matchLength = 0;
    int currentMatchLocation = 0;
    int matchFlag = 0;
    int currentMatchLength = 0;
    int forwardOffset = 0;

    while(backOffset>0)
    {
        //Value match, increment forwardOffset set match location
        if((*(imagePointer+forwardOffset)==*(imagePointer-backOffset))
            && !(searchPointer+forwardOffset>imageArrayLength))
        {
            //already part of a current match
            if(!matchFlag)
            {
                matchFlag = 1;
                currentMatchLocation = backOffset;     
            }
            currentMatchLength++;
            forwardOffset++;

        }
        else //values did not match
        {
            //we had a match, reset
            if(matchFlag)
            {
                //new longest match?
                if(currentMatchLocation-backOffset>=matchLength)
                {
                    matchLocation = currentMatchLocation;
                    matchLength = currentMatchLength;
                }
                currentMatchLocation = 0;
                currentMatchLength = 0;
                forwardOffset = 0;
                matchFlag = 0;

            }

        }

        backOffset--;

    }

    //Create Token
    struct token* token = malloc(sizeof(struct token));
    token->offset = matchLocation;
    token->length = matchLength;
    token->nextValue = *(imagePointer+matchLength);
    if(searchWindow->current_length<searchWindow->max_length)
        searchWindow->current_length += token->length+1;
    return token;

}


//main function, gets params and calls program
int main(int argc, char *argv[])
{

    if(argc<4)
    {
        printf("Not enough Arguments\nUsage: asn3e inputFilePath searchBufferSize outputFilePath\n");
        exit(1);
    }

    //Start CPU timer
    clock_t start = clock(), diff;

    Encode_Using_LZ77(argv[1], atoi(argv[2]), argv[3]);

    diff = clock() - start;

    int msec = diff * 1000 / CLOCKS_PER_SEC;

    printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);

    exit(0);

}
