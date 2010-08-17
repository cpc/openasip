/*
// JPEG decoder module
// Copyright 1999 Cristi Cuturicu
*/
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "jpegdec.h"
/* Used markers:*/
#define SOI 0xD8
#define EOI 0xD9
#define APP0 0xE0
#define SOF 0xC0
#define DQT 0xDB
#define DHT 0xC4
#define SOS 0xDA
#define DRI 0xDD
#define COM 0xFE

char error_string[90];
#define exit_func(err) { strcpy(error_string, err); return 0;}

static BYTE *buf; /* the buffer we use for storing the entire JPG file */

static BYTE bp; /*current byte*/
static WORD wp; /*current word*/

static DWORD byte_pos; /* current byte position */
#define BYTE_p(i) bp=buf[(i)++]
#define WORD_p(i) wp=(((WORD)(buf[(i)]))<<8) + buf[(i)+1]; (i)+=2
/*
// WORD X_image_size,Y_image_size; // X,Y sizes of the image
*/
static WORD X_round,Y_round; /* The dimensions rounded to multiple of Hmax*8 (X_round)
                             // and Ymax*8 (Y_round) */
    
    static BYTE *im_buffer; /* RGBA image buffer */
static DWORD X_image_bytes; /* size in bytes of 1 line of the image = X_round * 4 */
    static DWORD y_inc_value ; /* 32*X_round; // used by decode_MCU_1x2,2x1,2x2 */

    BYTE YH,YV,CbH,CbV,CrH,CrV; /* sampling factors (horizontal and vertical) for Y,Cb,Cr*/
static WORD Hmax,Vmax;


static BYTE zigzag[64]={ 0, 1, 5, 6,14,15,27,28,
				  2, 4, 7,13,16,26,29,42,
				  3, 8,12,17,25,30,41,43,
				  9,11,18,24,31,40,44,53,
				 10,19,23,32,39,45,52,54,
				 20,22,33,38,46,51,55,60,
				 21,34,37,47,50,56,59,61,
				 35,36,48,49,57,58,62,63 };
typedef struct {
    BYTE Length[17];  /* k =1-16 ; L[k] indicates the number of Huffman codes of length k*/
    WORD minor_code[17];  /* indicates the value of the smallest Huffman code of length k*/
    WORD major_code[17];  /* similar, but the highest code */
    BYTE V[65536];  /* V[k][j] = Value associated to the j-th Huffman code of length k */
	/* High nibble = nr of previous 0 coefficients */
	/* Low nibble = size (in bits) of the coefficient which will be taken from the data stream */
} Huffman_table;

/*static float *QT[4];  quantization tables, no more than 4 quantization tables (QT[0..3])*/

static int *QTI[4]; /* quantization tables, no more than 4 quantization tables (QT[0..3])*/
static Huffman_table HTDC[4]; /*DC huffman tables , no more than 4 (0..3)*/
static Huffman_table HTAC[4]; /*AC huffman tables                  (0..3)*/

static BYTE YQ_nr,CbQ_nr,CrQ_nr; /* quantization table number for Y, Cb, Cr*/
static BYTE YDC_nr,CbDC_nr,CrDC_nr; /* DC Huffman table number for Y,Cb, Cr*/
static BYTE YAC_nr,CbAC_nr,CrAC_nr; /* AC Huffman table number for Y,Cb, Cr*/

static BYTE Restart_markers; /* if 1 => Restart markers on , 0 => no restart markers*/
static WORD MCU_restart; /*Restart markers appears every MCU_restart MCU blocks*/
typedef void (*decode_MCU_func)(DWORD);


static SWORD DCY, DCCb, DCCr; /* Coeficientii DC pentru Y,Cb,Cr */
static SWORD DCT_coeff[64]; /* Current DCT_coefficients */
static BYTE Y[64],Cb[64],Cr[64]; /* Y, Cb, Cr of the current 8x8 block for the 1x1 case */
static BYTE Y_1[64],Y_2[64],Y_3[64],Y_4[64];
static BYTE tab_1[64],tab_2[64],tab_3[64],tab_4[64]; /* tabelele de supraesantionare pt cele 4 blocuri */

static SWORD Cr_tab[256],Cb_tab[256]; /* Precalculated Cr, Cb tables */
static SWORD Cr_Cb_green_tab[65536];

/*
// Initial conditions:
// byte_pos = start position in the Huffman coded segment
// WORD_get(w1); WORD_get(w2);wordval=w1;
*/
static BYTE d_k=0;  /* Bit displacement in memory, relative to the offset of w1 */
/* it's always <16 */
static WORD w1,w2; /* w1 = First word in memory; w2 = Second word */
static DWORD wordval ; /* the actual used value in Huffman decoding. */
static DWORD mask[17];
static SWORD neg_pow2[17]={0,-1,-3,-7,-15,-31,-63,-127,-255,-511,-1023,-2047,-4095,-8191,-16383,-32767};
/*static DWORD start_neg_pow2=(DWORD)neg_pow2; */

static int shift_temp;
#define RIGHT_SHIFT(x,shft)  \
	((shift_temp = (x)) < 0 ? \
	 (shift_temp >> (shft)) | ((~(0L)) << (32-(shft))) : \
	 (shift_temp >> (shft)))
#define DESCALE(x,n)  RIGHT_SHIFT((x) + (1L << ((n)-1)), n)

#define RANGE_MASK 1023L
static BYTE *rlimit_table;
void prepare_range_limit_table()
/* Allocate and fill in the sample_range_limit table */
{
  int j;
  rlimit_table = (BYTE *)malloc(5 * 256L + 128) ;
  /* First segment of "simple" table: limit[x] = 0 for x < 0 */
  memset((void *)rlimit_table,0,256);
  rlimit_table += 256;	/* allow negative subscripts of simple table */
  /* Main part of "simple" table: limit[x] = x */
  for (j = 0; j < 256; j++) rlimit_table[j] = j;
  /* End of simple table, rest of first half of post-IDCT table */
  for (j = 256; j < 640; j++) rlimit_table[j] = 255;
  /* Second half of post-IDCT table */
  memset((void *)(rlimit_table + 640),0,384);
  for (j = 0; j < 128 ; j++) rlimit_table[j+1024] = j;
}

