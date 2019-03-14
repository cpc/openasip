
#include "jpegdec.h"
/*#include <conio.h> */


#ifdef __TCE_V1__
#include "tceops.h"
#endif

#ifndef __TCE__
#include <stdio.h>
#else
#ifndef __TCE_V1__
#include "userdef.h"
#endif

void putchar(char c) {
#ifdef __TCE_V1__
    int ch = (int)c;
    _TCE_STDOUT(ch);
#else
    WRITETO(stdout.1, c);
#endif // __TCE_V1__
}

void put_data(char c) {
#ifdef __TCE_V1__
    int ch = (int)c;
    _TCE_OUTPUTDATA(ch);
#else
    WRITETO(outputdata.1, c);
#endif // __TCE_V1__
}

/* ugly byteorder-kludgefix */
void fwrite(void* data, int len, int foo, void* foo2) {
    int i;
#ifdef __BIG_ENDIAN__
    for( i = len -1; i>= 0; i-- ) {
        put_data(((char*)data)[i]);
    }
#else
    for( i = 0; i< len; i++ ) {
        put_data(((char*)data)[i]);
    }
#endif
}

void fwrite_nofix(void* data, int len, int foo, void* foo2) {
    int i;
    for( i = 0; i< len; i++ ) {
        put_data(((char*)data)[i]);
    }
}
#endif


/*#include <time.h> */

#include "testimg.h"

char *FileName="testimg.jpg";
extern char error_string[90];

typedef struct s_BM_header {
    WORD BMP_id ; /* 'B''M' */
    DWORD size; /* size in bytes of the BMP file */
    DWORD zero_res; /* 0 */
    DWORD offbits; /* 54 */
    DWORD biSize; /* 0x28 */
    DWORD Width;  /* X */
    DWORD Height;  /* Y */
    WORD  biPlanes; /* 1 */
    WORD  biBitCount ; /* 24 */
    DWORD biCompression; /* 0 = BI_RGB */
    DWORD biSizeImage; /* 0 */
    DWORD biXPelsPerMeter; /* 0xB40 */
    DWORD biYPelsPerMeter; /* 0xB40 */
    DWORD biClrUsed; /*0 */
    DWORD biClrImportant; /*0*/
} BM_header;
typedef struct s_RGB {
			 BYTE B;
		     BYTE G;
		     BYTE R;
} RGB;

void exitmessage(char *message)
{
#ifndef __TCE__
 printf("%s\n",message);exit(0);
#endif
}

