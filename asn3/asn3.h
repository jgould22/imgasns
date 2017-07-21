#ifndef ASN3_HEADER_
# define ASN3_HEADER_
#include "libpnm.h"

struct searchBuffer
{ 
    int current_length;
    int max_length;
};

typedef struct token
{
    int offset;
    int length;
    unsigned char nextValue;
}encodeToken;

int main (int argc, char *argv[]);
void Encode_Using_LZ77(char *in_PGM_filename_Ptr, unsigned int searching_buffer_size, char
*compressed_filename_Ptr);
encodeToken* findToken (struct searchBuffer *searchWindow,unsigned char *imagePointer,long int imageArrayLength,long int searchPointer);
void Decode_Using_LZ77(char *in_compressed_filename_Ptr, char *out_PGM_filename_Ptr);
#endif
