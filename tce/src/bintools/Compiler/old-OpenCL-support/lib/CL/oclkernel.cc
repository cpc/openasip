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
 * @file oclkernel.cc
 *
 * Implementations for functions used from OpenCL C kernels.
 * Note: the function definitions contain the _thread_context
 * first parameter altough it's not present in the declarations
 * in oclkernel.hh. This is to avoid the need to add the
 * first argument to these functions with the LLVM pass every
 * time a kernel is compiled.
 *
 * @author 2009 Carlos Sanchéz de La Lama (csanzhez @ urjc.es)
 * @author 2009-10 Pekka Jääskeläinen (pekka jaaskelainen tut fi)
 */

#include "opencl_types.hh"

extern "C" {
    uint
    get_work_dim(unsigned x, unsigned y, unsigned z, _thread_context *tc)
    {
        return tc->work_dim;
    }

    size_t
    get_global_size(uint dimindx,
		    unsigned x, unsigned y, unsigned z, _thread_context *tc)
    {
//         if (dimindx >= tc->work_dim)
//             return 1;

        return tc->global_size[dimindx];
    }

    size_t
    get_global_id(uint dimindx,
		  unsigned x, unsigned y, unsigned z, _thread_context *tc)
    {
//         if (dimindx >= tc->work_dim)
//             return 0;
      
      unsigned local = 0;

      switch(dimindx) {
      case 0:
	local = x;
      case 1:
	local = y;
      case 2:
	local = z;
      }
    
      return tc->global_id[dimindx] + local;
    }

    size_t
    get_local_size(uint dimindx,
		   unsigned x, unsigned y, unsigned z, _thread_context *tc)
    {
//         if (dimindx >= tc->work_dim)
//             return 1;
    
        return tc->local_size[dimindx];
    }

    size_t
    get_local_id(uint dimindx,
		 unsigned x, unsigned y, unsigned z, _thread_context *tc)
    {
//         if (dimindx >= tc->work_dim)
//             return 0;

      switch(dimindx) {
      case 0:
	return x;
      case 1:
	return y;
      case 2:
	return z;
      }

      return 0;
    
    }

    size_t
    get_num_groups(uint dimindx,
		   unsigned x, unsigned y, unsigned z, _thread_context *tc)
    {
//         if (dimindx >= tc->work_dim)
//             return 1;
    
        return tc->num_groups[dimindx];
    }

    size_t
    get_group_id(uint dimindx,
		 unsigned x, unsigned y, unsigned z, _thread_context *tc)
    {
//         if (dimindx >= tc->work_dim)
//             return 0;
    
        return tc->group_id[dimindx];
    }

    __attribute__ ((noinline)) void
    barrier(int,
	    unsigned x, unsigned y, unsigned z, _thread_context *tc)
    {
        asm volatile ("");
    }
}
