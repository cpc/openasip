#include <stdio.h>
#include "tceops.h"

unsigned int chroma_in0_32b = 0xF1234567;
unsigned int result;

int main() {

  unsigned char A1_Cb, A1_Cr;

  A1_Cb = (chroma_in0_32b>>24)&0x000000FF;      /* 0xF1 = 241 */
  A1_Cr = (chroma_in0_32b>>16)&0x000000FF;      /* 0x23 = 35 */


  _TCE_ADD(A1_Cb, A1_Cr, result);       /* 241*35 = 8435 = 0x20F3 */
  return result;
}
