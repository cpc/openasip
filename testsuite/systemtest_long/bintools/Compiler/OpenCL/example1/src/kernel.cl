/* D.1 A simple OpenCL Kernel example from OpenCL specs */

__attribute__((reqd_work_group_size(4, 1, 1)))
kernel void
dot_product (global const float4 *a,
             global const float4 *b, 
             global float *c) {
    int gid = get_global_id(0);
    c[gid] = dot(a[gid], b[gid]);    
} 