#ifdef _MSC_VER
WORD lookKbits(BYTE k)
{
 _asm {
	 mov dl, k
	 mov cl, 16
	 sub cl, dl
	 mov eax, [wordval]
	 shr eax, cl
 }
}

WORD WORD_hi_lo(BYTE byte_high,BYTE byte_low)
{
	_asm {
		  mov ah,byte_high
		  mov al,byte_low
		 }
}
SWORD get_svalue(BYTE k)
// k>0 always
// Takes k bits out of the BIT stream (wordval), and makes them a signed value
{
	_asm {
		   xor ecx, ecx
		   mov cl,k
		   mov eax,[wordval]
		   shl eax,cl
		   shr eax, 16
		   dec cl
		   bt eax,ecx
		   jc end_macro
	signed_value:inc cl
		   mov ebx,[start_neg_pow2]
		   add ax,word ptr [ebx+ecx*2]
		 end_macro:
	}
}

#else

#ifdef __WATCOMC__

WORD lookKbits(BYTE k);
#pragma aux lookKbits=\
    "mov eax,[wordval]"\ ; //eax = *wordval;
"mov cl, 16"\ ; //cl = 16;
"sub cl, dl"\ ; //cl = 16 - k
                    "shr eax, cl"               \ ; //return (*wordval) >> (16-k);
					   parm [dl] \
					   value [ax] \
					   modify [eax cl];
WORD WORD_hi_lo(BYTE byte_high,BYTE BYTE_low);
#pragma aux WORD_hi_lo=\
			   parm [ah] [al]\
			   value [ax] \
    modify [ax]; //return (hi << 8) | (lo&0xFF);

SWORD get_svalue(BYTE k);
// k>0 always
// Takes k bits out of the BIT stream (wordval), and makes them a signed value
#pragma aux get_svalue=\
    "xor ecx, ecx"\ ; //ecx = 0;
"mov cl, al"\ ; //cl = al; cx = input
"mov eax,[wordval]"\ ; // eax = *wordval;
"shl eax, cl"\ // ; eax = (*wordval) << input
"shr eax, 16"\ // ; eax = ((*wordval)<< input) >> 16
"dec cl"\ ; // cx--
"bt eax,ecx"\; // is cx:th bit set?
"jc end_macro"\ // is set: return ax
"signed_value:inc cl"\ // cx++
"mov ebx,[start_neg_pow2]"\ ; //ebx = *start_neg_pow2;
"add ax,word ptr [ebx+ecx*2]"\ // ax += (*start_neg_pow2)[cx]
			"end_macro:"\
			parm [al]\
			modify [eax ebx ecx]\
			value [ax];

#else

WORD lookKbits(BYTE k) {
    return wordval >> (16-k);
}

WORD WORD_hi_lo(BYTE byte_high,BYTE BYTE_low) {
    return (byte_high<<8) | (BYTE_low&0xFF);
}

SWORD get_svalue(BYTE k) {
    BYTE counter = k;
    int val = (wordval << k) >> 16;
    counter--;
    if( val & (1 <<(counter))) {
        return val;
    } else {
        counter++;
        val += (neg_pow2[counter]);
        return val;
    }
}

#endif
#endif

void skipKbits(BYTE k)
{
 BYTE b_high,b_low;
 d_k+=k;
 if (d_k>=16) { d_k-=16;
		w1=w2;
		/* Get the next word in w2 */
		BYTE_p(byte_pos);
		if (bp!=0xFF) b_high=bp;
		else {
            if (buf[byte_pos]==0) byte_pos++; /*skip 00 */
            else byte_pos--; /* stop byte_pos pe restart marker */
			  b_high=0xFF;
		}
		BYTE_p(byte_pos);
		if (bp!=0xFF) b_low=bp;
		else {
            if (buf[byte_pos]==0) byte_pos++; /*skip 00 */
            else byte_pos--; /* stop byte_pos pe restart marker */
			  b_low=0xFF;
		}
		w2=WORD_hi_lo(b_high,b_low);
 }

 wordval = ((DWORD)(w1)<<16) + w2;
 wordval <<= d_k;
 wordval >>= 16;
}

SWORD getKbits(BYTE k)
{
 SWORD signed_wordvalue;
 signed_wordvalue=get_svalue(k);
 skipKbits(k);
 return signed_wordvalue;
}

void calculate_mask()
{
  BYTE k;
  DWORD tmpdv;
  for (k=0;k<=16;k++) { tmpdv=0x10000;mask[k]=(tmpdv>>k)-1;} /*precalculated bit mask */
}

void init_QT()
{
 BYTE i;
/* for (i=0;i<=3;i++) QT[i]=(float *)malloc(sizeof(float)*64); */
 for (i=0;i<=3;i++) QTI[i]=(int *)malloc(sizeof(int)*64);
}

/*void load_quant_table(float *quant_table)*/
void load_quant_table(int *quant_table)
{
/*
 float scalefactor[8]={1.0f, 1.387039845f, 1.306562965f, 1.175875602f,
					   1.0f, 0.785694958f, 0.541196100f, 0.275899379f};
*/
 int scalefactor_int[8] = {65536>>1, 90901>>1, 85627>>1, 77062>>1,
                           65536>>1, 51491>>1, 35467>>1, 18081>>1};

 BYTE j,row,col;
/*
// Load quantization coefficients from JPG file, scale them for DCT and reorder
// from zig-zag order
*/

 for (j=0;j<=63;j++) quant_table[j]=buf[byte_pos+zigzag[j]];
 j=0;
 for (row=0;row<=7;row++)
   for (col=0;col<=7;col++) {
/*		quant_table[j]*=scalefactor[row]*scalefactor[col];*/
       quant_table[j]*=((scalefactor_int[row]*scalefactor_int[col])>>14);
		j++;
   }
 byte_pos+=64;
}

