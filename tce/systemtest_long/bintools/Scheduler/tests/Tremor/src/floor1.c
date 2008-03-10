/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis 'TREMOR' CODEC SOURCE CODE.   *
 *                                                                  *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis 'TREMOR' SOURCE CODE IS (C) COPYRIGHT 1994-2002    *
 * BY THE Xiph.Org FOUNDATION http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: floor backend 1 implementation

 ********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ogg.h"
#include "ivorbiscodec.h"
#include "codec_internal.h"
#include "registry.h"
#include "codebook.h"
#include "misc.h"

#include <stdio.h>

#define floor1_rangedB 140 /* floor 1 fixed at -140dB to 0dB range */

typedef struct {
  int sorted_index[VIF_POSIT+2];
  int forward_index[VIF_POSIT+2];
  int reverse_index[VIF_POSIT+2];
  
  int hineighbor[VIF_POSIT];
  int loneighbor[VIF_POSIT];
  int posts;

  int n;
  int quant_q;
  vorbis_info_floor1 *vi;

} vorbis_look_floor1;

/***********************************************/
 
static void floor1_free_info(vorbis_info_floor *i){
  vorbis_info_floor1 *info=(vorbis_info_floor1 *)i;
  if(info){
    memset(info,0,sizeof(*info));
    _ogg_free(info);
  }
}

static void floor1_free_look(vorbis_look_floor *i){
  vorbis_look_floor1 *look=(vorbis_look_floor1 *)i;
  if(look){
    memset(look,0,sizeof(*look));
    _ogg_free(look);
  }
}

static int ilog(unsigned int v){
  int ret=0;
  while(v){
    ret++;
    v>>=1;
  }
  return(ret);
}

static vorbis_info_floor *floor1_unpack (vorbis_info *vi,oggpack_buffer *opb){
  codec_setup_info     *ci=(codec_setup_info *)vi->codec_setup;
  int j,k,count=0,maxclass=-1,rangebits;

  vorbis_info_floor1 *info=(vorbis_info_floor1 *)_ogg_calloc(1,sizeof(*info));
  /* read partitions */
  info->partitions=oggpack_read(opb,5); /* only 0 to 31 legal */
  for(j=0;j<info->partitions;j++){
    info->partitionclass[j]=oggpack_read(opb,4); /* only 0 to 15 legal */
    if(maxclass<info->partitionclass[j])maxclass=info->partitionclass[j];
  }

  /* read partition classes */
  for(j=0;j<maxclass+1;j++){
    info->class_dim[j]=oggpack_read(opb,3)+1; /* 1 to 8 */
    info->class_subs[j]=oggpack_read(opb,2); /* 0,1,2,3 bits */
    if(info->class_subs[j]<0)
      goto err_out;
    if(info->class_subs[j])info->class_book[j]=oggpack_read(opb,8);
    if(info->class_book[j]<0 || info->class_book[j]>=ci->books)
      goto err_out;
    for(k=0;k<(1<<info->class_subs[j]);k++){
      info->class_subbook[j][k]=oggpack_read(opb,8)-1;
      if(info->class_subbook[j][k]<-1 || info->class_subbook[j][k]>=ci->books)
	goto err_out;
    }
  }

  /* read the post list */
  info->mult=oggpack_read(opb,2)+1;     /* only 1,2,3,4 legal now */ 
  rangebits=oggpack_read(opb,4);

  for(j=0,k=0;j<info->partitions;j++){
    count+=info->class_dim[info->partitionclass[j]]; 
    for(;k<count;k++){
      int t=info->postlist[k+2]=oggpack_read(opb,rangebits);
      if(t<0 || t>=(1<<rangebits))
	goto err_out;
    }
  }
  info->postlist[0]=0;
  info->postlist[1]=1<<rangebits;

  return(info);
  
 err_out:
  floor1_free_info(info);
  return(NULL);
}

static int icomp(const void *a,const void *b){
  return(**(int **)a-**(int **)b);
}

