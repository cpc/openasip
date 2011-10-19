//#include "tceops.h"

/*
#define USE_MUL_GAL_COP
//#define USE_SUBSHIFT_COP
//#define USE_PACK_COP
*/

 /*----------------------------------------------------------------------------*/

#define byte0(a)	a >> 24
#define byte1(a)	(a & 0xFF0000) >> 16
#define byte2(a)	(a & 0xFF00) >> 8
#define byte3(a)	(a & 0xFF)

#define byte0_2_word(b, a) a = (a & 0xFFFFFF) | b << 24
#define byte1_2_word(b, a) a = (a & 0xFF00FFFF) | b << 16
#define byte2_2_word(b, a) a = (a & 0xFFFF00FF) | b << 8
#define byte3_2_word(b, a) a = (a & 0xFFFFFF00) | b

#ifdef USE_PACK_COP
	#define bytes2word(b0, b1, b2, b3, a) _TCE_PACK(b0, b1, b2, b3, a)
#else
	#define bytes2word(b0, b1, b2, b3, a) a = (b0 << 24) | (b1 << 16) | (b2 << 8) | (b3)
#endif
/*----------------------------------------------------------------------------*/

 
__constant unsigned int Alogtable[] = {
1, 3, 5, 15, 17, 51, 85, 255, 26, 46, 114, 150, 161, 248, 19, 53,
95, 225, 56, 72, 216, 115, 149, 164, 247, 2, 6, 10, 30, 34, 102, 170,
229, 52, 92, 228, 55, 89, 235, 38, 106, 190, 217, 112, 144, 171, 230, 49,
83, 245, 4, 12, 20, 60, 68, 204, 79, 209, 104, 184, 211, 110, 178, 205,
76, 212, 103, 169, 224, 59, 77, 215, 98, 166, 241, 8, 24, 40, 120, 136,
131, 158, 185, 208, 107, 189, 220, 127, 129, 152, 179, 206, 73, 219, 118, 154,
181, 196, 87, 249, 16, 48, 80, 240, 11, 29, 39, 105, 187, 214, 97, 163,
254, 25, 43, 125, 135, 146, 173, 236, 47, 113, 147, 174, 233, 32, 96, 160,
251, 22, 58, 78, 210, 109, 183, 194, 93, 231, 50, 86, 250, 21, 63, 65,
195, 94, 226, 61, 71, 201, 64, 192, 91, 237, 44, 116, 156, 191, 218, 117,
159, 186, 213, 100, 172, 239, 42, 126, 130, 157, 188, 223, 122, 142, 137, 128,
155, 182, 193, 88, 232, 35, 101, 175, 234, 37, 111, 177, 200, 67, 197, 84,
252, 31, 33, 99, 165, 244, 7, 9, 27, 45, 119, 153, 176, 203, 70, 202,
69, 207, 74, 222, 121, 139, 134, 145, 168, 227, 62, 66, 198, 81, 243, 14,
18, 54, 90, 238, 41, 123, 141, 140, 143, 138, 133, 148, 167, 242, 13, 23,
57, 75, 221, 124, 132, 151, 162, 253, 28, 36, 108, 180, 199, 82, 246, 1,
};
/*----------------------------------------------------------------------------*/
__constant unsigned int Logtable[] = {
0, 0, 25, 1, 50, 2, 26, 198, 75, 199, 27, 104, 51, 238, 223, 3,
100, 4, 224, 14, 52, 141, 129, 239, 76, 113, 8, 200, 248, 105, 28, 193,
125, 194, 29, 181, 249, 185, 39, 106, 77, 228, 166, 114, 154, 201, 9, 120,
101, 47, 138, 5, 33, 15, 225, 36, 18, 240, 130, 69, 53, 147, 218, 142,
150, 143, 219, 189, 54, 208, 206, 148, 19, 92, 210, 241, 64, 70, 131, 56,
102, 221, 253, 48, 191, 6, 139, 98, 179, 37, 226, 152, 34, 136, 145, 16,
126, 110, 72, 195, 163, 182, 30, 66, 58, 107, 40, 84, 250, 133, 61, 186,
43, 121, 10, 21, 155, 159, 94, 202, 78, 212, 172, 229, 243, 115, 167, 87,
175, 88, 168, 80, 244, 234, 214, 116, 79, 174, 233, 213, 231, 230, 173, 232,
44, 215, 117, 122, 235, 22, 11, 245, 89, 203, 95, 176, 156, 169, 81, 160,
127, 12, 246, 111, 23, 196, 73, 236, 216, 67, 31, 45, 164, 118, 123, 183,
204, 187, 62, 90, 251, 96, 177, 134, 59, 82, 161, 108, 170, 85, 41, 157,
151, 178, 135, 144, 97, 190, 220, 252, 188, 149, 207, 205, 55, 63, 91, 209,
83, 57, 132, 60, 65, 162, 109, 71, 20, 42, 158, 93, 86, 242, 211, 171,
68, 17, 146, 217, 35, 32, 46, 137, 180, 124, 184, 38, 119, 153, 227, 165,
103, 74, 237, 222, 197, 49, 254, 24, 13, 99, 140, 128, 192, 247, 112, 7,
};
/*----------------------------------------------------------------------------*/
__constant unsigned int Sen[] = {
    99, 124, 119, 123, 242, 107, 111, 197,
    48,   1, 103,  43, 254, 215, 171, 118,
    202, 130, 201, 125, 250,  89,  71, 240,
    173, 212, 162, 175, 156, 164, 114, 192,
    183, 253, 147,  38,  54,  63, 247, 204,
    52, 165, 229, 241, 113, 216,  49,  21,
    4, 199,  35, 195,  24, 150,   5, 154,
    7,  18, 128, 226, 235,  39, 178, 117,
    9, 131,  44,  26,  27, 110,  90, 160,
    82,  59, 214, 179,  41, 227,  47, 132,
    83, 209,   0, 237,  32, 252, 177,  91,
    106, 203, 190,  57,  74,  76,  88, 207,
    208, 239, 170, 251,  67,  77,  51, 133,
    69, 249,   2, 127,  80,  60, 159, 168,
    81, 163,  64, 143, 146, 157,  56, 245,
    188, 182, 218,  33,  16, 255, 243, 210,
    205,  12,  19, 236,  95, 151,  68,  23,
    196, 167, 126,  61, 100,  93,  25, 115,
    96, 129,  79, 220,  34,  42, 144, 136,
    70, 238, 184,  20, 222,  94,  11, 219,
    224,  50,  58,  10,  73,   6,  36,  92,
    194, 211, 172,  98, 145, 149, 228, 121,
    231, 200,  55, 109, 141, 213,  78, 169,
    108,  86, 244, 234, 101, 122, 174,   8,
    186, 120,  37,  46,  28, 166, 180, 198,
    232, 221, 116,  31,  75, 189, 139, 138,
    112,  62, 181, 102,  72,   3, 246,  14,
    97,  53,  87, 185, 134, 193,  29, 158,  
    225, 248, 152,  17, 105, 217, 142, 148, 
    155,  30, 135, 233, 206,  85,  40, 223, 
    140, 161, 137,  13, 191, 230,  66, 104, 
    65, 153,  45,  15, 176,  84, 187,  22   
};
/*----------------------------------------------------------------------------*/
__constant unsigned char Sde[] = {
82, 9, 106, 213, 48, 54, 165, 56, 191, 64, 163, 158, 129, 243, 215, 251,
124, 227, 57, 130, 155, 47, 255, 135, 52, 142, 67, 68, 196, 222, 233, 203,
84, 123, 148, 50, 166, 194, 35, 61, 238, 76, 149, 11, 66, 250, 195, 78,
8, 46, 161, 102, 40, 217, 36, 178, 118, 91, 162, 73, 109, 139, 209, 37,
114, 248, 246, 100, 134, 104, 152, 22, 212, 164, 92, 204, 93, 101, 182, 146,
108, 112, 72, 80, 253, 237, 185, 218, 94, 21, 70, 87, 167, 141, 157, 132,
144, 216, 171, 0, 140, 188, 211, 10, 247, 228, 88, 5, 184, 179, 69, 6,
208, 44, 30, 143, 202, 63, 15, 2, 193, 175, 189, 3, 1, 19, 138, 107,
58, 145, 17, 65, 79, 103, 220, 234, 151, 242, 207, 206, 240, 180, 230, 115,
150, 172, 116, 34, 231, 173, 53, 133, 226, 249, 55, 232, 28, 117, 223, 110,
71, 241, 26, 113, 29, 41, 197, 137, 111, 183, 98, 14, 170, 24, 190, 27,
252, 86, 62, 75, 198, 210, 121, 32, 154, 219, 192, 254, 120, 205, 90, 244,
31, 221, 168, 51, 136, 7, 199, 49, 177, 18, 16, 89, 39, 128, 236, 95,
96, 81, 127, 169, 25, 181, 74, 13, 45, 229, 122, 159, 147, 201, 156, 239,
160, 224, 59, 77, 174, 42, 245, 176, 200, 235, 187, 60, 131, 83, 153, 97,
23, 43, 4, 126, 186, 119, 214, 38, 225, 105, 20, 99, 85, 33, 12, 125,
};