void write_buf_to_BMP(BYTE *im_buffer, WORD X_bitmap, WORD Y_bitmap, char *BMPname)
{
 SWORD x,y;
/* RGB *pixel; */
 BM_header BH;
#ifndef __TCE__
 FILE *fp_bitmap;
#else
 void *fp_bitmap;
#endif
 DWORD im_loc_bytes;
 BYTE nr_fillingbytes, i;
 BYTE zero_byte=0;

#ifndef __TCE__
 fp_bitmap=fopen(BMPname,"wb");
 if (fp_bitmap==NULL) exitmessage("File cannot be created");
#endif

 if (X_bitmap%4!=0) nr_fillingbytes=4-((X_bitmap*3L)%4);
  else nr_fillingbytes=0;

 BH.BMP_id = 'M'*256+'B';     fwrite(&BH.BMP_id,2,1,fp_bitmap);
 BH.size=54+Y_bitmap*(X_bitmap*3+nr_fillingbytes);fwrite(&BH.size,4,1,fp_bitmap);
 BH.zero_res = 0;             fwrite(&BH.zero_res,4,1,fp_bitmap);
 BH.offbits = 54;             fwrite(&BH.offbits,4,1,fp_bitmap);
 BH.biSize = 0x28;            fwrite(&BH.biSize,4,1,fp_bitmap);
 BH.Width = X_bitmap;	      fwrite(&BH.Width,4,1,fp_bitmap);
 BH.Height = Y_bitmap;	      fwrite(&BH.Height,4,1,fp_bitmap);
 BH.biPlanes = 1;             fwrite(&BH.biPlanes,2,1,fp_bitmap);
 BH.biBitCount = 24;          fwrite(&BH.biBitCount,2,1,fp_bitmap);
 BH.biCompression = 0;        fwrite(&BH.biCompression,4,1,fp_bitmap);
 BH.biSizeImage = 0;          fwrite(&BH.biSizeImage,4,1,fp_bitmap);
 BH.biXPelsPerMeter = 0xB40;  fwrite(&BH.biXPelsPerMeter,4,1,fp_bitmap);
 BH.biYPelsPerMeter = 0xB40;  fwrite(&BH.biYPelsPerMeter,4,1,fp_bitmap);
 BH.biClrUsed = 0;	          fwrite(&BH.biClrUsed,4,1,fp_bitmap);
 BH.biClrImportant = 0;	      fwrite(&BH.biClrImportant,4,1,fp_bitmap);

#ifndef __TCE__
 printf("Writing bitmap ...\n");
#endif
 im_loc_bytes=(DWORD)im_buffer+((DWORD)Y_bitmap-1)*X_bitmap*4;

 for (y=0;y<Y_bitmap;y++)
  {
   for (x=0;x<X_bitmap;x++)
	{
/*	 pixel=(RGB *)im_loc_bytes; */
#ifndef __TCE__
	 fwrite(im_loc_bytes, 3, 1, fp_bitmap);
#else
	 fwrite_nofix((void*)im_loc_bytes, 3, 1, fp_bitmap);
#endif
	 im_loc_bytes+=4;
	}
   for (i=0;i<nr_fillingbytes;i++) {
	   fwrite(&zero_byte,1,1,fp_bitmap);
   }
   im_loc_bytes-=2L*X_bitmap*4;
  }
#ifndef __TCE__
 printf("Done.\n");
 fclose(fp_bitmap);
#endif

}

/* void main(int argc, char *argv[]) */
int main(int argc,char **argv,char **envp)
{
#ifndef __TCE__
 FILE *fp;
#endif
 DWORD X_image, Y_image;
 BYTE *our_image_buffer;
/*
 clock_t start_time, finish_time;
 float duration;
*/

/*
 if (argc<=1) fp=fopen(FileName,"rb");
  else fp=fopen(argv[1],"rb");
 if (fp==NULL) exitmessage("File not found ?");
*/
 if (!load_JPEG_header(initial_data,initial_data_size ,&X_image,&Y_image)) {exitmessage(error_string);return 0;}
/* fclose(fp); */
#ifndef __TCE__
 printf(" X_image = %d\n",X_image);
 printf(" Y_image = %d\n",Y_image);
#endif

/*
 printf("Sampling factors: \n");
 printf("Y  : H=%d,V=%d\n", YH,YV);
 printf("Cb : H=%d,V=%d\n", CbH,CbV);
 printf("Cr : H=%d,V=%d\n", CrH,CrV);
 printf("Restart markers  = %d\n", Restart_markers);
 printf("MCU restart = %d\n", MCU_restart);
 getch();
*/

#ifndef __TCE__
 printf("Decoding JPEG image...\n");
#endif
 /* main decoder */
/*
 start_time = clock();
*/
 decode_JPEG_image();

#ifndef __TCE__
 printf("Decoding finished.\n");
#endif
 /* 
 finish_time = clock();
 duration = (double)(finish_time - start_time) / CLK_TCK;
 printf( "Time elapsed: %2.1f seconds\n", duration );
*/
 if (!get_JPEG_buffer(X_image,Y_image,&our_image_buffer)) {exitmessage(error_string);return 0;}

 write_buf_to_BMP(our_image_buffer,X_image,Y_image, "image.bmp");
/* getch(); */
}