void load_Huffman_table(Huffman_table *HT)
{
  BYTE k,j;
  DWORD code;

  for (j=1;j<=16;j++) {
	BYTE_p(byte_pos);
	HT->Length[j]=bp;
  }
  
  // DEBUG: struct alignment
  //if (HT == &HTDC[1]) { 
  //    int i;
  //    printf("HT->Length[3] (%i) == HTDC[3].Length[1] (%i)\n", HT->Length[3], HTDC[1].Length[3]);
  //    for (i=0; i < 17; i++) {
  //        printf("%i ", HT->Length[i]);
  //    }  
  //    printf("\n");
  //    for (i=0; i < 17; i++) {
  //        printf("%i ", HTDC[1].Length[i]);
  //    }
  //    printf("\n");
  //}

  for (k=1;k<=16;k++)
	for (j=0;j<HT->Length[k];j++) {
		BYTE_p(byte_pos);
		HT->V[WORD_hi_lo(k,j)]=bp;
	}

  code=0;
  for (k=1;k<=16;k++) {
	 HT->minor_code[k] = (WORD)code;
	 for (j=1;j<=HT->Length[k];j++) code++;
	 HT->major_code[k]=(WORD)(code-1);
	 code*=2;
	 if (HT->Length[k]==0) {
		HT->minor_code[k]=0xFFFF;
		HT->major_code[k]=0;
	 }
  }  
}

void process_Huffman_data_unit(BYTE DC_nr, BYTE AC_nr,SWORD *previous_DC)
{
/*
// Process one data unit. A data unit = 64 DCT coefficients
// Data is decompressed by Huffman decoding, then the array is dezigzag-ed
// The result is a 64 DCT coefficients array: DCT_coeff
*/
   BYTE nr,k,j,EOB_found;
   register WORD tmp_Hcode;
   BYTE size_val,count_0;
   WORD *min_code,*maj_code; /* min_code[k]=minimum code of length k, maj_code[k]=similar but maximum */
   WORD *max_val, *min_val;
   BYTE *huff_values;
   SWORD DCT_tcoeff[64];
   BYTE byte_temp;
/*
// Start Huffman decoding
// First the DC coefficient decoding
*/
   min_code=HTDC[DC_nr].minor_code;
   maj_code=HTDC[DC_nr].major_code;
   huff_values=HTDC[DC_nr].V;

   for (nr = 0; nr < 64 ; nr++) DCT_tcoeff[nr] = 0; /*Initialize DCT_tcoeff */

   nr=0;/* DC coefficient */

   min_val = &min_code[1]; max_val = &maj_code[1];
   for (k=1;k<=16;k++) {
	 tmp_Hcode=lookKbits(k);
/*	   max_val = &maj_code[k]; min_val = &min_code[k]; */
	 if ( (tmp_Hcode<=*max_val)&&(tmp_Hcode>=*min_val) ) { /*Found a valid Huffman code */
		skipKbits(k);
		size_val=huff_values[WORD_hi_lo(k,(BYTE)(tmp_Hcode-*min_val))];
		if (size_val==0) DCT_tcoeff[0]=*previous_DC;
		else {
			DCT_tcoeff[0]=*previous_DC+getKbits(size_val);
			*previous_DC=DCT_tcoeff[0];
		}
		break;
	 }
	 min_val++; max_val++;
   }

/* Second, AC coefficient decoding */
   min_code=HTAC[AC_nr].minor_code;
   maj_code=HTAC[AC_nr].major_code;
   huff_values=HTAC[AC_nr].V;

   nr=1; /* AC coefficient */
   EOB_found=0;
   while ( (nr<=63)&&(!EOB_found) )
	{
	 max_val = &maj_code[1]; min_val =&min_code[1];
	 for (k=1;k<=16;k++)
	 {
	   tmp_Hcode=lookKbits(k);
/*	   max_val = &maj_code[k]; &min_val = min_code[k]; */
	   if ( (tmp_Hcode<=*max_val)&&(tmp_Hcode>=*min_val) )
	   {
		skipKbits(k);
		byte_temp=huff_values[WORD_hi_lo(k,(BYTE)(tmp_Hcode-*min_val))];
		size_val=byte_temp&0xF;
		count_0=byte_temp>>4;
		if (size_val==0) {if (count_0==0) EOB_found=1;
            else if (count_0==0xF) nr+=16; /*skip 16 zeroes */
						}
		else
		{
            nr+=count_0; /*skip count_0 zeroes */
		 DCT_tcoeff[nr++]=getKbits(size_val);
		}
		break;
	   }
	   min_val++; max_val++;
	 }
	 if (k>16) nr++;  /* This should not occur */
	}
   for (j=0;j<=63;j++) DCT_coeff[j]=DCT_tcoeff[zigzag[j]]; /* Et, voila ... DCT_coeff*/
}

