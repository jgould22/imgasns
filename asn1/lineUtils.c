#include "libpnm.h"
#include "math.h"
/*
Function draws line based on Bresenham's line algorithm taken from 
https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm and modified 
here to work with our image library for PGM images
*/
void drawLinePGM(struct PGM_Image * pbmPic,int x0,int y0, int x1, int y1,int greyLevel){
  int dx = abs(x1-x0);
  int sx = x0<x1 ? 1 : -1;
  int dy = abs(y1-y0);
  int sy = y0<y1 ? 1 : -1;
  int err = (dx>dy ? dx : -dy)/2;
  int e2;

  for(;;){
    pbmPic->image[y0][x0] = greyLevel;
    if (x0==x1 && y0==y1){
          break;
    } 
    e2 = err;
    if (e2 >-dx) { 
      err -= dy; 
      x0 += sx; 
    }
    if (e2 < dy) { 
      err += dx; 
      y0 += sy; 
    }
  }

}


/*
Function draws line based on Bresenham's line algorithm taken from 
https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm and modified 
here to work with our image library for PBM images
*/
void drawLine(struct PBM_Image * pbmPic,int x0,int y0, int x1, int y1,int colour){
  int dx = abs(x1-x0);
  int sx = x0<x1 ? 1 : -1;
  int dy = abs(y1-y0);
  int sy = y0<y1 ? 1 : -1;
  int err = (dx>dy ? dx : -dy)/2;
  int e2;
  
  for(;;){
    pbmPic->image[y0][x0] = colour;
    if (x0==x1 && y0==y1){
          break;
    } 
    e2 = err;
    if (e2 >-dx) { 
      err -= dy; 
      x0 += sx; 
    }
    if (e2 < dy) { 
      err += dx; 
      y0 += sy; 
    }
  }
}

void drawX(struct PBM_Image * pbmPic,int topLeftX,int topLeftY, int recWidth, int recHeight,int color){

  //Draw First line
  drawLine(pbmPic,topLeftX,topLeftY,topLeftX+recWidth-1,topLeftY+recHeight-1,color);
  //Draw Second X
  drawLine(pbmPic,topLeftX,topLeftY+recHeight-1,topLeftX+recWidth-1,topLeftY,color);

}
