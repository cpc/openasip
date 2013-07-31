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
 * Implementation of the host-side library of the TCE OpenCL support for
 * the "standalone" static linkage mode.
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

#define DUMMY_PLATFORM_ID 42
#define DUMMY_CONTEXT 32000

extern "C" {

//////////////////////////////////////////////////////////////////////////////
// Code for implementing the kernel registry
//////////////////////////////////////////////////////////////////////////////

// a single kernel call instance
struct _cl_kernel {
    /* General information */
    char* name;

    /* Argument data */
    void* args[_TCE_CL_DEVICE_MAX_PARAMETERS];
    size_t sizes[_TCE_CL_DEVICE_MAX_PARAMETERS];

   /* Default implementation */
   struct _OpenCLKernel* impl;
};

/* An array of callable OpenCL kernel functions. */
struct _OpenCLKernel* _opencl_kernel_registry[_TCE_CL_MAX_KERNELS];

/* Must be volatile otherwise LLVM removes the global constructor. */
volatile int _opencl_kernel_count = 0;

/* Internal counter to ensure unique kernel IDs */
volatile int current_id = 0;

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
    int i;

    const char *name = kernel->name;
    /* todo: the work group dimensions */
    for (i = 0; i < _opencl_kernel_count; i++) {
	struct _OpenCLKernel* k = _opencl_kernel_registry[i];
	const char *kname = k->name;

	if (!strcmp(name, kname)) {
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
    
    if (context != (cl_context) DUMMY_CONTEXT)
	return CL_INVALID_CONTEXT;

    switch (param_name) {
    default:
	return CL_INVALID_VALUE;
    case CL_CONTEXT_REFERENCE_COUNT:
	/* TODO: Implement reference counts. For now, always returns 1. */
	if (param_value_size_ret != NULL)
	    *param_value_size_ret = sizeof(cl_uint);
	if (param_value != NULL) {
	    if (param_value_size < sizeof(cl_uint))
	    	return CL_INVALID_VALUE;
	    *((cl_uint *) param_value) = 1;
	}	
	break;
    case CL_CONTEXT_DEVICES:
	/* For now, the only device is 0. */
	if (param_value_size_ret != NULL)
	    *param_value_size_ret = sizeof(cl_device_id[1]);
	if (param_value != NULL) {
	    if (param_value_size < sizeof(cl_device_id[1]))
	    	return CL_INVALID_VALUE;
	    *((cl_device_id *) param_value) = (cl_device_id) 0;	
	}
	break;
    case CL_CONTEXT_PROPERTIES:
	/* Return the dummy platform. */
	if (param_value_size_ret != NULL)
	    *param_value_size_ret = sizeof(cl_context_properties[1]);
	if (param_value != NULL) {
	    if (param_value_size < sizeof(cl_context_properties[1]))
		return CL_INVALID_VALUE; 
	    *((cl_context_properties *) param_value) =
		(cl_platform_id) DUMMY_PLATFORM_ID;
	}	
    }
 
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
    return (cl_context) DUMMY_CONTEXT;
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
    return (cl_context) DUMMY_CONTEXT;
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
    
    if (memobj == NULL)
	return CL_INVALID_MEM_OBJECT;
    /* TODO: Reference counters, free */
    return CL_SUCCESS;
}

cl_mem
clCreateBuffer(
    cl_context   context,
    cl_mem_flags flags,
    size_t       size,
    void *       host_ptr,
    cl_int *     errcode_ret) {

    cl_mem mem;

    if (flags & CL_MEM_USE_HOST_PTR) {
	
	if (host_ptr == NULL) {
		if (errcode_ret != NULL)
			*errcode_ret = CL_INVALID_VALUE;
		return NULL;
	}
#ifdef DEBUG_OCL_HOST
	iprintf("clCreateBuffer: using the host pointer "
		"%p for the buffer\n", host_ptr);
#endif

	mem = (cl_mem)host_ptr;
    } else 
    	mem  = (cl_mem)malloc(size);

    if (mem == NULL) {
        if (errcode_ret != NULL)
            *errcode_ret = CL_OUT_OF_HOST_MEMORY;
        return NULL;
    } else {
        if (errcode_ret != NULL)
            *errcode_ret = CL_SUCCESS;
    }
    
    if (flags & CL_MEM_COPY_HOST_PTR && !(flags & CL_MEM_USE_HOST_PTR) 
	&& host_ptr != NULL) {
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
#ifdef DEBUG_OCL_HOST
        iprintf("Couldn't malloc %u bytes of memory for the cl_kernel.\n",
                sizeof(struct _cl_kernel));
#endif
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

    /* Select the default implementation */
    kernel->impl = _find_opencl_kernel(kernel);
    
    if (kernel->impl == NULL) {
#ifdef DEBUG_OCL_HOST
        iprintf("clCreateKernel: could not find the kernel %s in the registry"
		" of %d kernels.\n", kernel->name, _opencl_kernel_count);
#endif
	if (errcode_ret != NULL)
	    *errcode_ret = CL_INVALID_PROGRAM_EXECUTABLE;
	return NULL;
    }

    if (errcode_ret != NULL)
        *errcode_ret = CL_SUCCESS;
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

    /* For now, use the default implementation */
    struct _OpenCLKernel *kernel_impl = kernel->impl;

    if (kernel_impl == NULL) {
#ifdef DEBUG_OCL_HOST
        iprintf("clEnqueueNDRangeKernel: could not find a valid kernel" 
		" implementation.\n");
#endif
        return CL_INVALID_PROGRAM_EXECUTABLE;
    }

//     struct _thread_context *tc = 
//         (struct _thread_context *)malloc(sizeof(struct _thread_context));

    kernel_impl->call(
        kernel->args, kernel->sizes, work_dim, local_work_size, global_work_size);
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

    kernel->sizes[arg_index] = arg_size;
    if (arg_value != NULL) {
        if (kernel->args[arg_index] == NULL)
            kernel->args[arg_index] = malloc(arg_size);
        
        if (kernel->args[arg_index] == NULL)
            return CL_OUT_OF_RESOURCES;

        memcpy(kernel->args[arg_index], arg_value, arg_size);   
    } else {
        /* A __local argument. Just store the size, the launcher
           will allocate the local buffer. Assume there's enough
           local memory for now. */
        kernel->args[arg_index] = NULL;
    }
    return CL_SUCCESS;
}

cl_int
clReleaseCommandQueue(cl_command_queue command_queue) {
    return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReadBuffer(
    cl_command_queue    command_queue,
    cl_mem              buffer,
    cl_bool             blocking_read,
    size_t              offset,
    size_t              cb, 
    void *              ptr,
    cl_uint             num_events_in_wait_list,
    const cl_event *    event_wait_list,
    cl_event *          event) CL_API_SUFFIX__VERSION_1_0 {

    cl_uchar *offset_buffer;

    offset_buffer = (cl_uchar *)buffer + offset; 

    if (ptr == (void *) offset_buffer) {
#ifdef DEBUG_OCL_HOST
    	iprintf("clEnqueueReadBuffer: no-op because ptr and buffer are the same\n");
#endif
	return CL_SUCCESS;
    }

#ifdef DEBUG_OCL_HOST
    iprintf("clEnqueueReadBuffer: copying %#x bytes from %p to %p\n", cb, offset_buffer,
	    ptr);
#endif

    memmove(ptr, offset_buffer, cb);
    return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueWriteBuffer(cl_command_queue command_queue,
                     cl_mem buffer,
                     cl_bool blocking_write,
                     size_t offset,
                     size_t cb, 
                     const void *ptr,
                     cl_uint num_events_in_wait_list,
                     const cl_event *event_wait_list,
                     cl_event *event) CL_API_SUFFIX__VERSION_1_0 {
    
    cl_uchar *offset_buffer;

    offset_buffer = (cl_uchar *)buffer + offset; 
    
    if (ptr == (void *) offset_buffer) {
#ifdef DEBUG_OCL_HOST
    	iprintf("clEnqueueWriteBuffer: no-op because ptr and buffer are the same\n");
#endif
	return CL_SUCCESS;
    }

#ifdef DEBUG_OCL_HOST
    iprintf("clEnqueueReadBuffer: copying %#x bytes from %p to %p\n", cb, offset_buffer,
	    ptr);
#endif
    memmove(offset_buffer, ptr, cb);
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
        platforms[0] = (cl_platform_id) DUMMY_PLATFORM_ID;
    if (num_platforms != NULL)
        *num_platforms = 1;
    
    return CL_SUCCESS;
}

/* COPIED DIRECTLY FROM POCL START (todo: reuse directly) */
CL_API_ENTRY cl_int CL_API_CALL 
clGetPlatformInfo(cl_platform_id   platform,
                  cl_platform_info param_name,
                  size_t           param_value_size, 
                  void *           param_value,
                  size_t *         param_value_size_ret) CL_API_SUFFIX__VERSION_1_0
{
  const char *ret = 0;
  int retlen;	

  if (platform != DUMMY_PLATFORM_ID)
      return CL_INVALID_PLATFORM;
	
  switch (param_name)
	{
    case CL_PLATFORM_PROFILE:
      // TODO: figure this out depending on the native execution host.
      // assume FULL_PROFILE for now.
      ret = "FULL_PROFILE";
      break;
    case CL_PLATFORM_VERSION:
      ret = "OpenCL 1.2";
      break;
    case CL_PLATFORM_NAME:
      ret = "Portable OpenCL";
      break;
    case CL_PLATFORM_VENDOR:
      ret = "The POCL project";
      break;
    case CL_PLATFORM_EXTENSIONS:
      // TODO: list all suppoted extensions here.
      ret = "";
      break;
    default: 
      return CL_INVALID_VALUE;
	}

  // Specs say (section 4.1) to "ignore param_value" should it be NULL
  if (param_value == NULL)
    return CL_SUCCESS;	
	
  // the OpenCL API docs *seem* to count the trailing NULL
  retlen = strlen(ret) + 1;
	
  if (param_value_size < retlen)
    return CL_INVALID_VALUE;

  strncpy((char*)param_value, ret, retlen); 
	
  if (param_value_size_ret != NULL)
    *param_value_size_ret=retlen;
	
  return CL_SUCCESS;

}

/* COPIED DIRECTLY FROM POCL END (todo: reuse via a library) */

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
