/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file oclc_launcher_header.c
 *
 * A template C file for generating the common code for the launcher
 * file of OpenCL C kernel work group functions. This code forms the
 * beginning of the launcher code file. After this the actual kernel
 * launcher functions are generated for all the kernels in the compiled
 * OpenCL C file.
 *
 * @author Pekka Jääskeläinen 2010-2011
 * @note rating: red
 */
#define min(a,b) (((a) < (b)) ? (a) : (b))

#include <stdlib.h>
#include <stdio.h>
#include "cl_tce.h"
#include <pocl.h>

//#define DEBUG_MT
//#define DEBUG_OCL

/* The arguments and context data passed to a work group function. */
struct _ocl_args {
    void** args;
    int work_dim;
    const size_t* local_work_size;
    const size_t* global_work_size;
    int first_gid_x;
    int last_gid_x;
};

/* this function should be called to register the kernel to the Host API's kernel
   registry */
extern "C" void _register_opencl_kernel(struct _OpenCLKernel* kernel);