static vorbis_look_floor *floor1_look(vorbis_dsp_state *vd,vorbis_info_mode *mi,
                              vorbis_info_floor *in){

  int *sortpointer[VIF_POSIT+2];
  vorbis_info_floor1 *info=(vorbis_info_floor1 *)in;
  vorbis_look_floor1 *look=(vorbis_look_floor1 *)_ogg_calloc(1,sizeof(*look));
  int i,j,n=0;

  look->vi=info;
  look->n=info->postlist[1];
 
  /* we drop each position value in-between already decoded values,
     and use linear interpolation to predict each new value past the
     edges.  The positions are read in the order of the position
     list... we precompute the bounding positions in the lookup.  Of
     course, the neighbors can change (if a position is declined), but
     this is an initial mapping */

  for(i=0;i<info->partitions;i++)n+=info->class_dim[info->partitionclass[i]];
  n+=2;
  look->posts=n;

  /* also store a sorted position index */
  for(i=0;i<n;i++)sortpointer[i]=info->postlist+i;
  qsort(sortpointer,n,sizeof(*sortpointer),icomp);

  /* points from sort order back to range number */
  for(i=0;i<n;i++)look->forward_index[i]=sortpointer[i]-info->postlist;
  /* points from range order to sorted position */
  for(i=0;i<n;i++)look->reverse_index[look->forward_index[i]]=i;
  /* we actually need the post values too */
  for(i=0;i<n;i++)look->sorted_index[i]=info->postlist[look->forward_index[i]];
  
  /* quantize values to multiplier spec */
  switch(info->mult){
  case 1: /* 1024 -> 256 */
    look->quant_q=256;
    break;
  case 2: /* 1024 -> 128 */
    look->quant_q=128;
    break;
  case 3: /* 1024 -> 86 */
    look->quant_q=86;
    break;
  case 4: /* 1024 -> 64 */
    look->quant_q=64;
    break;
  }

  /* discover our neighbors for decode where we don't use fit flags
     (that would push the neighbors outward) */
  for(i=0;i<n-2;i++){
    int lo=0;
    int hi=1;
    int lx=0;
    int hx=look->n;
    int currentx=info->postlist[i+2];
    for(j=0;j<i+2;j++){
      int x=info->postlist[j];
      if(x>lx && x<currentx){
	lo=j;
	lx=x;
      }
      if(x<hx && x>currentx){
	hi=j;
	hx=x;
      }
    }
    look->loneighbor[i]=lo;
    look->hineighbor[i]=hi;
  }

  return(look);
}

static int render_point(int x0,int x1,int y0,int y1,int x){
  y0&=0x7fff; /* mask off flag */
  y1&=0x7fff;
    
  {
    int dy=y1-y0;
    int adx=x1-x0;
    int ady=abs(dy);
    int err=ady*(x-x0);
    
    int off=err/adx;
    if(dy<0)return(y0-off);
    return(y0+off);
  }
}

