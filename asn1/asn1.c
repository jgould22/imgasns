#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "recUtils.h"
#include "lineUtils.h"
#include "libpnm.h"

#define PBM_TYPE 1
#define PGM_TYPE 2
#define PPM_TYPE 3

typedef struct InputParameters  {
  int imageType;
  int imageWidth;
  int imageHeight;
  char* imageOutName;
  int outPutCode;
} InputParameters;



char* createFileName(char* fileName, char* fileType ){

  char * nameAndType = (char *) malloc(1 + strlen(fileName)+ strlen(fileType) );
  strcpy(nameAndType, fileName);
  strcat(nameAndType, fileType);
  return nameAndType;

}



//Takes InputParameters and returns 0 if no errors or 1 for errors
void validateInput(InputParameters* p, int argc, char* argv[]){

   //Return Errors based on cases outlined in assignment
  //Check For correct number of args
  if(argc != 6){
        printf("Incorrect number of Arguments arguments must be Image Type, Width, Height, \
        Output Name, and Out Put code\n");
        exit(1);
  }

  char *endptr;
  //Set Image type
  p->imageType = strtol(argv[1], &endptr, 10);

  errno = 0;
  if (endptr == argv[1])
  {
    printf("Image Type must be a number / Interger\n");
    exit(1);
  }

  //Invalid Image Type
  if(p->imageType < 1 || p->imageType > 3){
    printf("Invalid Image Type\n");
    printf("Options are pbm, pgm, and ppm\n");
    exit(1);
  }

  //Set height and width
  //Convert String to Int for width

  p->imageWidth = strtol(argv[2], &endptr, 10);

  errno = 0;
  if (endptr == argv[2])
  {
    printf("Image Width must be a number / Interger\n");
    exit(1);
  }


  //Convert String to int for height
  p->imageHeight = strtol(argv[3], &endptr, 10);

  errno = 0;
  if (endptr == argv[3])
  {
    printf("Image Height must be a number / Interger\n");
    exit(1);
  }

  //Image width for pbm and pgm must be divisiable by 4
  if(p->imageType == 1 || p->imageType == 2){

    //Check image width and length are multiples of 4
    if(p->imageWidth%4 != 0 || p->imageWidth < 4){
      printf("Image Width must be multiple of 4 and greater than or equal to 4\n");
      exit(1);
    }

  }

  //Image width for PPM must be 6 or greater and mod6 =0
  if(p->imageType == 3){

    if(p->imageWidth%6 != 0 || p->imageWidth < 6){
      printf("Image Width for PPM must be multiple of 6 and greater than or equal 6\n");
      exit(1);
    }

  }

  //Check height is a multiple of 4
  if(p->imageHeight%4 != 0){
    printf("Image Height must be multiple of 4\n");
    exit(1);
  }

  //Ensure height is 4 or greater
  if(p->imageHeight < 4){
    printf("Image Height must be greater than or equal to 4\n");
    exit(1);
  }

  //set output file name
  p->imageOutName = argv[4];

  //Set file type
  p->outPutCode = strtol(argv[5], &endptr, 10);

  errno = 0;
  if (endptr == argv[5])
  {
    printf("Output code must be 0 or 1\n");
    exit(1);
  }

  if(p->outPutCode < 0 || p->outPutCode > 1){
    printf("Output code must be 0 or 1\n");
    exit(1);
  }

}

/*
Program 1
*/
//Provides solution to problem 1, Drawing white rectangle with x on a black rectangle
void progOne(InputParameters* p){

  struct PBM_Image pbmPic;
  create_PBM_Image(&pbmPic, p->imageWidth, p->imageHeight);
  //Draw Black Rectangle
  drawRectangle(&pbmPic,0,0,p->imageWidth,p->imageHeight, BLACK );

  //Draw White Rectangle
  drawRectangle(&pbmPic,(p->imageWidth/4),(p->imageHeight/4),p->imageWidth/2,p->imageHeight/2,WHITE);

  //Draw Black X
  drawX(&pbmPic,(p->imageWidth/4),(p->imageHeight/4),(p->imageWidth/2),(p->imageHeight/2),BLACK);

  //Save picture
  save_PBM_Image(&pbmPic,p->imageOutName,p->outPutCode);

  //Free picture Memory 
  free_PBM_Image(&pbmPic);


}


