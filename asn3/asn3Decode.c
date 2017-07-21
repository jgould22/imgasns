#include "libpnm.h"
#include <time.h>
#include "asn3.h"

void Decode_Using_LZ77(char *in_compressed_filename_Ptr, char *out_PGM_filename_Ptr) 
{
  
    //read in file
    long int tokenCounter;
    int width=0,height=0,maxGrayValue=0;
    FILE* file;
    if ( !(file = fopen(in_compressed_filename_Ptr, "r")) )
    {
        printf("File Error\n");
        exit(1);
    }

    //Read in image data
    fscanf(file, "%d", &height);
    fscanf(file, "%d", &width);
    fscanf(file, "%d", &maxGrayValue);
    fscanf(file, "%ld", &tokenCounter);

    struct PGM_Image pgmPic;
    create_PGM_Image(&pgmPic, width, height,maxGrayValue);

    struct token* tokenArray = malloc(tokenCounter * sizeof(struct token)); 
    //read in offsets
    int i,j;
    for (i = 0; i < tokenCounter; i++)
    {
        fscanf(file, "%d", &tokenArray[i].offset);
    }

    //read in lengths
    for (i = 0; i < tokenCounter; i++)
    {
        fscanf(file, "%d", &tokenArray[i].length);
    }

    //read in nextValue
    for (i = 0; i < tokenCounter; i++)
    {
        fscanf(file, "%hhu", &tokenArray[i].nextValue);
    }

    fclose(file);

    long int imageArrayLength = pgmPic.width*pgmPic.height;

    //Copy image to 1D array because for some reason pgm.image can not be accessed with pointer math 
    unsigned char *image = malloc(sizeof(unsigned char)*imageArrayLength);

    //Decode picture
    long int readPointer = 0;
    long int writePointer = 0;
    long int startPointer = writePointer;

    for(i =0;i<tokenCounter;i++)
    {
        readPointer = writePointer-tokenArray[i].offset;
        startPointer = writePointer;

        for(j =0;j<tokenArray[i].length;j++)
        {
            //If longer than start repeat pattern
            if(startPointer == readPointer)
                readPointer = startPointer-tokenArray[i].offset;

            *(image+writePointer) = *(image+readPointer);
            writePointer++;
            readPointer++;

        }

        //write token letter and increment write pointer
         *(image+writePointer) = tokenArray[i].nextValue;
         writePointer++;

    }

    //copy from image pointer to pic struct 
    long int counter = 0;
    for(i =0;i<pgmPic.height;i++)
    {
       for(j =0;j<pgmPic.width;j++)
        {
            pgmPic.image[i][j]= image[counter++];
        }   
    }

    //save pic file
    save_PGM_Image(&pgmPic, out_PGM_filename_Ptr, 1);


}

//main function, gets params and calls program
int main(int argc, char *argv[])
{

    if(argc<3 || argc >3)
    {
        printf("Argument Error\nUsage: asn3d inputFilePath outputFilePath\n");
        exit(1);
    }

    //Start Timer
    clock_t start = clock(), diff;

    Decode_Using_LZ77(argv[1], argv[2]);

    diff = clock() - start;

    int msec = diff * 1000 / CLOCKS_PER_SEC;

    printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);

    exit(0);

}
