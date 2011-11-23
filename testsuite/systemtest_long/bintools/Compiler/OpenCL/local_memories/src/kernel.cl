/* A kernel that tests the local memory accesses with 
   a strange code that moves stuff to the local memory
   and then back ;)
 */
//int iprintf(const char *format, ...);

__attribute__((reqd_work_group_size(4, 1, 1)))
kernel void
dot_product (global const float4 *a,
             global const float4 *b, 
             global float *c,
             local float *temp1,
             local float *temp2) {

    int lid = get_local_id(0);
    int gid = get_global_id(0);

    temp1[lid] = dot(a[gid], b[gid]);
    barrier(CLK_LOCAL_MEM_FENCE);

    temp2[3 - lid] = temp1[lid];
    barrier(CLK_LOCAL_MEM_FENCE);

    temp1[lid] = temp2[3 - lid];
    barrier(CLK_LOCAL_MEM_FENCE);

    c[gid] = temp1[lid];
} 