/*
Program 2
*/
void progTwo(InputParameters* p){

  //Create Picture
  struct PGM_Image pgmPic;
  create_PGM_Image(&pgmPic, p->imageWidth, p->imageHeight,255);
  //Draw Black Rectangle
  drawRectanglePGM(&pgmPic,0,0,p->imageWidth,p->imageHeight, 0 );

  //Draw White Rectangle
  drawRectCenterGradient(&pgmPic,p->imageWidth/4,p->imageHeight/4,p->imageWidth/2,p->imageHeight/2,255,1);

  //Save picture
  save_PGM_Image(&pgmPic,p->imageOutName,p->outPutCode);

  //Free picture Memory 
  free_PGM_Image(&pgmPic);


}

/*
Program 3
*/
void progThree(InputParameters* p){

    //Create all the reqired pictures for this program 1 ppm and 3 pgm
    struct PPM_Image ppmPic;
    struct PGM_Image pic1, pic2, pic3;
    create_PPM_Image(&ppmPic, p->imageWidth, p->imageHeight,255);

    //Draw red gradient rectangle
    drawRectangleGradient(&ppmPic,0,0,p->imageWidth/3,p->imageHeight/2, RED, 255, 0 );
   //Draw green gradient rectangle
    drawRectangleGradient(&ppmPic,p->imageWidth/3,0,p->imageWidth/3,p->imageHeight/2, GREEN, 255, 1 );
    //Draw blue gradient rectangle
    drawRectangleGradient(&ppmPic,(p->imageWidth/3)*2,0,p->imageWidth/3,p->imageHeight/2, BLUE, 255, 0 );
    //Draw bottum left black gradient
    drawRectangleGradient(&ppmPic,0,p->imageHeight/2,p->imageWidth/2,p->imageHeight/2, 3,255,0 );
    //Draw bottum right gradient
    drawRectangleGradient(&ppmPic,p->imageWidth/2,p->imageHeight/2,p->imageWidth/2,p->imageHeight/2, 3,255,1);

    //Copy ppm to its respective gray maps
    copy_PPM_to_PGM(&ppmPic, &pic1, RED);
    copy_PPM_to_PGM(&ppmPic, &pic2, GREEN);
    copy_PPM_to_PGM(&ppmPic, &pic3, BLUE);

    //Save all 3 pgm and ppn
    save_PGM_Image(&pic1,createFileName(p->imageOutName,"-RED"),p->outPutCode);
    save_PGM_Image(&pic2,createFileName(p->imageOutName,"-GREEN"),p->outPutCode);
    save_PGM_Image(&pic3,createFileName(p->imageOutName,"-BLUE"),p->outPutCode);
    save_PPM_Image(&ppmPic,p->imageOutName,p->outPutCode);

    //Free image memory
    free_PPM_Image(&ppmPic);
    free_PGM_Image(&pic1);
    free_PGM_Image(&pic2);
    free_PGM_Image(&pic3);

}

//main function, gets params and calls program functions depending on image types
int main (int argc, char *argv[]){

  //Set Program arguments
  InputParameters p;

  validateInput(&p,argc,argv);

  //Call program 1
  if(p.imageType == PBM_TYPE)
    progOne(&p);

  //Call program 2
  if(p.imageType == PGM_TYPE)
    progTwo(&p);

  //Call Program 3
  if(p.imageType == PPM_TYPE)
    progThree(&p);

  //exit with 0
  exit(0);

}