static ogg_int32_t FLOOR_fromdB_LOOKUP[256]={
  0x000000e5, 0x000000f4, 0x00000103, 0x00000114,
  0x00000126, 0x00000139, 0x0000014e, 0x00000163,
  0x0000017a, 0x00000193, 0x000001ad, 0x000001c9,
  0x000001e7, 0x00000206, 0x00000228, 0x0000024c,
  0x00000272, 0x0000029b, 0x000002c6, 0x000002f4,
  0x00000326, 0x0000035a, 0x00000392, 0x000003cd,
  0x0000040c, 0x00000450, 0x00000497, 0x000004e4,
  0x00000535, 0x0000058c, 0x000005e8, 0x0000064a,
  0x000006b3, 0x00000722, 0x00000799, 0x00000818,
  0x0000089e, 0x0000092e, 0x000009c6, 0x00000a69,
  0x00000b16, 0x00000bcf, 0x00000c93, 0x00000d64,
  0x00000e43, 0x00000f30, 0x0000102d, 0x0000113a,
  0x00001258, 0x0000138a, 0x000014cf, 0x00001629,
  0x0000179a, 0x00001922, 0x00001ac4, 0x00001c82,
  0x00001e5c, 0x00002055, 0x0000226f, 0x000024ac,
  0x0000270e, 0x00002997, 0x00002c4b, 0x00002f2c,
  0x0000323d, 0x00003581, 0x000038fb, 0x00003caf,
  0x000040a0, 0x000044d3, 0x0000494c, 0x00004e10,
  0x00005323, 0x0000588a, 0x00005e4b, 0x0000646b,
  0x00006af2, 0x000071e5, 0x0000794c, 0x0000812e,
  0x00008993, 0x00009283, 0x00009c09, 0x0000a62d,
  0x0000b0f9, 0x0000bc79, 0x0000c8b9, 0x0000d5c4,
  0x0000e3a9, 0x0000f274, 0x00010235, 0x000112fd,
  0x000124dc, 0x000137e4, 0x00014c29, 0x000161bf,
  0x000178bc, 0x00019137, 0x0001ab4a, 0x0001c70e,
  0x0001e4a1, 0x0002041f, 0x000225aa, 0x00024962,
  0x00026f6d, 0x000297f0, 0x0002c316, 0x0002f109,
  0x000321f9, 0x00035616, 0x00038d97, 0x0003c8b4,
  0x000407a7, 0x00044ab2, 0x00049218, 0x0004de23,
  0x00052f1e, 0x0005855c, 0x0005e135, 0x00064306,
  0x0006ab33, 0x00071a24, 0x0007904b, 0x00080e20,
  0x00089422, 0x000922da, 0x0009bad8, 0x000a5cb6,
  0x000b091a, 0x000bc0b1, 0x000c8436, 0x000d5471,
  0x000e3233, 0x000f1e5f, 0x001019e4, 0x001125c1,
  0x00124306, 0x001372d5, 0x0014b663, 0x00160ef7,
  0x00177df0, 0x001904c1, 0x001aa4f9, 0x001c603d,
  0x001e384f, 0x00202f0f, 0x0022467a, 0x002480b1,
  0x0026dff7, 0x002966b3, 0x002c1776, 0x002ef4fc,
  0x0032022d, 0x00354222, 0x0038b828, 0x003c67c2,
  0x004054ae, 0x004482e8, 0x0048f6af, 0x004db488,
  0x0052c142, 0x005821ff, 0x005ddc33, 0x0063f5b0,
  0x006a74a7, 0x00715faf, 0x0078bdce, 0x0080967f,
  0x0088f1ba, 0x0091d7f9, 0x009b5247, 0x00a56a41,
  0x00b02a27, 0x00bb9ce2, 0x00c7ce12, 0x00d4ca17,
  0x00e29e20, 0x00f15835, 0x0101074b, 0x0111bb4e,
  0x01238531, 0x01367704, 0x014aa402, 0x016020a7,
  0x017702c3, 0x018f6190, 0x01a955cb, 0x01c4f9cf,
  0x01e269a8, 0x0201c33b, 0x0223265a, 0x0246b4ea,
  0x026c9302, 0x0294e716, 0x02bfda13, 0x02ed9793,
  0x031e4e09, 0x03522ee4, 0x03896ed0, 0x03c445e2,
  0x0402efd6, 0x0445ac4b, 0x048cbefc, 0x04d87013,
  0x05290c67, 0x057ee5ca, 0x05da5364, 0x063bb204,
  0x06a36485, 0x0711d42b, 0x0787710e, 0x0804b299,
  0x088a17ef, 0x0918287e, 0x09af747c, 0x0a50957e,
  0x0afc2f19, 0x0bb2ef7f, 0x0c759034, 0x0d44d6ca,
  0x0e2195bc, 0x0f0cad0d, 0x10070b62, 0x1111aeea,
  0x122da66c, 0x135c120f, 0x149e24d9, 0x15f525b1,
  0x176270e3, 0x18e7794b, 0x1a85c9ae, 0x1c3f06d1,
  0x1e14f07d, 0x200963d7, 0x221e5ccd, 0x2455f870,
  0x26b2770b, 0x29363e2b, 0x2be3db5c, 0x2ebe06b6,
  0x31c7a55b, 0x3503ccd4, 0x3875c5aa, 0x3c210f44,
  0x4009632b, 0x4432b8cf, 0x48a149bc, 0x4d59959e,
  0x52606733, 0x57bad899, 0x5d6e593a, 0x6380b298,
  0x69f80e9a, 0x70dafda8, 0x78307d76, 0x7fffffff,
};

static void render_line(int x0,int x1,int y0,int y1,ogg_int32_t *d){
  int dy=y1-y0;
  int adx=x1-x0;
  int ady=abs(dy);
  int base=dy/adx;
  int sy=(dy<0?base-1:base+1);
  int x=x0;
  int y=y0;
  int err=0;

  ady-=abs(base*adx);

  d[x]= MULT31_SHIFT15(d[x],FLOOR_fromdB_LOOKUP[y]);

  while(++x<x1){
    err=err+ady;
    if(err>=adx){
      err-=adx;
      y+=sy;
    }else{
      y+=base;
    }
    d[x]= MULT31_SHIFT15(d[x],FLOOR_fromdB_LOOKUP[y]);
  }
}