void IDCT_transform(SWORD *incoeff,BYTE *outcoeff,BYTE Q_nr)
/* Fast float IDCT transform */
{
 BYTE x;
 SBYTE y;
 SWORD *inptr;
 BYTE *outptr;

 int workspace[64];
 int *wsptr;/*Workspace pointer */
/*
 float workspace[64];
 float *wsptr;//Workspace pointer
 float *quantptr; // Quantization table pointer */
 int *quantptr; /* Quantization table pointer */
/*
 float dcval;
 fkiat tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7;
 float tmp10,tmp11,tmp12,tmp13;
 float z5,z10,z11,z12,z13;
*/
 int dcval;
 int tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7;
 int tmp10,tmp11,tmp12,tmp13;
 int z5,z10,z11,z12,z13;

 BYTE *range_limit=rlimit_table+128;
 /* Pass 1: process COLUMNS from input and store into work array. */
 wsptr=workspace;
 inptr=incoeff;
/* quantptr=QT[Q_nr]; */
 quantptr=QTI[Q_nr];
 for (y=0;y<=7;y++)
  {
   if( (inptr[8]|inptr[16]|inptr[24]|inptr[32]|inptr[40]|inptr[48]|inptr[56])==0)
	{
        /* AC terms all zero (in a column) */
        dcval=(inptr[0]*quantptr[0]) >> 16;
	 wsptr[0]  = dcval;
	 wsptr[8]  = dcval;
	 wsptr[16] = dcval;
	 wsptr[24] = dcval;
	 wsptr[32] = dcval;
	 wsptr[40] = dcval;
	 wsptr[48] = dcval;
	 wsptr[56] = dcval;
	 inptr++;quantptr++;wsptr++;/*advance pointers to next column*/
	 continue ;
	}
   /*Even part */
   tmp0 = (inptr[0] *quantptr[0]) >> 16;
   tmp1 = (inptr[16]*quantptr[16]) >> 16;
   tmp2 = (inptr[32]*quantptr[32]) >> 16;
   tmp3 = (inptr[48]*quantptr[48]) >> 16;

	tmp10 = tmp0 + tmp2;/* phase 3 */
	tmp11 = tmp0 - tmp2;

	tmp13 = tmp1 + tmp3;/* phases 5-3 */


    tmp12 = (((tmp1 - tmp3) * 92680) >> 16 ) - tmp13;
/*	tmp12 = (tmp1 - tmp3) * 1.414213562f - tmp13; // 2*c4 */

	tmp0 = tmp10 + tmp13;/* phase 2 */
	tmp3 = tmp10 - tmp13;
	tmp1 = tmp11 + tmp12;
	tmp2 = tmp11 - tmp12;

	/* Odd part */
	tmp4 = (inptr[8] *quantptr[8]) >> 16;
    tmp5 = (inptr[24]*quantptr[24]) >> 16;
    tmp6 = (inptr[40]*quantptr[40]) >> 16;
    tmp7 = (inptr[56]*quantptr[56]) >> 16;
    
	z13 = tmp6 + tmp5;/* phase 6 */
	z10 = tmp6 - tmp5;
	z11 = tmp4 + tmp7;
	z12 = tmp4 - tmp7;
    
	tmp7 = z11 + z13;/* phase 5 */
/*	tmp11= (z11 - z13) * 1.414213562f; // 2*c4 */
	tmp11= (((z11 - z13)) * 92680) >> 16;
/*	z5 = (z10 + z12) * 1.847759065f;  2*c2 */

    z5 = ((z10 + z12) * 121094)>>16; /* 2*c2 */

/*	tmp10 = 1.082392200f * z12 - z5; // 2*(c2-c6) */
    tmp10 = ((70936 * z12)>>16) - z5; /* 2*(c2-c6) */

/*	tmp12 = -2.613125930f * z10 + z5;// -2*(c2+c6)*/

    tmp12 = ((-171254 * z10)>>16) + z5;

	tmp6 = tmp12 - tmp7;/* phase 2 */
	tmp5 = tmp11 - tmp6;
	tmp4 = tmp10 + tmp5;

	wsptr[0]  = tmp0 + tmp7;
	wsptr[56] = tmp0 - tmp7;
	wsptr[8]  = tmp1 + tmp6;
	wsptr[48] = tmp1 - tmp6;
	wsptr[16] = tmp2 + tmp5;
	wsptr[40] = tmp2 - tmp5;
	wsptr[32] = tmp3 + tmp4;
	wsptr[24] = tmp3 - tmp4;
	inptr++;
	quantptr++;
	wsptr++;/*advance pointers to the next column */
  }

/*
//  Pass 2: process ROWS from work array, store into output array.
// Note that we must descale the results by a factor of 8 = 2^3
*/
 outptr=outcoeff;
 wsptr=workspace;
 for (x=0;x<=7;x++)
  {
      /* Even part */
	tmp10 = wsptr[0] + wsptr[4];
	tmp11 = wsptr[0] - wsptr[4];

	tmp13 = wsptr[2] + wsptr[6];
/*	tmp12 =(wsptr[2] - wsptr[6]) * 1.414213562f - tmp13; */

	tmp12 =(((wsptr[2] - wsptr[6]) * 92680)>>16) - tmp13;
    

	tmp0 = tmp10 + tmp13;
	tmp3 = tmp10 - tmp13;
	tmp1 = tmp11 + tmp12;
	tmp2 = tmp11 - tmp12;

    /* Odd part */
	z13 = wsptr[5] + wsptr[3];
	z10 = wsptr[5] - wsptr[3];
	z11 = wsptr[1] + wsptr[7];
	z12 = wsptr[1] - wsptr[7];

	tmp7 = z11 + z13;
/*	tmp11= (z11 - z13) * 1.414213562f; */

	tmp11= ((z11 - z13) * 92680) >> 16;

/*	z5 = (z10 + z12) * 1.847759065f; // 2*c2 */
	z5 = ((z10 + z12) * 121095) >> 16; /* 2*c2 */
/*	tmp10 = 1.082392200f * z12 - z5;  // 2*(c2-c6) */
	tmp10 = ((70936 * z12) >> 16 ) - z5;  /* 2*(c2-c6) */
/*	tmp12 = -2.613125930f * z10 + z5; // -2*(c2+c6) */
	tmp12 = ((-171254 * z10)>>16) + z5; /* -2*(c2+c6) */

	tmp6 = tmp12 - tmp7;
	tmp5 = tmp11 - tmp6;
	tmp4 = tmp10 + tmp5;

    /* Final output stage: scale down by a factor of 8 */
	outptr[0] = range_limit[(DESCALE((int) ((tmp0 + tmp7)), 3)) & 1023L];
    outptr[7] = range_limit[(DESCALE((int) ((tmp0 - tmp7)), 3)) & 1023L];
	outptr[1] = range_limit[(DESCALE((int) ((tmp1 + tmp6)), 3)) & 1023L];
	outptr[6] = range_limit[(DESCALE((int) ((tmp1 - tmp6)), 3)) & 1023L];
	outptr[2] = range_limit[(DESCALE((int) ((tmp2 + tmp5)), 3)) & 1023L];
	outptr[5] = range_limit[(DESCALE((int) ((tmp2 - tmp5)), 3)) & 1023L];
	outptr[4] = range_limit[(DESCALE((int) ((tmp3 + tmp4)), 3)) & 1023L];
	outptr[3] = range_limit[(DESCALE((int) ((tmp3 - tmp4)), 3)) & 1023L];

	wsptr+=8;/*advance pointer to the next row */
	outptr+=8;
  }
}

