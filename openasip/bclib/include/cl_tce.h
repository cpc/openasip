/*
    Copyright (c) 2002-2010 Tampere University.

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
 * Declarations used by the OpenCL implementation of TCE.
 *
 * @author Pekka J‰‰skel‰inen 2009-2010
 */
#ifndef CL_TCE_H_
#define CL_TCE_H_

#include "opencl_types.hh"

#ifdef __cplusplus
extern "C" {
#endif

// the maximum number of kernels per standalone program
#define _TCE_CL_MAX_KERNELS 32
#define _TCE_CL_DEVICE_MAX_PARAMETERS 16

/* device info, these will be partially generated according to the
   TTAGPU device in the future */
#define _TCE_CL_DEVICE_MAX_PARAMETER_SIZE 256

/**
 * Represents information about a callable OpenCL kernel.
 *
 */ 
struct _OpenCLKernel {
    /* the name of the kernel */
    const char* name;

    /* kernel "trampoline" function that converts the raw argument data to
       the kernel's function arguments and calls the kernel */
    void (*call) (void** /*arguments*/, size_t*, int, const size_t*, const size_t*);
};

void _register_opencl_kernel(struct _OpenCLKernel* kernel);

#ifdef __cplusplus
}
#endif

#endif

