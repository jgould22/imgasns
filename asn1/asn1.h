#ifndef ASN1_HEADER_
# define ASN1_HEADER_
#include "libpnm.h"

typedef struct InputParameters  {
  int imageType;
  int imageWidth;
  int imageLength;
  char* imageOutName;
  int outPutCode;
} InputParameters;

int main(int,char[]);
int setImageType(char[], int);
char* createFileName(char* fileName, char* fileType );
void progOne(InputParameters* p);
void progThree(InputParameters* p);


#endif
