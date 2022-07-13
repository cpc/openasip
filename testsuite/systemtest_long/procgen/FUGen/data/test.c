int main() {

   volatile int z, z2, z3, z4;
   volatile int add_res_x, add_res_y;
   _TCE_ADD(-1, 1, z);
   _TCE_ADD(-2, -2, z2);
   _TCE_AND(0xf2ad7235,0x43d43178, z3);
   _TCE_GT(0,0, z4);
   _TCE_GT(0,5, z4);
   _TCE_GT(5,0, z4);
   return 0;
}
