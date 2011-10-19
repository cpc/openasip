/*
    Copyright (c) 2002-2011 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file oclhost.cc
 *
 * Implementation of the host-side library of the TCE OpenCL support.
 *
 * @author Pekka J‰‰skel‰inen 2009-2011 (pjaaskel-no.spam-cs.tut.fi)
 */

#define __STDC_LIMIT_MACROS

#include "opencl_types.hh"
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

//#define DEBUG_OCL_HOST

#ifdef DEBUG_OCL_HOST
#include <stdio.h>
#endif

#include <CL/cl.h>
#include <string.h>
#include "cl_tce.h"

extern "C" {

//////////////////////////////////////////////////////////////////////////////
// Code for implementing the kernel registry
//////////////////////////////////////////////////////////////////////////////

// a single kernel call instance
struct _cl_kernel {
    char* name;
    void* args[_TCE_CL_DEVICE_MAX_PARAMETERS];
};

/* An array of callable OpenCL kernel functions. */
struct _OpenCLKernel* _opencl_kernel_registry[_TCE_CL_MAX_KERNELS];

/* Must be volatile otherwise LLVM removes the global constructor. */
volatile int _opencl_kernel_count = 0;

void 
_register_opencl_kernel(struct _OpenCLKernel* kernel) {
#ifdef DEBUG_OCL_HOST
    puts("registering kernel ");
    puts(kernel->name);
    puts("\n");
#endif
    _opencl_kernel_registry[_opencl_kernel_count++] = kernel;
}

static struct _OpenCLKernel* 
_find_opencl_kernel(cl_kernel kernel) { 
    int i = 0;
    for (; i < _opencl_kernel_count; ++i) {
        struct _OpenCLKernel* k = _opencl_kernel_registry[i];
        /* todo: the work group dimensions */
        if (strcmp(k->name, kernel->name) == 0) {
            return k;
        }
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////////
// OpenCL host API implementations
//////////////////////////////////////////////////////////////////////////////
cl_int
clGetContextInfo(
    cl_context         context, 
    cl_context_info    param_name, 
    size_t             param_value_size, 
    void *             param_value, 
    size_t *           param_value_size_ret) {
    /* TODO */
    return CL_SUCCESS;
}

/**
 * A dummy function for creating a context.
 *
 * @todo Currently we do not need to store any data in the cl_context,
 *       so it does not matter what we return here.
 */
cl_context
clCreateContextFromType(
    cl_context_properties* properties,
    cl_device_type         device_type,
    void (*pfn_notify)(const char *, const void *, size_t, void *),
    void *                 user_data,
    cl_int *               errcode_ret) {
    return (cl_context)32000;
}

/**
 * A dummy function for creating a context.
 *
 * @todo Currently we do not need to store any data in the cl_context,
 *       so it does not matter what we return here.
 */
cl_context 
clCreateContext(
    cl_context_properties * /* properties */,
    cl_uint                 /* num_devices */,
    const cl_device_id *    /* devices */,
    void (*pfn_notify)(const char *, const void *, size_t, void *),
    void *                  /* user_data */,
    cl_int *                /* errcode_ret */) {
    return (cl_context)32000;
}

/**
 * Runtime building of kernels not currently supported so this
 * always returns CL_BUILD_NONE.
 */
cl_int
clGetProgramBuildInfo(
    cl_program            /* program */,
    cl_device_id          /* device */,
    cl_program_build_info /* param_name */,
    size_t                /* param_value_size */,
    void *                /* param_value */,
    size_t *              /* param_value_size_ret */) {
    return CL_BUILD_NONE;
}                          

cl_int
clReleaseMemObject(cl_mem memobj) {
    return CL_SUCCESS;
}

cl_mem
clCreateBuffer(
    cl_context   context,
    cl_mem_flags flags,
    size_t       size,
    void *       host_ptr,
    cl_int *     errcode_ret) {

    cl_mem mem = (cl_mem)malloc(size);

    if (mem == NULL) {
        if (errcode_ret != NULL)
            *errcode_ret = CL_OUT_OF_HOST_MEMORY;
        return NULL;
    } else {
        if (errcode_ret != NULL)
            *errcode_ret = CL_SUCCESS;
    }

    if (flags & CL_MEM_COPY_HOST_PTR && host_ptr != NULL)
    {
#ifdef DEBUG_OCL_HOST
        iprintf(
            "clCreateBuffer: copying %#x bytes from %p to %p\n", 
            size, host_ptr, mem);
#endif
        memcpy((void*)mem, host_ptr, size);
    }
    return mem;
}

cl_command_queue
clCreateCommandQueue(
    cl_context                     context, 
    cl_device_id                   device, 
    cl_command_queue_properties    properties,
    cl_int *                       errcode_ret) {
    cl_command_queue queue = (cl_command_queue)0x100;
    if (errcode_ret != NULL)
        *errcode_ret = CL_SUCCESS;
    return queue;
}

cl_kernel
clCreateKernel(
    cl_program      program, 
    const char *    kernel_name,
    cl_int *        errcode_ret) {

    int i = 0;
    cl_kernel kernel = (cl_kernel)malloc(sizeof(struct _cl_kernel));

    if (kernel == NULL) {
        if (errcode_ret != NULL)
            *errcode_ret = CL_OUT_OF_HOST_MEMORY;
        return NULL;
    }

    /* copy the name string and its trailing zero */
    kernel->name = (char*)malloc(strlen(kernel_name) + 1); 

    if (kernel->name == NULL) {
        if (errcode_ret != NULL)
            *errcode_ret = CL_OUT_OF_HOST_MEMORY;
        return NULL;
    }

    memcpy(kernel->name, kernel_name, strlen(kernel_name) + 1);
    for (i = 0; i < _TCE_CL_DEVICE_MAX_PARAMETERS; ++i) {
        kernel->args[i] = NULL;
    }
    return kernel;
}

cl_program 
clCreateProgramWithSource(
    cl_context        context,
    cl_uint           count,
    const char **     strings,
    const size_t *    lengths,
    cl_int *          errcode_ret) {

    cl_program program = (cl_program)malloc(sizeof(cl_program));
    if (errcode_ret != NULL)
        *errcode_ret = CL_SUCCESS;  
    return program;
}

cl_int
clEnqueueNDRangeKernel(
    cl_command_queue command_queue,
    cl_kernel        kernel,
    cl_uint          work_dim,
    const size_t *   global_work_offset,
    const size_t *   global_work_size,
    const size_t *   local_work_size,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) {

    /* TODO: find an implementation of the kernel that is 
       optimal for the given work group dimensions */
    /* TODO: fall back to an implementation with a single
       work item processed at a time */

    struct _OpenCLKernel* kernel_impl = 
        _find_opencl_kernel(kernel);

    if (kernel_impl == NULL) {
#ifdef DEBUG_OCL_HOST
        iprintf("clEnqueueNDRangeKernel: could not find the kernel in the registry.\n");
#endif
        return CL_INVALID_PROGRAM_EXECUTABLE;
    }

//     struct _thread_context *tc = 
//         (struct _thread_context *)malloc(sizeof(struct _thread_context));

    kernel_impl->call(
        kernel->args, work_dim, local_work_size, global_work_size);
    return CL_SUCCESS;
}

cl_int
clBuildProgram(
    cl_program           program,
    cl_uint              num_devices,
    const cl_device_id * device_list,
    const char *         options, 
    void (*pfn_notify)(cl_program /* program */, void * /* user_data */),
    void *                user_data) {
    return CL_SUCCESS;
}

cl_int
clSetKernelArg(
    cl_kernel    kernel,
    cl_uint      arg_index,
    size_t       arg_size,
    const void *  arg_value) {

    if (arg_value != NULL) {
        if (kernel->args[arg_index] == NULL)
            kernel->args[arg_index] = malloc(arg_size);
        
        if (kernel->args[arg_index] == NULL)
            return CL_INVALID_ARG_SIZE;

        memcpy(kernel->args[arg_index], arg_value, arg_size);   
    } else {
        if (kernel->args[arg_index] == NULL)
            kernel->args[arg_index] = malloc(sizeof(void *)); /* cl_mem? */

#if 0
        assert(kernel->args[arg_index] != NULL);
#endif
        *((void **) (kernel->args[arg_index])) = malloc(arg_size);
    }
        
    return CL_SUCCESS;
}

cl_int
clReleaseCommandQueue(cl_command_queue command_queue) {
    return CL_SUCCESS;
}

cl_int
clEnqueueReadBuffer(
    cl_command_queue    command_queue,
    cl_mem              buffer,
    cl_bool             blocking_read,
    size_t              offset,
    size_t              cb, 
    void *              ptr,
    cl_uint             num_events_in_wait_list,
    const cl_event *    event_wait_list,
    cl_event *          event) {
#ifdef DEBUG_OCL_HOST
    iprintf("clEnqueueReadBuffer: copying %#x bytes from %p to %p\n", cb, buffer, ptr);
#endif
    memcpy(ptr, buffer, cb);
    return CL_SUCCESS;
}

cl_int
clReleaseKernel(cl_kernel kernel) {
    return CL_SUCCESS;
}

cl_int
clReleaseProgram(cl_program program) {
    return CL_SUCCESS;
}

cl_int
clReleaseContext(cl_context context) {
    return CL_SUCCESS;    
}

/**
 * Returns the device info for the given device.
 *
 * @todo this should be made device-specific, some of the values should be
 * returned according to the device. For example, the memory sizes. Current
 * return values are known "OK" values.
 */
cl_int
clGetDeviceInfo(
    cl_device_id device,
    cl_device_info param_name,
    size_t param_value_size,
    void* param_value,
    size_t* param_value_size_ret) {
    const char* DEVICE_VENDOR = "TUT";
    const char* DEVICE_NAME = "TTA-TCE";

    switch (param_name) {
    default:
        return CL_INVALID_VALUE;
    case CL_DEVICE_TYPE:        
        if (param_value != NULL)
            *(cl_device_type*)param_value = 
                CL_DEVICE_TYPE_ACCELERATOR | CL_DEVICE_TYPE_GPU | 
                CL_DEVICE_TYPE_CPU;
        return CL_SUCCESS;
    case CL_DEVICE_GLOBAL_MEM_SIZE:
        if (param_value != NULL)
            *(cl_ulong*)param_value = 16*1024*1024;
        return CL_SUCCESS;
    case CL_DEVICE_ENDIAN_LITTLE:
        if (param_value != NULL)
            *(cl_bool*)param_value = CL_TRUE;
        return CL_SUCCESS;
    case CL_DEVICE_COMPILER_AVAILABLE:
        if (param_value != NULL)
            *(cl_bool*)param_value = CL_FALSE; /* embedded platform profile */
        return CL_SUCCESS;
    case CL_DEVICE_EXECUTION_CAPABILITIES:
        if (param_value != NULL)
            *(cl_device_exec_capabilities*)param_value = 
                CL_EXEC_KERNEL | CL_EXEC_NATIVE_KERNEL;
        return CL_SUCCESS;
    case CL_DEVICE_MAX_WORK_GROUP_SIZE:
        if (param_value != NULL)
            *(size_t*)param_value = SIZE_MAX;
        return CL_SUCCESS; 
    case CL_DEVICE_VENDOR:
        if (param_value != NULL) 
            strncpy((char*)param_value, DEVICE_VENDOR, param_value_size);
        if (param_value_size_ret != NULL)
            *param_value_size_ret = strlen(DEVICE_VENDOR);
        return CL_SUCCESS;
    case CL_DEVICE_NAME:
        if (param_value != NULL)
            strncpy((char*)param_value, DEVICE_NAME, param_value_size);
        if (param_value_size_ret != NULL)
            *param_value_size_ret = strlen(DEVICE_NAME);
        return CL_SUCCESS;
    }
    return CL_INVALID_VALUE;
}

cl_int 
clFinish(cl_command_queue /* command_queue */) {
    return CL_SUCCESS;
}

cl_int 
clGetDeviceIDs(cl_platform_id   /* platform */,
               cl_device_type   /* device_type */, 
               cl_uint         num_entries, 
               cl_device_id*   devices, 
               cl_uint*        num_devices) {

    if (num_entries == 0 && devices != NULL) {
        return CL_INVALID_VALUE;
    }

    if (devices == NULL && num_devices == NULL) {
        return CL_DEVICE_NOT_FOUND;
    }

    /* we can act as any device type */
    if (devices != NULL) {
        devices[0] = 0; 
    }

    if (num_devices != NULL) {
        *num_devices = 1;
    }
    return CL_SUCCESS;
}


cl_int
clGetPlatformIDs(cl_uint          /*num_entries*/,
                 cl_platform_id * platforms,
                 cl_uint *        num_platforms) {
    if (platforms == NULL && num_platforms == NULL) {
        return CL_INVALID_VALUE;
    }
    // just some dummy platform id here
    if (platforms != NULL)
        platforms[0] = (cl_platform_id) 1;
    if (num_platforms != NULL)
        *num_platforms = 1;
    
    return CL_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// NVIDIA OpenCL SDK APIs for easier out-of-the-box compilation
//////////////////////////////////////////////////////////////////////////////
char* 
oclLoadProgSource(
    const char* cFilename, const char* cPreamble, size_t* szFinalLength) {
    // need to return a freeable object just for out-of-the-box
    // compilation sake
    return (char*)malloc(1);
}

} // extern "C"