void precalculate_Cr_Cb_tables()
{
 WORD k;
 WORD Cr_v,Cb_v;
/* for (k=0;k<=255;k++) Cr_tab[k]=(SWORD)((k-128.0)*1.402); */

 for (k=0;k<=255;k++) Cr_tab[k]=(SWORD)(((k-128)*91881)>>16); 

/* for (k=0;k<=255;k++) Cb_tab[k]=(SWORD)((k-128.0)*1.772);*/
 for (k=0;k<=255;k++) Cb_tab[k]=(SWORD)(((k-128)*116130)>>16);

 for (Cr_v=0;Cr_v<=255;Cr_v++)
  for (Cb_v=0;Cb_v<=255;Cb_v++)
/*   Cr_Cb_green_tab[((WORD)(Cr_v)<<8)+Cb_v]=(int)(-0.34414*(Cb_v-128.0)-0.71414*(Cr_v-128.0));*/
   Cr_Cb_green_tab[((WORD)(Cr_v)<<8)+Cb_v]=
/*       (int) */
       ((-22554*(Cb_v-128)) - ( 46802 * ( Cr_v-128))) >> 16;
/*       (-0.34414*(Cb_v-128.0)-0.71414*(Cr_v-128.0)); */

}

void convert_8x8_YCbCr_to_RGB(BYTE *Y, BYTE *Cb, BYTE *Cr, DWORD im_loc, DWORD X_image_bytes, BYTE *im_buffer)
/*
// Functia (ca optimizare) poate fi apelata si fara parametrii Y,Cb,Cr
// Stim ca va fi apelata doar in cazul 1x1
*/
{
  DWORD x,y;
  BYTE im_nr;
  BYTE *Y_val = Y, *Cb_val = Cb, *Cr_val = Cr;
  BYTE *ibuffer = im_buffer + im_loc;

  for (y=0;y<8;y++)
   {
	im_nr=0;
	for (x=0;x<8;x++)
	  {
          ibuffer[im_nr++] = rlimit_table[*Y_val + Cb_tab[*Cb_val]]; /*B */
          ibuffer[im_nr++] = rlimit_table[*Y_val + Cr_Cb_green_tab[WORD_hi_lo(*Cr_val,*Cb_val)]]; /*G */
	   ibuffer[im_nr++] = rlimit_table[*Y_val + Cr_tab[*Cr_val]]; /* R */
/*
// Monochrome display
	   im_buffer[im_nr++] = *Y_val;
	   im_buffer[im_nr++] = *Y_val;
	   im_buffer[im_nr++] = *Y_val;
*/
	   Y_val++; Cb_val++; Cr_val++; im_nr++;
	  }
	ibuffer+=X_image_bytes;
   }
}

void convert_8x8_YCbCr_to_RGB_tab(BYTE *Y, BYTE *Cb, BYTE *Cr, BYTE *tab, DWORD im_loc, DWORD X_image_bytes, BYTE *im_buffer)
/* Functia (ca optimizare) poate fi apelata si fara parametrii Cb,Cr*/
{
  DWORD x,y;
  BYTE nr, im_nr;
  BYTE Y_val,Cb_val,Cr_val;
  BYTE *ibuffer = im_buffer + im_loc;

  nr=0;
  for (y=0;y<8;y++)
   {
	im_nr=0;
	for (x=0;x<8;x++)
	  {
	   Y_val=Y[nr];
	   Cb_val=Cb[tab[nr]]; Cr_val=Cr[tab[nr]]; /* reindexare folosind tabelul
                                               // de supraesantionare precalculat */
                                                    ibuffer[im_nr++] = rlimit_table[Y_val + Cb_tab[Cb_val]]; /*B */
	   ibuffer[im_nr++] = rlimit_table[Y_val + Cr_Cb_green_tab[WORD_hi_lo(Cr_val,Cb_val)]]; /*G */
	   ibuffer[im_nr++] = rlimit_table[Y_val + Cr_tab[Cr_val]]; /* R */
	   nr++; im_nr++;
	  }
	ibuffer+=X_image_bytes;
   }
}

void calculate_tabs()
{
 BYTE tab_temp[256];
 BYTE x,y;

 /*  Tabelul de supraesantionare 16x16 */
 for (y=0;y<16;y++)
	 for (x=0;x<16;x++)
	   tab_temp[y*16+x] = (y/YV)* 8 + x/YH;

 /* Din el derivam tabelele corespunzatoare celor 4 blocuri de 8x8 pixeli*/
 for (y=0;y<8;y++)
	{
	 for (x=0;x<8;x++)
	  tab_1[y*8+x]=tab_temp[y*16+x];
	 for (x=8;x<16;x++)
	  tab_2[y*8+(x-8)]=tab_temp[y*16+x];
	}
 for (y=8;y<16;y++)
	{
	 for (x=0;x<8;x++)
	  tab_3[(y-8)*8+x]=tab_temp[y*16+x];
	 for (x=8;x<16;x++)
	  tab_4[(y-8)*8+(x-8)]=tab_temp[y*16+x];
	}
}


