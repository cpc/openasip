/* A Simple OpenCL Kernel example from OpenCL specs  -- the host code */
#include <CL/cl.h>
#include <stdlib.h>
#include <stdio.h>


//#define VERIFICATION
//#define DEBUG

/* OpenCL API implementations end */

void
delete_memobjs(cl_mem *memobjs, int n)
{
    int i;
    for (i=0; i<n; i++)
        clReleaseMemObject(memobjs[i]);
}

int
exec_dot_product_kernel(const char *program_source,
                        int n, void *srcA, void *srcB, void *dst)
{
    cl_context context;

    cl_command_queue cmd_queue;
    cl_device_id *devices;
    cl_program program;
    cl_kernel kernel;
    cl_mem memobjs[3];
    size_t global_work_size[1];
    size_t local_work_size[1];
    size_t cb;
    cl_int err;
// create the OpenCL context on a GPU device
    context = clCreateContextFromType(NULL, CL_DEVICE_TYPE_GPU,
                                      NULL, NULL, NULL);
    if (context == (cl_context)0)
        return -1;
// get the list of GPU devices associated with context
    clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &cb);
    cb = 0;
    devices = (_cl_device_id**)malloc(cb);
    clGetContextInfo(context, CL_CONTEXT_DEVICES, cb, devices, NULL);
// create a command-queue
    cmd_queue = clCreateCommandQueue(context, devices[0], 0, NULL);
    if (cmd_queue == (cl_command_queue)0)
    {
        clReleaseContext(context);
        return -2;
    }
    //free(devices);
// allocate the buffer memory objects
    memobjs[0] = clCreateBuffer(context,
                                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                sizeof(cl_float4) * n, srcA, NULL);
    if (memobjs[0] == (cl_mem)0)
    {
        clReleaseCommandQueue(cmd_queue);
        clReleaseContext(context);
        return -3;
    }
    memobjs[1] = clCreateBuffer(context,
                                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                sizeof(cl_float4) * n, srcB, NULL);
    if (memobjs[1] == (cl_mem)0)
    {
        delete_memobjs(memobjs, 1);
        clReleaseCommandQueue(cmd_queue);
        clReleaseContext(context);
        return -4;
    }

    memobjs[2] = clCreateBuffer(context,
                                CL_MEM_READ_WRITE,
                                sizeof(cl_float) * n, NULL, NULL);
    if (memobjs[2] == (cl_mem)0)
    {
        delete_memobjs(memobjs, 2);
        clReleaseCommandQueue(cmd_queue);
        clReleaseContext(context);
        return -5;
    }
// create the program
    program = clCreateProgramWithSource(context,
                                        1, (const char**)&program_source, NULL, NULL);
//     if (program == (cl_program)0)
//     {
//         delete_memobjs(memobjs, 3);
//         clReleaseCommandQueue(cmd_queue);
//         clReleaseContext(context);
//         return -6;
//     }
// build the program
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        delete_memobjs(memobjs, 3);
        clReleaseProgram(program);
        clReleaseCommandQueue(cmd_queue);
        clReleaseContext(context);
        return -7;
    }
// create the kernel
    kernel = clCreateKernel(program, "dot_product", NULL);
    if (kernel == (cl_kernel)0)
    {
        delete_memobjs(memobjs, 3);
        clReleaseProgram(program);
        clReleaseCommandQueue(cmd_queue);
        clReleaseContext(context);
        return -8;
    }
// set the args values
    err = clSetKernelArg(kernel, 0,
                         sizeof(cl_mem), (void *) &memobjs[0]);
    err |= clSetKernelArg(kernel, 1,
                          sizeof(cl_mem), (void *) &memobjs[1]);
    err |= clSetKernelArg(kernel, 2,
                          sizeof(cl_mem), (void *) &memobjs[2]);

    if (err != CL_SUCCESS)
    {
        delete_memobjs(memobjs, 3);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(cmd_queue);
        clReleaseContext(context);
        return -9;
    }
// set work-item dimensions
    global_work_size[0] = n; /* n work groups with 1 work item per group ?*/
    local_work_size[0]= 4; /* only single work item per work group ?*/
// execute kernel
    err = clEnqueueNDRangeKernel(cmd_queue, kernel, 1, NULL,
                                 global_work_size, local_work_size,
                                 0, NULL, NULL);

    if (err != CL_SUCCESS)
    {
        delete_memobjs(memobjs, 3);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(cmd_queue);
        clReleaseContext(context);
        return -10;
    }
// read output image
    err = clEnqueueReadBuffer(cmd_queue, memobjs[2], CL_TRUE,
                              0, n * sizeof(cl_float), dst,
                              0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        delete_memobjs(memobjs, 3);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(cmd_queue);
        clReleaseContext(context);
        return -11;
    }
// release kernel, program, and memory objects
    delete_memobjs(memobjs, 3);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmd_queue);
    clReleaseContext(context);
    return 0; // success...
}


#define TEST_SET_SIZE 16

volatile cl_float4 inputA[TEST_SET_SIZE];
volatile cl_float4 inputB[TEST_SET_SIZE];
volatile cl_float results[TEST_SET_SIZE];

/* TODO: global constructor call injection breaks
   in case main is inlined as the context is not 
   saved before calling it at the moment and inlining
   brings in live variables. FIXME by doing the calls
   as LLVM Instruction level after which proper
   context saving code will be generated automatically. */
int main() {  

    for (int i = 0; i < TEST_SET_SIZE; ++i) {
        inputA[i][0] = i;
        inputA[i][1] = i;
        inputA[i][2] = i;
        inputA[i][3] = i;

        inputB[i][0] = 1;
        inputB[i][1] = 2;
        inputB[i][2] = 3;
        inputB[i][3] = 4;
    }

    int retVal = exec_dot_product_kernel(
        NULL, TEST_SET_SIZE, (void*)inputA, 
        (void*)inputB, (void*)results);


#ifdef DEBUG
    if (retVal != 0)
        printf("ERROR: %d\n", retVal);
#endif

#ifdef VERIFICATION
    /* verify results */

    for (int i = 0; i < TEST_SET_SIZE; ++i) {

        cl_float expected = 
            inputA[i][0]*inputB[i][0] +
            inputA[i][1]*inputB[i][1] +
            inputA[i][2]*inputB[i][2] +
            inputA[i][3]*inputB[i][3];

        printf("result[%d]: (%f, %f, %f, %f) dot (%f, %f, %f, %f) = %f ",
               i, 
               inputA[i][0], inputA[i][1], inputA[i][2], inputA[i][3], 
               inputB[i][0], inputB[i][1], inputB[i][2], inputB[i][3], 
               results[i]);

        if (results[i] == expected)
            printf("OK\n");
        else
            printf("%f != %f NOK\n", results[i], expected);
    }
#endif
    return retVal;
}