static void *floor1_inverse1(vorbis_block *vb,vorbis_look_floor *in){
  vorbis_look_floor1 *look=(vorbis_look_floor1 *)in;
  vorbis_info_floor1 *info=look->vi;
  codec_setup_info   *ci=(codec_setup_info *)vb->vd->vi->codec_setup;
  
  int i,j,k;
  codebook *books=ci->fullbooks;   
  
  /* unpack wrapped/predicted values from stream */
  if(oggpack_read(&vb->opb,1)==1){
    int *fit_value=(int *)_vorbis_block_alloc(vb,(look->posts)*sizeof(*fit_value));
    
    fit_value[0]=oggpack_read(&vb->opb,ilog(look->quant_q-1));
    fit_value[1]=oggpack_read(&vb->opb,ilog(look->quant_q-1));
    
    /* partition by partition */
    /* partition by partition */
    for(i=0,j=2;i<info->partitions;i++){
      int classv=info->partitionclass[i];
      int cdim=info->class_dim[classv];
      int csubbits=info->class_subs[classv];
      int csub=1<<csubbits;
      int cval=0;

      /* decode the partition's first stage cascade value */
      if(csubbits){
	cval=vorbis_book_decode(books+info->class_book[classv],&vb->opb);

	if(cval==-1)goto eop;
      }

      for(k=0;k<cdim;k++){
	int book=info->class_subbook[classv][cval&(csub-1)];
	cval>>=csubbits;
	if(book>=0){
	  if((fit_value[j+k]=vorbis_book_decode(books+book,&vb->opb))==-1)
	    goto eop;
	}else{
	  fit_value[j+k]=0;
	}
      }
      j+=cdim;
    }

    /* unwrap positive values and reconsitute via linear interpolation */
    for(i=2;i<look->posts;i++){
      int predicted=render_point(info->postlist[look->loneighbor[i-2]],
				 info->postlist[look->hineighbor[i-2]],
				 fit_value[look->loneighbor[i-2]],
				 fit_value[look->hineighbor[i-2]],
				 info->postlist[i]);
      int hiroom=look->quant_q-predicted;
      int loroom=predicted;
      int room=(hiroom<loroom?hiroom:loroom)<<1;
      int val=fit_value[i];

      if(val){
	if(val>=room){
	  if(hiroom>loroom){
	    val = val-loroom;
	  }else{
	  val = -1-(val-hiroom);
	  }
	}else{
	  if(val&1){
	    val= -((val+1)>>1);
	  }else{
	    val>>=1;
	  }
	}

	fit_value[i]=val+predicted;
	fit_value[look->loneighbor[i-2]]&=0x7fff;
	fit_value[look->hineighbor[i-2]]&=0x7fff;

      }else{
	fit_value[i]=predicted|0x8000;
      }
	
    }

    return(fit_value);
  }
 eop:
  return(NULL);
}

static int floor1_inverse2(vorbis_block *vb,vorbis_look_floor *in,void *memo,
			  ogg_int32_t *out){
  vorbis_look_floor1 *look=(vorbis_look_floor1 *)in;
  vorbis_info_floor1 *info=look->vi;

  codec_setup_info   *ci=(codec_setup_info *)vb->vd->vi->codec_setup;
  int                  n=ci->blocksizes[vb->W]/2;
  int j;

  if(memo){
    /* render the lines */
    int *fit_value=(int *)memo;
    int hx=0;
    int lx=0;
    int ly=fit_value[0]*info->mult;
    for(j=1;j<look->posts;j++){
      int current=look->forward_index[j];
      int hy=fit_value[current]&0x7fff;
      if(hy==fit_value[current]){
	
	hy*=info->mult;
	hx=info->postlist[current];
	
	render_line(lx,hx,ly,hy,out);
	
	lx=hx;
	ly=hy;
      }
    }
    for(j=hx;j<n;j++)out[j]*=ly; /* be certain */    
    return(1);
  }
  memset(out,0,sizeof(*out)*n);
  return(0);
}

/* export hooks */
vorbis_func_floor floor1_exportbundle={
  &floor1_unpack,&floor1_look,&floor1_free_info,
  &floor1_free_look,&floor1_inverse1,&floor1_inverse2
};

