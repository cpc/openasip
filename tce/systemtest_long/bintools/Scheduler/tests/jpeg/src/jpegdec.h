

#ifndef __JPEGDEC_H__
#define __JPEGDEC_H__ 

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
*/

#define BYTE unsigned char
#define WORD unsigned short int

#define DWORD unsigned int
#define SDWORD signed int

#define SBYTE signed char
#define SWORD signed short int

int load_JPEG_header(char* buffer, int bufSize, DWORD *X_image, DWORD *Y_image);
void decode_JPEG_image();
int get_JPEG_buffer(WORD X_image,WORD Y_image, BYTE **address_dest_buffer);

#endif
