__attribute__((reqd_work_group_size(8, 8, 1))) __kernel void
ADDF8x8(__global float *dst,
        __global float *src_a,
        __global float *src_b) {
    unsigned int x = get_local_id(0);
    unsigned int y = get_local_id(1);
    unsigned int width = get_local_size(0);

    dst += x + width * y;
    src_a += x + width * y;
    src_b += x + width * y;
    
    *dst = *src_a + *src_b;
}
                      
                     