void switchblockInt(unsigned int *block)
{
	unsigned int block_aux[4];
	
	bytes2word(byte0(block[0]),byte0(block[1]),byte0(block[2]),byte0(block[3]),block_aux[0]);
	bytes2word(byte1(block[0]),byte1(block[1]),byte1(block[2]),byte1(block[3]),block_aux[1]);
	bytes2word(byte2(block[0]),byte2(block[1]),byte2(block[2]),byte2(block[3]),block_aux[2]);
	bytes2word(byte3(block[0]),byte3(block[1]),byte3(block[2]),byte3(block[3]),block_aux[3]);
	
	block[0] = block_aux[0];
	block[1] = block_aux[1];
	block[2] = block_aux[2];
	block[3] = block_aux[3];
}

/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/

#ifdef cl_TCE_MUL_GAL
unsigned int
mul(unsigned int a, unsigned int b, __global unsigned int *Alogtable,__global unsigned int *Logtable)
{
	unsigned int ret;
    clMUL_GALTCE(a, b, ret);
	//_TCE_MUL_GAL(a,b,ret);
	return ret;
}
#else
#warning Not using MUL_GAL
unsigned int 
mul(unsigned int a, unsigned int b,__global unsigned int *Alogtable,__global unsigned int *Logtable) 
{
#if 0
unsigned int d, ret;

d = 0;
ret = 0;

d = (Logtable[a] + Logtable[b]);
d = d >= 255 ? d-255 : d;
ret = (a && b) ? Alogtable[d] : 0;

return ret; 
#else

/* Trying to reformulate so hopefully the if-converter (with its
   current limitations) will digest the thing better. */

unsigned int d, ret;

d = 0;
ret = 0;

d = (Logtable[a] + Logtable[b]);

if (d >= 255)
    d -= 255;


ret = Alogtable[d];
if (!(a && b))
    ret = 0;

return ret;


#endif
}
#endif

