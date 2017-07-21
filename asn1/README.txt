To compile on solaris/linux use "make linux" or "make solaris" command in ./src folder

The program, asn1 takes five arguments
o image type code (e.g.,1 for pbm, 2 for pgm, or 3 for ppm),
o image width,
o image height,
o output image name, and
o image format code (e.g., 0 for ASCII or 1 for raw).

For example
“1 500 500 500x500pbm.pbm 0" will produce a pbm image of width 500 
height 500 and name it 500x500pbm.pbm and save it in raw format