int init_JPG_decoding()
{
 byte_pos=0;
 init_QT();
 calculate_mask();
 prepare_range_limit_table();
 precalculate_Cr_Cb_tables();
 return 1; /*for future error check */
}

/*
DWORD filesize(FILE *fp)
{
 DWORD pos;
 DWORD pos_cur;
 pos_cur=ftell(fp);
 fseek(fp,0,SEEK_END);
 pos=ftell(fp);
 fseek(fp,pos_cur,SEEK_SET);
 return pos;
}
*/

/*int load_JPEG_header(FILE *fp, DWORD *X_image, DWORD *Y_image)*/

int load_JPEG_header(char* buffer, int bufSize, DWORD *X_image, DWORD *Y_image) 
{
 DWORD length_of_file;
 BYTE vers,units;
 WORD Xdensity,Ydensity,Xthumbnail,Ythumbnail;
 WORD length;
/* float *qtable; */
 int *iqtable; /* 16.16 fxp version of qtable */
 DWORD old_byte_pos;
 Huffman_table *htable;
 DWORD j;
 BYTE precision,comp_id,nr_components;
 BYTE QT_info,HT_info;
 BYTE SOS_found,SOF_found;

 length_of_file = bufSize;
/*filesize(fp); */
 buf=(BYTE *)malloc(length_of_file+4);
 if (buf==NULL) exit_func("Not enough memory for loading file");
/* fread(buf,length_of_file,1,fp);*/
 memcpy(buf, buffer, length_of_file);

 if ((buf[0]!=0xFF)||(buf[1]!=SOI)) exit_func("Not a JPG file ?\n");
 if ((buf[2]!=0xFF)||(buf[3]!=APP0)) exit_func("Invalid JPG file.");
 if ( (buf[6]!='J')||(buf[7]!='F')||(buf[8]!='I')||(buf[9]!='F')||
	  (buf[10]!=0) ) exit_func("Invalid JPG file.");

 init_JPG_decoding();
 byte_pos=11;

 BYTE_p(byte_pos);vers=bp;
 if (vers!=1) exit_func("JFIF version not supported");
 BYTE_p(byte_pos); /* vers_lo=bp; */
 BYTE_p(byte_pos);  units=bp;
/* if (units!=0) exit_func("JPG format not supported");
                 ;//	printf("units = %d\n", units); */;
 WORD_p(byte_pos); Xdensity=wp; WORD_p(byte_pos); Ydensity=wp;
/*
 if ((Xdensity!=1)||(Ydensity!=1)) exit_func("JPG format not supported"); 
	 ;  //{printf("X density = %d\n",Xdensity); printf("Y density = %d\n",Ydensity);}
                                   */;
 BYTE_p(byte_pos);Xthumbnail=bp;BYTE_p(byte_pos);Ythumbnail=bp;
 if ((Xthumbnail!=0)||(Ythumbnail!=0))
	exit_func(" Cannot process JFIF thumbnailed files\n");
 /* Start decoding process */
 SOS_found=0; SOF_found=0; Restart_markers=0;
 while ((byte_pos<length_of_file)&&!SOS_found)
 {
  BYTE_p(byte_pos);
  if (bp!=0xFF) continue;
  /* A marker was found */
  BYTE_p(byte_pos);
  switch(bp)
  {
  case DQT: WORD_p(byte_pos); length=wp; /* length of the DQT marker */
			 for (j=0;j<wp-2;)
				{
				 old_byte_pos=byte_pos;
				 BYTE_p(byte_pos); QT_info=bp;
				 if ((QT_info>>4)!=0)
				 exit_func("16 bit quantization table not supported");
				 iqtable=QTI[QT_info&0xF];
				 load_quant_table(iqtable);
				 j+=byte_pos-old_byte_pos;
				}
			 break;
   case DHT: WORD_p(byte_pos); length=wp;
			 for (j=0;j<wp-2;)
				{
				 old_byte_pos=byte_pos;
				 BYTE_p(byte_pos); HT_info=bp;
				 if ((HT_info&0x10)!=0) htable=&HTAC[HT_info&0xF];
				 else htable=&HTDC[HT_info&0xF];
				 load_Huffman_table(htable);
				 j+=byte_pos-old_byte_pos;
				}
			 break;
   case COM: WORD_p(byte_pos); length=wp;
			 byte_pos+=wp-2;
			 break;
   case DRI: Restart_markers=1;
       WORD_p(byte_pos); length=wp; /*should be = 4*/
			 WORD_p(byte_pos);  MCU_restart=wp;
			 if (MCU_restart==0) Restart_markers=0;
			 break;
  case SOF: WORD_p(byte_pos); length=wp; /*should be = 8+3*3=17 */
			 BYTE_p(byte_pos); precision=bp;
			 if (precision!=8) exit_func("Only 8 bit precision supported");
			 WORD_p(byte_pos); *Y_image=wp; WORD_p(byte_pos); *X_image=wp;
			 BYTE_p(byte_pos); nr_components=bp;
			 if (nr_components!=3) exit_func("Only truecolor JPGS supported");
			 for (j=1;j<=3;j++)
				{
				 BYTE_p(byte_pos); comp_id=bp;
				 if ((comp_id==0)||(comp_id>3)) exit_func("Only YCbCr format supported");
				 switch (comp_id)
					{
                    case 1: /* Y */
							BYTE_p(byte_pos); YH=bp>>4;YV=bp&0xF;
							BYTE_p(byte_pos); YQ_nr=bp;
							break;
                    case 2: /* Cb */
							BYTE_p(byte_pos); CbH=bp>>4;CbV=bp&0xF;
							BYTE_p(byte_pos); CbQ_nr=bp;
							break;
                    case 3: /* Cr */
							BYTE_p(byte_pos); CrH=bp>>4;CrV=bp&0xF;
							BYTE_p(byte_pos); CrQ_nr=bp;
							break;
					}
				}
			 SOF_found=1;
			 break;
  case SOS: WORD_p(byte_pos); length=wp; /*should be = 6+3*2=12 */
		 BYTE_p(byte_pos); nr_components=bp;
		 if (nr_components!=3) exit_func("Invalid SOS marker");
		 for (j=1;j<=3;j++)
		   {
			BYTE_p(byte_pos); comp_id=bp;
			if ((comp_id==0)||(comp_id>3)) exit_func("Only YCbCr format supported");
			switch (comp_id)
			{
            case 1: /* Y */
					BYTE_p(byte_pos); YDC_nr=bp>>4;YAC_nr=bp&0xF;
					break;
            case 2: /* Cb */
					BYTE_p(byte_pos); CbDC_nr=bp>>4;CbAC_nr=bp&0xF;
					break;
            case 3: /* Cr */
					BYTE_p(byte_pos); CrDC_nr=bp>>4;CrAC_nr=bp&0xF;
					break;
			}
		   }
		 BYTE_p(byte_pos); BYTE_p(byte_pos); BYTE_p(byte_pos); /* Skip 3 bytes */
		 SOS_found=1;
		 break;
   case 0xFF:
		 break; /* do nothing for 0xFFFF, sequence of consecutive 0xFF are for
                // filling purposes and should be ignored */
   default:  WORD_p(byte_pos); length=wp;
		 byte_pos+=wp-2; /*skip unknown marker */
		 break;
  }
 }
 if (!SOS_found) exit_func("Invalid JPG file. No SOS marker found.");
 if (!SOF_found) exit_func("Progressive JPEGs not supported");

 if ((CbH>YH)||(CrH>YH)) exit_func("Vertical sampling factor for Y should be >= sampling factor for Cb,Cr");
 if ((CbV>YV)||(CrV>YV)) exit_func("Horizontal sampling factor for Y should be >= sampling factor for Cb,Cr");

 if ((CbH>=2)||(CbV>=2)) exit_func("Cb sampling factors should be = 1");
 if ((CrV>=2)||(CrV>=2)) exit_func("Cr sampling factors should be = 1");
/*
// Restricting sampling factors for Y,Cb,Cr should give us 4 possible cases for sampling factors
// YHxYV,CbHxCbV,CrHxCrV: 2x2,1x1,1x1;  1x2,1x1,1x1; 2x1,1x1,1x1;
// and 1x1,1x1,1x1 = no upsampling needed
*/
 Hmax=YH,Vmax=YV;
 if ( *X_image%(Hmax*8)==0) X_round=*X_image; /* X_round = Multiple of Hmax*8 */
 else X_round=(*X_image/(Hmax*8)+1)*(Hmax*8);
 if ( *Y_image%(Vmax*8)==0) Y_round=*Y_image; /* Y_round = Multiple of Vmax*8 */
 else Y_round=(*Y_image/(Vmax*8)+1)*(Vmax*8);

 im_buffer=(BYTE *)malloc(X_round*Y_round*4);
 if (im_buffer==NULL) exit_func("Not enough memory for storing the JPEG image");

 return 1;
}

