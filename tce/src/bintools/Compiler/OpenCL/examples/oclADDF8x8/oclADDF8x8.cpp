#include <stdlib.h>
#include <CL/cl.h>

#define N 16

int
main() {
    cl_context context;
    cl_mem dst, src_a, src_b;
    cl_program program;
    cl_kernel kernel;
    size_t global_work_size[2];
    size_t local_work_size[2];
    cl_command_queue cmd_queue;
    cl_int err;

    float *input_a = (float *) malloc(sizeof(float) * N * N);
    float *input_b = (float *) malloc(sizeof(float) * N * N);

    dst = clCreateBuffer(context,
                         CL_MEM_READ_WRITE, 
                         sizeof(cl_float) * N * N, NULL, NULL);
    
    src_a = clCreateBuffer(context,
                           CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                           sizeof(cl_float) * N * N, input_a, NULL);

    src_b = clCreateBuffer(context,
                           CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
                           sizeof(cl_float) * N * N, input_b, NULL);

    kernel = clCreateKernel(program, "ADDF8x8", NULL);

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &dst);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *) &src_a);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *) &src_b);
    if (err != CL_SUCCESS) {
        clReleaseMemObject(dst);
        clReleaseMemObject(src_a);
        clReleaseMemObject(src_b);
        clReleaseKernel(kernel);
        return -1;
    }

    global_work_size[0] = N;
    global_work_size[1] = N;
    local_work_size[0] = 8;
    local_work_size[0] = 8;

    err = clEnqueueNDRangeKernel(cmd_queue, kernel, 2, NULL,
                                 global_work_size, local_work_size,
                                 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        clReleaseMemObject(dst);
        clReleaseMemObject(src_a);
        clReleaseMemObject(src_b);
        clReleaseKernel(kernel);
        return -1;
    }

    clReleaseMemObject(dst);
    clReleaseMemObject(src_a);
    clReleaseMemObject(src_b);
    clReleaseKernel(kernel);
    return 0;
}
                         
