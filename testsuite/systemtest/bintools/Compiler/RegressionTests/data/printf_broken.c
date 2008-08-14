#include <stdio.h>
#include "tceops.h"


unsigned int chroma_in0_32b;
unsigned int chroma_in1_32b;
unsigned int chroma_in2_32b;

unsigned int array[3]={0x9a8aba52, 0x99eb4a0c, 0xac9f4e29};

volatile unsigned char temp1, temp2, temp3;

int main() {


	unsigned int chroma_in0_32b;
	unsigned int chroma_in1_32b;
	unsigned int chroma_in2_32b;

	unsigned char P0_Cb, Q0_Cb;
	unsigned char P0_Cr, Q0_Cr;
    unsigned char A0_Cb, A1_Cb, B0_Cb, B1_Cb, C0_Cb, C1_Cb;
    unsigned char A0_Cr, A1_Cr, B0_Cr, B1_Cr, C0_Cr, C1_Cr;	


	chroma_in0_32b = array[0];
	chroma_in1_32b = array[1];
	chroma_in2_32b = array[2];

	A1_Cb = (chroma_in0_32b&0xFF000000)>>24;
	A1_Cr = (chroma_in0_32b&0x00FF0000)>>16;
	B1_Cb = (chroma_in1_32b&0xFF000000)>>24;
	B1_Cr = (chroma_in1_32b&0x00FF0000)>>16;
	C1_Cb = (chroma_in2_32b&0xFF000000)>>24;
	C1_Cr = (chroma_in2_32b&0x00FF0000)>>16;

    /* Remove one or both of the following and it starts working again: */
	_TCE_MUL(B1_Cr, B1_Cb, P0_Cb); 
	_TCE_MUL(B1_Cr, B1_Cb, Q0_Cb);  

    temp1 = B1_Cb;
    temp2 = B1_Cr; 

/* If this is enabled, it starts working. So probably a DCE issue of some kind.
  
    temp3 = P0_Cb*Q0_Cb;
*/
	return(0);
}