void resync()
/* byte_pos  = pozitionat pe restart marker */
{
 byte_pos+=2;
 BYTE_p(byte_pos);
 if (bp==0xFF) byte_pos++; /* skip 00 */
 w1=WORD_hi_lo(bp, 0);
 BYTE_p(byte_pos);
 if (bp==0xFF) byte_pos++; /* skip 00 */
 w1+=bp;
 BYTE_p(byte_pos);
 if (bp==0xFF) byte_pos++; /* skip 00 */
 w2=WORD_hi_lo(bp, 0);
 BYTE_p(byte_pos);
 if (bp==0xFF) byte_pos++; /* skip 00 */
 w2+=bp;
 wordval=w1; d_k=0; /* Reinit bitstream decoding */
 DCY=0; DCCb=0; DCCr=0; /* Init DC coefficients */
}

void decode_MCU_1x1(DWORD im_loc)
{
    /* Y */
 process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
 IDCT_transform(DCT_coeff,Y,YQ_nr);
 /* Cb */
 process_Huffman_data_unit(CbDC_nr,CbAC_nr,&DCCb);
 IDCT_transform(DCT_coeff,Cb,CbQ_nr);
 /* Cr */
 process_Huffman_data_unit(CrDC_nr,CrAC_nr,&DCCr);
 IDCT_transform(DCT_coeff,Cr,CrQ_nr);

 convert_8x8_YCbCr_to_RGB(Y,Cb,Cr,im_loc,X_image_bytes,im_buffer);
}
void decode_MCU_2x1(DWORD im_loc)
{
    /* Y */
 process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
 IDCT_transform(DCT_coeff,Y_1,YQ_nr);
 process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
 IDCT_transform(DCT_coeff,Y_2,YQ_nr);
 /* Cb */
 process_Huffman_data_unit(CbDC_nr,CbAC_nr,&DCCb);
 IDCT_transform(DCT_coeff,Cb,CbQ_nr);
 /* Cr */
 process_Huffman_data_unit(CrDC_nr,CrAC_nr,&DCCr);
 IDCT_transform(DCT_coeff,Cr,CrQ_nr);

 convert_8x8_YCbCr_to_RGB_tab(Y_1,Cb,Cr,tab_1,im_loc,X_image_bytes,im_buffer);
 convert_8x8_YCbCr_to_RGB_tab(Y_2,Cb,Cr,tab_2,im_loc+32,X_image_bytes,im_buffer);
}

