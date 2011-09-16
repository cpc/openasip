unsigned int chroma_in0_32b = 0xF1234567;
unsigned int result, result2;

int main() {

  unsigned char A1_Cb, A1_Cr;

  A1_Cb = (chroma_in0_32b>>24)&0x000000FF;
  A1_Cr = (chroma_in0_32b>>16)&0x000000FF;
  
  _TCEFU_SUB("ALU6", A1_Cb, A1_Cr, result);
  _TCEFU_ADD("ALU8", A1_Cb, A1_Cr, result2);
  return result;

}
