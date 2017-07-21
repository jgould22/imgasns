#include <libpnm.h>
#include "lineUtils.h"
#include <math.h>

//Draws a rectangels on PBM image
void drawRectangle(struct PBM_Image * pbmPic,int topLeftX,int topLeftY, int recWidth, int recHeight,int color){

  int row, col;
  for(row = topLeftY; row<(recHeight+topLeftY); row++)
    for(col = topLeftX; col<(recWidth+topLeftX); col++)
        pbmPic->image[row][col] = color;

}

//Draw a rectangle on a PGM image
void drawRectanglePGM(struct PGM_Image * pgmPic,int topLeftX,int topLeftY, int recWidth, int recHeight,int color){

  int row, col;
  for(row = topLeftY; row<(recHeight+topLeftY); row++)
    for(col = topLeftX; col<(recWidth+topLeftX); col++)
        pgmPic->image[row][col] = color;

}

//Create a rectangle filled with a gradient direction defined by reverse
void drawRectangleGradient(struct PPM_Image * ppmPic,int topLeftX,int topLeftY, int recWidth, int recHeight,int gradientColor,int maxGreyValue,int reverse)
{

  //Check to see if the gradient goes up or down
  int gradient;
  if(reverse)
    gradient=maxGreyValue;
  else
    gradient = 0;

  float percentCompleteCounter=0;

  //Loop through the rectangle 
  int row, col, color;
  for(row = topLeftY; row<(recHeight+topLeftY); row++){

    for(col = topLeftX; col<(recWidth+topLeftX); col++){

      for(color = RED; color <= BLUE; color++){

        //Inncrement all colors that are not the gradient color and set the gradient color to max
        //This increases white since the other colors are moving towards 255 255 255
        if(color != gradientColor || gradientColor == 3)
          ppmPic->image[row][col][color] = gradient;
        else
          ppmPic->image[row][col][color] = maxGreyValue;

      }

    }

    //Check the percentage of completion to increment the gradient towards max gradient 
    percentCompleteCounter++;
    gradient = (float)maxGreyValue * ((percentCompleteCounter/(float)recHeight));
    if(reverse)
      gradient=maxGreyValue -gradient;


  }


}


//Draws a a rectangle with no fill
void drawEmptyRectangle(struct PGM_Image * pbmPic,int topLeftX,int topLeftY, int recWidth, int recHeight,int color){

  //Use -1s to correct for 0 indexing 
  //Top
  drawLinePGM(pbmPic,topLeftX,topLeftY,topLeftX+recWidth-1,topLeftY,color);
  //Left
  drawLinePGM(pbmPic,topLeftX,topLeftY,topLeftX,topLeftY+recHeight-1,color);
  //Right
  drawLinePGM(pbmPic,topLeftX+recWidth-1,topLeftY,topLeftX+recWidth-1,topLeftY+recHeight-1,color);
  //Bottum
  drawLinePGM(pbmPic,topLeftX,topLeftY+recHeight-1,topLeftX+recWidth-1,topLeftY+recHeight-1,color);

}

//Draws a rectangle with a center gradient 
void drawRectCenterGradient(struct PGM_Image * pgmPic,int topLeftX,int topLeftY, int recWidth, int recHeight,int maxGreyValue,int reverse)
{

  //Set line to center
  int x0 = topLeftX;
  int y0 = topLeftY;
  int x1 = topLeftX + recWidth;
  int y1 = topLeftY + recHeight;
  int gradRecWidth = recWidth;
  int gradRecHeight = recHeight;
  float hypcalc = sqrt(pow(recWidth/2,2)+pow(recHeight/2,2));

  int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
  int err = (dx>dy ? dx : -dy)/2, e2;

  //Determin if gradient is going to darker or lighter
  int gradient;
  if(reverse)
    gradient=maxGreyValue;
  else
    gradient = 0;
  int percentCompleteCounter = 0;

  //Draw a line from the top left corner to the center of the rectangle
  //As you go calculate the percentage of the distance along the line to determin the grey level
  //For each spot on the line draw a rectangle, this will fill in the other sides of the gradient
  for(;;){

    drawEmptyRectangle(pgmPic, x0, y0, gradRecWidth, gradRecHeight, gradient);

    if ((x0==x1 && y0==y1)||(gradRecWidth==0 && gradRecHeight == 0) ) break;
    e2 = err;

    //Determin if line should advance x
    if (e2 >-dx) {
      err -= dy;
      x0 += sx;
      gradRecWidth -= (sx*2); //as you incrment x based on the error decrement the rec

      //Calculte how far along the line you are 
      percentCompleteCounter = sqrt(pow(x0-(recWidth/2),2)+pow(y0-(recHeight/2),2));
      gradient = (float)maxGreyValue * (percentCompleteCounter/hypcalc);
      if(reverse)
        gradient=maxGreyValue -gradient;
    }
    //Determin if line should advance y
    if (e2 < dy) {
      err += dx;
      y0 += sy;
      gradRecHeight -= (sy*2);
      percentCompleteCounter = sqrt(pow(x0-(recWidth/2),2)+pow(y0-(recHeight/2),2));
      gradient = (float)maxGreyValue * (percentCompleteCounter/hypcalc);
      if(reverse)
        gradient=maxGreyValue -gradient;
    }


  }


}