void decode_MCU_2x2(DWORD im_loc)
{
    /* Y */
 process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
 IDCT_transform(DCT_coeff,Y_1,YQ_nr);
 process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
 IDCT_transform(DCT_coeff,Y_2,YQ_nr);
 process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
 IDCT_transform(DCT_coeff,Y_3,YQ_nr);
 process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
 IDCT_transform(DCT_coeff,Y_4,YQ_nr);
 /* Cb */
 process_Huffman_data_unit(CbDC_nr,CbAC_nr,&DCCb);
 IDCT_transform(DCT_coeff,Cb,CbQ_nr);
 /* Cr */
 process_Huffman_data_unit(CrDC_nr,CrAC_nr,&DCCr);
 IDCT_transform(DCT_coeff,Cr,CrQ_nr);

 convert_8x8_YCbCr_to_RGB_tab(Y_1,Cb,Cr,tab_1,im_loc,X_image_bytes,im_buffer);
 convert_8x8_YCbCr_to_RGB_tab(Y_2,Cb,Cr,tab_2,im_loc+32,X_image_bytes,im_buffer);
 convert_8x8_YCbCr_to_RGB_tab(Y_3,Cb,Cr,tab_3,im_loc+y_inc_value,X_image_bytes,im_buffer);
 convert_8x8_YCbCr_to_RGB_tab(Y_4,Cb,Cr,tab_4,im_loc+y_inc_value+32,X_image_bytes,im_buffer);
}

void decode_MCU_1x2(DWORD im_loc)
{
    /* Y */
 process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
 IDCT_transform(DCT_coeff,Y_1,YQ_nr);
 process_Huffman_data_unit(YDC_nr,YAC_nr,&DCY);
 IDCT_transform(DCT_coeff,Y_2,YQ_nr);
 /* Cb */
 process_Huffman_data_unit(CbDC_nr,CbAC_nr,&DCCb);
 IDCT_transform(DCT_coeff,Cb,CbQ_nr);
 /* Cr */
 process_Huffman_data_unit(CrDC_nr,CrAC_nr,&DCCr);
 IDCT_transform(DCT_coeff,Cr,CrQ_nr);

 convert_8x8_YCbCr_to_RGB_tab(Y_1,Cb,Cr,tab_1,im_loc,X_image_bytes,im_buffer);
 convert_8x8_YCbCr_to_RGB_tab(Y_2,Cb,Cr,tab_3,im_loc+y_inc_value,X_image_bytes,im_buffer);
}

void decode_JPEG_image()
{
 decode_MCU_func decode_MCU;

 WORD x_mcu_cnt,y_mcu_cnt;
 DWORD nr_mcu;
 WORD X_MCU_nr,Y_MCU_nr; /* Nr de MCU-uri */
 DWORD MCU_dim_x; /*dimensiunea in bufferul imagine a unui MCU pe axa x */
 DWORD im_loc_inc; /* = 7 * X_round * 4 sau 15*X_round*4; */
 DWORD im_loc; /*locatia in bufferul imagine */

 byte_pos-=2;
 resync();

 y_inc_value = 32*X_round;
 calculate_tabs(); /* Calcul tabele de supraesantionare, tinand cont de YH si YV */

 if ((YH==1)&&(YV==1)) decode_MCU=decode_MCU_1x1;
 else {
	   if (YH==2)
	   {
		if (YV==2) decode_MCU=decode_MCU_2x2;
		else decode_MCU=decode_MCU_2x1;
	   }
	   else decode_MCU=decode_MCU_1x2;
 }
 MCU_dim_x=Hmax*8*4;

 Y_MCU_nr=Y_round/(Vmax*8); /* nr of MCUs on Y axis */
 X_MCU_nr=X_round/(Hmax*8); /* nr of MCUs on X axis */

 X_image_bytes=X_round*4; im_loc_inc = (Vmax*8-1) * X_image_bytes;
 nr_mcu=0; im_loc=0; /* memory location of the current MCU */
 for (y_mcu_cnt=0;y_mcu_cnt<Y_MCU_nr;y_mcu_cnt++)
 {
  for (x_mcu_cnt=0;x_mcu_cnt<X_MCU_nr;x_mcu_cnt++)
   {
	decode_MCU(im_loc);
	if ((Restart_markers)&&((nr_mcu+1)%MCU_restart==0)) resync();
	nr_mcu++;
	im_loc+=MCU_dim_x;
   }
  im_loc+=im_loc_inc;
 }
}

int get_JPEG_buffer(WORD X_image,WORD Y_image, BYTE **address_dest_buffer)
{
 WORD y;
/* DWORD dest_loc=0; */
 BYTE *src_buffer=im_buffer;
 BYTE *dest_buffer_start, *dest_buffer;

 DWORD src_bytes_per_line=X_round*4;
 DWORD dest_bytes_per_line=X_image*4;


 if ((X_round==X_image)&&(Y_round==Y_image))
	*address_dest_buffer=im_buffer;
 else
 {
  dest_buffer_start = (BYTE *)malloc(X_image*Y_image*4);
  if (dest_buffer_start==NULL) exit_func("Not enough memory for storing the JPEG image");
  dest_buffer = dest_buffer_start;
  for (y=0;y<Y_image;y++) {
	   memcpy(dest_buffer,src_buffer,dest_bytes_per_line);
	   src_buffer+=src_bytes_per_line;
	   dest_buffer+=dest_bytes_per_line;
  }
 *address_dest_buffer=dest_buffer_start;
 free(im_buffer);
 }
/* release the buffer which contains the JPG file */
 free(buf);
 return 1;
}
