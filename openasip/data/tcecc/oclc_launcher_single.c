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
 * @file oclc_launcher_single.c
 *
 * A template C file for generating the launcher code for OpenCL C
 * kernel work group functions. This version does not spawn any threads,
 * thus is the most efficient version for single core execution.
 *
 * @author Pekka Jääskeläinen 2010-2011
 * @note rating: red
 */
extern "C" void %(converted_name)s(void** args, pocl_context*);

static void __opencl_launch_wg_%(kernel_name)s(void **args, pocl_context* context) {
#ifdef DEBUG_OCL
                iprintf("### launching a WG %%d-%%d-%%d of %%d-%%d-%%d\\n", 
                        gid_x, gid_y, gid_z, num_groups_x, num_groups_y, num_groups_z);
#endif
                %(converted_name)s(args, context);
}

static void __opencl_trampoline_%(kernel_name)s(
    void** args, 
    int work_dim, 
    const size_t* local_work_size, 
    const size_t* global_work_size,
    int first_gidx, int last_gidx) {

    int num_groups_y, num_groups_z;

    const int local_size_x = local_work_size[0];
    const int global_size_x = global_work_size[0];
    const int num_groups_x = global_size_x / local_size_x;

    // We need only one context for the single core version
    pocl_context context;
    context.work_dim = work_dim;
    context.global_offset[0] = 0;
    context.global_offset[1] = 0;
    context.global_offset[2] = 0;

    // Use only the number of iterators needed
    switch (work_dim) {
    case 1: 
        context.num_groups[1] = 1; 
        context.num_groups[2] = 1;
    	context.group_id[1] = 0;
   	context.group_id[2] = 0;
        for (unsigned gid_x = first_gidx; gid_x <= last_gidx; gid_x++) { 
    	    context.group_id[0] = gid_x;
	    __opencl_launch_wg_%(kernel_name)s(args, &context);
        }
        break; 
    case 2:
    	num_groups_y = global_work_size[1] / local_work_size[1];
	context.num_groups[1] = num_groups_y;
	context.num_groups[2] = 1;
    	context.group_id[2] = 0;
        for (unsigned gid_x = first_gidx; gid_x <= last_gidx; gid_x++) { 
            for (unsigned gid_y = 0; gid_y < num_groups_y; gid_y++) { 
    		context.group_id[0] = gid_x;
    		context.group_id[1] = gid_y;
	        __opencl_launch_wg_%(kernel_name)s(args, &context);
	    }
        }
        break;
    case 3:
    	num_groups_y = global_work_size[1] / local_work_size[1];
    	num_groups_z = global_work_size[2] / local_work_size[2];
	context.group_id[1] = num_groups_y;
	context.group_id[2] = num_groups_z;
        for (unsigned gid_x = first_gidx; gid_x <= last_gidx; gid_x++) { 
            for (unsigned gid_y = 0; gid_y < num_groups_y; gid_y++) { 
                for (unsigned gid_z = 0; gid_z < num_groups_z; gid_z++) {
    		    context.group_id[0] = gid_x;
    		    context.group_id[1] = gid_y;
    		    context.group_id[2] = gid_z;
	            __opencl_launch_wg_%(kernel_name)s(args, &context);
		}
	    }
        }
	break;
    }
}

static void* __opencl_wg_thread_%(kernel_name)s(void* arg_struct) {
    _ocl_args* args = (_ocl_args*)arg_struct;
    __opencl_trampoline_%(kernel_name)s(
        args->args, args->work_dim, 
        args->local_work_size, args->global_work_size, 
        args->first_gid_x, args->last_gid_x);
    return 0;
}

void __opencl_trampoline_mt_%(kernel_name)s(
    void** args, 
    size_t* sizes,
    int work_dim, 
    const size_t* local_work_size, 
    const size_t* global_work_size) {
    int num_groups_x = global_work_size[0] / local_work_size[0];
    int i, first_gid_x, last_gid_x;

    /* single thread version: execute all work groups in
       a single trampoline call as fast as possible. 

       Do not create any threads. */
    /* allocate data for the local buffers. TODO: this is not
       thread safe due to the shared 'args' struct. In MT we need
       to copy the local pointers for each invocation. */
    int arg_is_local[] = _%(kernel_name)s_ARG_IS_LOCAL;
    for (int i = 0; i < _%(kernel_name)s_NUM_ARGS; ++i) {
        if (!arg_is_local[i]) continue;
        args[i] = realloc(args[i], sizeof(void*));             
        *((void**)args[i]) = realloc(*((void**)args[i]), sizes[i]);
        if (*((void**)args[i]) == NULL) {
#ifdef DEBUG_OCL
            puts("### out of memory while allocating the local buffers\\n");
#endif
            exit(1);
        }
    }
#if _%(kernel_name)s_NUM_LOCALS != 0
    /* Allocate data for the automatic local buffers which have
       been converted to pointer arguments in the kernel launcher
       function. They are the last arguments always. */
        int alocal_sizes[] = _%(kernel_name)s_LOCAL_SIZE;
        for (int i = _%(kernel_name)s_NUM_ARGS; 
             i < _%(kernel_name)s_NUM_ARGS + _%(kernel_name)s_NUM_LOCALS; 
             ++i) {
            args[i] = realloc(args[i], sizeof(void*));
            *((void**)args[i]) = realloc(*((void**)args[i]), alocal_sizes[i - _%(kernel_name)s_NUM_ARGS]);
#ifdef DEBUG_OCL
            iprintf("### allocated %%d bytes for the automatic local arg %%d at %%x\\n", 
                    alocal_sizes[i - _%(kernel_name)s_NUM_ARGS], i, *(unsigned int*)args[i]);
#endif
            if (*((void**)args[i]) == NULL) {
#ifdef DEBUG_OCL
                puts("### out of memory while allocating the local buffers\\n");
#endif
                exit(1);
            }          
        }
#endif
        __opencl_trampoline_%(kernel_name)s(
            args, work_dim, local_work_size, global_work_size,
            0, num_groups_x - 1);
    /* TODO: Pass back local buffers in some struct, such as cl_kernel, so
	     that they can be freed with clReleaseKernel(). It is too expensive
	     to free them now. For now, they are leaked. */
}
