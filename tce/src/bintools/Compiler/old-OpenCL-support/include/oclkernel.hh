/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file oclkernel.hh
 *
 * Declarations of functions and definitions visible to OpenCL kernels.
 * The idea is like with GLSL compilation to convert as much as possible
 * of the kernel by inclusion of this header to C++ code, thus require
 * minimal "preprocessing" of the code. 
 *
 * @author 2009-2010 Pekka J‰‰skel‰inen
 *
 */
#ifndef OCL_KERNEL_FUNCS
#define OCL_KERNEL_FUNCS

#include "opencl_types.hh"

#define __global
#define __constant const 

#define cl_mem_fence_flags int

#define CLK_LOCAL_MEM_FENCE 1

extern "C" {
  uint
  get_work_dim();

  size_t
  get_global_size(uint dimindx);

  size_t
  get_global_id(uint dimindx);

  size_t
  get_local_size(uint dimindx);

  size_t
  get_local_id(uint dimindx);

  size_t
  get_num_groups(uint dimindx);

  size_t
  get_group_id(uint dimindx);

  void
  barrier(cl_mem_fence_flags flags);
}

#endif