/*----------------------------------------------------------------------------*/

void MixColumInt(unsigned int *state,__global unsigned int *Alogtable,__global unsigned int *Logtable)
{
	unsigned int aux_state[4];
	unsigned int b0, b1, b2, b3;
	
	b0 = mul(2,byte0(state[0]),Alogtable,Logtable) ^ mul(3,byte0(state[1]),Alogtable,Logtable) ^ byte0(state[2]) ^ byte0(state[3]);
	b1 = mul(2,byte1(state[0]),Alogtable,Logtable) ^ mul(3,byte1(state[1]),Alogtable,Logtable) ^ byte1(state[2]) ^ byte1(state[3]);
	b2 = mul(2,byte2(state[0]),Alogtable,Logtable) ^ mul(3,byte2(state[1]),Alogtable,Logtable) ^ byte2(state[2]) ^ byte2(state[3]);
	b3 = mul(2,byte3(state[0]),Alogtable,Logtable) ^ mul(3,byte3(state[1]),Alogtable,Logtable) ^ byte3(state[2]) ^ byte3(state[3]);
	
	bytes2word(b0,b1,b2,b3,aux_state[0]);
	
	b0 = byte0(state[0]) ^mul(2,byte0(state[1]),Alogtable,Logtable) ^ mul(3,byte0(state[2]),Alogtable,Logtable) ^ byte0(state[3]);
	b1 = byte1(state[0]) ^mul(2,byte1(state[1]),Alogtable,Logtable) ^ mul(3,byte1(state[2]),Alogtable,Logtable) ^ byte1(state[3]);
	b2 = byte2(state[0]) ^mul(2,byte2(state[1]),Alogtable,Logtable) ^ mul(3,byte2(state[2]),Alogtable,Logtable) ^ byte2(state[3]);	
	b3 = byte3(state[0]) ^mul(2,byte3(state[1]),Alogtable,Logtable) ^ mul(3,byte3(state[2]),Alogtable,Logtable) ^ byte3(state[3]);
	
	bytes2word(b0,b1,b2,b3,aux_state[1]);	
	
	b0 = byte0(state[0]) ^ byte0(state[1]) ^mul(2,byte0(state[2]),Alogtable,Logtable) ^ mul(3,byte0(state[3]),Alogtable,Logtable);
	b1 = byte1(state[0]) ^ byte1(state[1]) ^mul(2,byte1(state[2]),Alogtable,Logtable) ^ mul(3,byte1(state[3]),Alogtable,Logtable);		
	b2 = byte2(state[0]) ^ byte2(state[1]) ^mul(2,byte2(state[2]),Alogtable,Logtable) ^ mul(3,byte2(state[3]),Alogtable,Logtable);
	b3 = byte3(state[0]) ^ byte3(state[1]) ^mul(2,byte3(state[2]),Alogtable,Logtable) ^ mul(3,byte3(state[3]),Alogtable,Logtable);	
	
	bytes2word(b0,b1,b2,b3,aux_state[2]);
	
	b0 = mul(3,byte0(state[0]),Alogtable,Logtable) ^ byte0(state[1]) ^ byte0(state[2]) ^ mul(2,byte0(state[3]),Alogtable,Logtable);
	b1 = mul(3,byte1(state[0]),Alogtable,Logtable) ^ byte1(state[1]) ^ byte1(state[2]) ^ mul(2,byte1(state[3]),Alogtable,Logtable);
	b2 = mul(3,byte2(state[0]),Alogtable,Logtable) ^ byte2(state[1]) ^ byte2(state[2]) ^ mul(2,byte2(state[3]),Alogtable,Logtable);
	b3 = mul(3,byte3(state[0]),Alogtable,Logtable) ^ byte3(state[1]) ^ byte3(state[2]) ^ mul(2,byte3(state[3]),Alogtable,Logtable);
	
	bytes2word(b0,b1,b2,b3,aux_state[3]);
	
	state[0] = aux_state[0];
	state[1] = aux_state[1];
	state[2] = aux_state[2];
	state[3] = aux_state[3];
		
}
/*----------------------------------------------------------------------------*/

