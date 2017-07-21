#ifndef ASN2_HEADER_
# define ASN2_HEADER_
#include "libpnm.h"

struct node {
	int first_value;
	int second_value;
};

int main (int argc, char *argv[]);
long int *generate_pixel_frequency(struct PGM_Image *input_pgm_image, int
*number_of_non_zero_values_in_the_frequency_array);
struct node *generate_Huffman_nodes(long int *pixel_frequency, int max_gray_value, int
number_of_non_zero_values_in_the_frequency_array);
void get_lowest_values(int* first,int* second,long int *pixel_frequency,int arraySize);

#endif
