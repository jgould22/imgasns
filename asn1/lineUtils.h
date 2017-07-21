#ifndef LINE_U_HEADER_
# define LINE_U_HEADER_
#include "libpnm.h"

void drawLine(struct PBM_Image * pbmPic,int x0,int y0, int x1, int y1,int colour);
void drawLinePGM(struct PGM_Image * pbmPic,int x0,int y0, int x1, int y1,int greyLevel);
void drawX(struct PBM_Image * pbmPic,int topLeftX,int topLeftY, int recWidth, int recHeight,int colour);

#endif