void
AddRoundKey2Int(unsigned int *state, unsigned int *key, unsigned int round)
{
    state[0] ^= key[4*round];
    state[1] ^= key[4*round+1];
    state[2] ^= key[4*round+2];
    state[3] ^= key[4*round+3];
}

/*----------------------------------------------------------------------------*/

/*void ShiftRowsInt(unsigned int *state)
{
  unsigned int aux;
  aux = byte0(state[1]);
  byte0_2_word(byte1(state[1]),state[1]);
  byte1_2_word(byte2(state[1]),state[1]);
  byte2_2_word(byte3(state[1]),state[1]);
  byte3_2_word(aux,state[1]);
  
  aux = byte0(state[2]);
  byte0_2_word(byte2(state[2]),state[2]);
  byte2_2_word(aux,state[2]);
  aux = byte1(state[2]);
  byte1_2_word(byte3(state[2]),state[2]);
  byte3_2_word(aux,state[2]);
  
  aux = byte3(state[3]);
  byte3_2_word(byte2(state[3]),state[3]);
  byte2_2_word(byte1(state[3]),state[3]);
  byte1_2_word(byte0(state[3]),state[3]);
  byte0_2_word(aux,state[3]);
}

void 
SubBytesInt(unsigned int *state)
{
    byte0_2_word(Sen[byte0(state[0])],state[0]);
    byte1_2_word(Sen[byte1(state[0])],state[0]);
    byte2_2_word(Sen[byte2(state[0])],state[0]);
    byte3_2_word(Sen[byte3(state[0])],state[0]);
    byte0_2_word(Sen[byte0(state[1])],state[1]);
    byte1_2_word(Sen[byte1(state[1])],state[1]);
    byte2_2_word(Sen[byte2(state[1])],state[1]);
    byte3_2_word(Sen[byte3(state[1])],state[1]);
    byte0_2_word(Sen[byte0(state[2])],state[2]);
    byte1_2_word(Sen[byte1(state[2])],state[2]);
    byte2_2_word(Sen[byte2(state[2])],state[2]);
    byte3_2_word(Sen[byte3(state[2])],state[2]);
    byte0_2_word(Sen[byte0(state[3])],state[3]);
    byte1_2_word(Sen[byte1(state[3])],state[3]);
    byte2_2_word(Sen[byte2(state[3])],state[3]);
    byte3_2_word(Sen[byte3(state[3])],state[3]);
}
*/

