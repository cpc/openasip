#include <stdio.h>
#include "tceops.h"


unsigned int chroma_in0_32b;
unsigned int chroma_in1_32b;
unsigned int chroma_in2_32b;

unsigned int array[3]={0x9a8aba52, 0x99eb4a0c, 0xac9f4e29};


int main() {


	unsigned int chroma_in0_32b;
	unsigned int chroma_in1_32b;
	unsigned int chroma_in2_32b;

	unsigned char A0_Cb, A1_Cb, B0_Cb, B1_Cb, C0_Cb, C1_Cb;
	unsigned char A0_Cr, A1_Cr, B0_Cr, B1_Cr, C0_Cr, C1_Cr;	
	unsigned char P0_Cb, Q0_Cb;
	unsigned char P0_Cr, Q0_Cr;

	chroma_in0_32b = array[0];
	chroma_in1_32b = array[1];
	chroma_in2_32b = array[2];

	A1_Cb = (chroma_in0_32b&0xFF000000)>>24;
	A1_Cr = (chroma_in0_32b&0x00FF0000)>>16;
	B1_Cb = (chroma_in1_32b&0xFF000000)>>24;
	B1_Cr = (chroma_in1_32b&0x00FF0000)>>16;
	C1_Cb = (chroma_in2_32b&0xFF000000)>>24;
	C1_Cr = (chroma_in2_32b&0x00FF0000)>>16;

    /* If this line is added, bug is reproduced: */
	/* printf("\n B1_Cb: %x, B1_Cr: %x", B1_Cb, B1_Cr); */


	_TCE_MUL(B1_Cr, B1_Cb, P0_Cb);		
	_TCE_MUL(B1_Cr, B1_Cb, Q0_Cb);

	printf("\n A1_Cb: %x, A1_Cr: %x", A1_Cb, A1_Cr);
	printf("\n B1_Cb: %x, B1_Cr: %x", B1_Cb, B1_Cr); /* Prints: "B1_Cb: 0, B1_Cr: 0" */
	printf("\n C1_Cb: %x, C1_Cr: %x", C1_Cb, C1_Cr);
	

	return(0);

}


