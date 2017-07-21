#ifndef RECUTILS_HEADER_
# define RECUTILS_HEADER_
#include "libpnm.h"


void drawRectangle(struct PBM_Image * pbmPic,int bottomLeftX,int bottomLeftY, int recWidth, int recHeight,int colour);
void drawRectanglePGM(struct PGM_Image * pbmPic,int bottomLeftX,int bottomLeftY, int recWidth, int recHeight,int colour);
void drawRectangleGradient(struct PPM_Image * ppmPic,int topLeftX,int topLeftY, int recWidth, int recHeight,int gradientColor,int maxGreyValue,int reverse);
void drawRectSideGradient(struct PGM_Image * pgmPic,int topLeftX,int topLeftY, int recWidth, int recHeight,int maxGreyValue,int reverse);
void drawEmptyRectangle(struct PGM_Image * pbmPic,int topLeftX,int topLeftY, int recWidth, int recHeight,int color);
void drawRectCenterGradient(struct PGM_Image * pgmPic,int topLeftX,int topLeftY, int recWidth, int recHeight,int maxGreyValue,int reverse);


#endif