void SubShiftInt(global unsigned int *state, __global unsigned int *Sen)
{
	unsigned int aux;
	//unsigned int b0, b1, b2, b3;
		
	bytes2word(Sen[byte0(state[0])],Sen[byte1(state[0])],Sen[byte2(state[0])],Sen[byte3(state[0])],state[0]);
	bytes2word(Sen[byte1(state[1])],Sen[byte2(state[1])],Sen[byte3(state[1])],Sen[byte0(state[1])],state[1]);
	bytes2word(Sen[byte2(state[2])],Sen[byte3(state[2])], Sen[byte0(state[2])], Sen[byte1(state[2])],state[2]);
	bytes2word(Sen[byte3(state[3])],Sen[byte0(state[3])],Sen[byte1(state[3])],Sen[byte2(state[3])], state[3]);	
}

/*----------------------------------------------------------------------------*/
 

void round_encInt(__global unsigned int *block, __global unsigned int *key, unsigned int round, global unsigned int *Alogtable, __global unsigned int *Logtable, __global unsigned int *Sen, __global unsigned int *Sde)
{
#ifdef cl_TCE_SUBSHIFT
	clSUBSHIFTTCE(block[0],block[1],block[2],block[3],block[0],block[1],block[2],block[3]);
#else
#warning Not using SUBSHIFT
	SubShiftInt(block,Sen);
#endif	

	MixColumInt(block,Alogtable,Logtable);
	AddRoundKey2Int(block,key, round);

}

/*----------------------------------------------------------------------------*/

void lround_encInt(__global unsigned int *block, __global unsigned int *key ,__global unsigned int *Alogtable, __global unsigned int *Logtable, __global unsigned int *Sen, __global unsigned int *Sde)
{
#ifdef cl_TCE_SUBSHIFT
	clSUBSHIFTTCE(block[0],block[1],block[2],block[3],block[0],block[1],block[2],block[3]);
#else
#warning Not using SUBSHIFT
	SubShiftInt(block,Sen);
#endif	
	AddRoundKey2Int(block,key, 10);

}


/*----------------------------------------------------------------------------*/

void
encrypt_aes2Int(unsigned int *block, __global unsigned int *expandedkey, __global unsigned int *Alogtable, __global unsigned int *Logtable, __global unsigned int *Sen, __global unsigned int *Sde)
{
int i = 0;
 
switchblockInt(block);
 
 

 
AddRoundKey2Int(block, expandedkey,0);
 

round_encInt(block, expandedkey, 1,Alogtable, Logtable, Sen, Sde);
round_encInt(block, expandedkey, 2,Alogtable, Logtable, Sen, Sde);
round_encInt(block, expandedkey, 3,Alogtable, Logtable, Sen, Sde);
round_encInt(block, expandedkey, 4,Alogtable, Logtable, Sen, Sde);
round_encInt(block, expandedkey, 5,Alogtable, Logtable, Sen, Sde);
round_encInt(block, expandedkey, 6,Alogtable, Logtable, Sen, Sde);
round_encInt(block, expandedkey, 7,Alogtable, Logtable, Sen, Sde);
round_encInt(block, expandedkey, 8,Alogtable, Logtable, Sen, Sde);
round_encInt(block, expandedkey, 9,Alogtable, Logtable, Sen, Sde);
lround_encInt(block, expandedkey,Alogtable, Logtable, Sen, Sde);
 


switchblockInt(block);
}

/*----------------------------------------------------------------------------*/


__attribute__((reqd_work_group_size(64, 1, 1)))
__kernel void Encrypt(__global const unsigned int* g_idata, __global unsigned int* g_odata, __global unsigned int* encrypt_key, __global unsigned int* _Alogtable, __global unsigned int* _Logtable, __global unsigned int* _Sen, __global unsigned int* _Sde) {
    
  unsigned int tid = get_global_id(0);  
  const unsigned int num_threads = get_global_size(0);
  	
  unsigned int g_odata_aux[4];
  
  int index_begin = tid*4;

/*   //copy key to aux_key */
/*   unsigned int aux_encrypt_ky[200]; */
/*   for(int i = 0; i < 200; i++) */
/*       aux_encrypt_key[i] = encrypt_key[i]; */
 
  //copy block to process
  for(int j = 0; j < 4; j++)
  {
      g_odata_aux[j] = g_idata[j + index_begin];
  }
    
  encrypt_aes2Int(g_odata_aux ,encrypt_key, _Alogtable, _Logtable, _Sen, _Sde);		
  
  //write back result
  for(int j = 0; j < 4; j++)
  {
      g_odata[j + index_begin] = g_odata_aux[j];
  }	
  
}

