#include <stdio.h>
#include <stdlib.h>
#include "asn2.h"
#include "libpnm.h"

//Calculates MSE and returns a float
float mean_absolute_error(char *file_name_1_ptr, char *file_name_2_ptr) 
{
    //Declare pic sturcts
    struct PGM_Image pic1, pic2;
    int p1 = load_PGM_Image(&pic1, file_name_1_ptr);
    int p2 = load_PGM_Image(&pic2, file_name_2_ptr);

    //Check if they can be compared return -1 if not
    if(p1==-1 || p2 == -1)
    {
    	return -1;
    }

    if(pic1.height != pic2.height||pic1.width != pic2.width)
    {
    	return -1;
    }

	float scale= 1;

    if(pic1.maxGrayValue > pic2.maxGrayValue)
    {

    	scale =  pic1.maxGrayValue / pic2.maxGrayValue;

    }
    else if(pic1.maxGrayValue < pic2.maxGrayValue)
    {

    	scale =  pic2.maxGrayValue / pic1.maxGrayValue;

    }

    //Calculate the error, by finding the difference, scaling the values as you go
    //Malloc error array to store values
    int * errorArray = malloc((pic1.height * pic2.width)*sizeof(int));

    int row, col,i=0;                  
    for (row = 0; row < pic1.height; row++)
    {
        for (col = 0; col < pic1.width; col++)
        {

			if(pic1.maxGrayValue < pic2.maxGrayValue)
			{
				errorArray[i] = abs((int) ((pic1.image[row][col]*scale)-pic2.image[row][col]));
			}
			else if(pic1.maxGrayValue > pic2.maxGrayValue)
			{
				errorArray[i] = abs((int) ((pic2.image[row][col]*scale)-pic1.image[row][col]));
			}
			else
			{
				errorArray[i] = abs((int) ((pic2.image[row][col])-pic1.image[row][col]));
			}

			i++;

        }
    }

    //Loop through the errors to sum them
    long int total = 0;
    for(i = 0;i<(pic1.height * pic2.width);i++)
    {
    	total += errorArray[i];
    }
    //return MSE
	return (float) total / (pic1.height * pic2.width);

}



//main function, gets params and calls program
int main(int argc, char *argv[])
{

    //get MSE
	float MSE = mean_absolute_error(argv[1], argv[2]);

    //if not error print MSE
	if(MSE!= -1)
		printf("The MSE between these two photos is %f\n", MSE);
	else
		printf("Input File Error, ensure files exist and are same size and type\n"); //Error in files

    exit(0);

}
