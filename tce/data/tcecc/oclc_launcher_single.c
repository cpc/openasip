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
 * @file oclc_simple_launcher.c
 *
 * A template C file for generating the launcher code for OpenCL C
 * kernel work group functions. This version does not spawn any threads,
 * thus is the most efficient version for single core execution.
 *
 * @author Pekka Jääskeläinen 2010-2011
 * @note rating: red
 */
extern "C" void %(converted_name)s(void** args, pocl_context*);

static void __opencl_trampoline_%(kernel_name)s(
    void** args, 
    int work_dim, 
    const size_t* local_work_size, 
    const size_t* global_work_size,
    int first_gidx, int last_gidx) {

    const int local_size_x = local_work_size[0];
    const int local_size_y = (work_dim >= 2) ? local_work_size[1] : 1;
    const int local_size_z = (work_dim == 3) ? local_work_size[2] : 1;

    const int global_size_x = global_work_size[0];
    const int global_size_y = (work_dim >= 2) ? global_work_size[1] : 1;
    const int global_size_z = (work_dim == 3) ? global_work_size[2] : 1;

    const int num_groups_x = global_size_x / local_size_x;
    const int num_groups_y = (work_dim >= 2) ? (global_size_y / local_size_y) : 1;
    const int num_groups_z = (work_dim == 3) ? (global_size_z / local_size_z) : 1;

    for (unsigned gid_x = first_gidx; gid_x <= last_gidx; gid_x++) { 
        for (unsigned gid_y = 0; gid_y < num_groups_y; gid_y++) { 
            for (unsigned gid_z = 0; gid_z < num_groups_z; gid_z++) {
                pocl_context context;
                context.work_dim = work_dim;
                context.num_groups[0] = num_groups_x;
                context.num_groups[1] = num_groups_y;
                context.num_groups[2] = num_groups_z;
                context.group_id[0] = gid_x;
                context.group_id[1] = gid_y;
                context.group_id[2] = gid_z;
                context.global_offset[0] = 0;
                context.global_offset[1] = 0;
                context.global_offset[2] = 0;
#ifdef DEBUG_OCL
                iprintf("### launching a WG %%d-%%d-%%d of %%d-%%d-%%d\\n", 
                        gid_x, gid_y, gid_z, num_groups_x, num_groups_y, num_groups_z);
#endif
                %(converted_name)s(args, &context);
            } 
        }
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

    if (USE_MT && _MAX_WG_THREADS > 1) {
#if USE_MT == 1
        int num_threads = min(_MAX_WG_THREADS, num_groups_x);
        int wgs_per_thread = num_groups_x / num_threads;
        /* In case the work group count is not divisible by the
           number of threads, we have to execute some additional
           workgroups in the last thread. */
        int leftover_wgs = num_groups_x - (num_threads*wgs_per_thread);
        pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t)*num_threads);
        _ocl_args* arguments = (_ocl_args*)malloc(sizeof(_ocl_args)*num_threads);

#ifdef DEBUG_MT    
    iprintf("### global_work_size[0]==%%d local_work_size[0]==%%d\\n", 
             global_work_size[0], local_work_size[0]);
    iprintf("### creating %%d work group threads\\n", num_threads);
    iprintf("### wgs per thread==%%d leftover wgs==%%d\\n", wgs_per_thread, leftover_wgs);
#endif
    
        for (i = 0, first_gid_x = 0, last_gid_x = wgs_per_thread - 1; 
             i < num_threads; 
             ++i, first_gid_x += wgs_per_thread, last_gid_x += wgs_per_thread) {
           arguments[i].args = args;
           arguments[i].work_dim = work_dim;
           arguments[i].local_work_size = local_work_size;
           arguments[i].global_work_size = global_work_size;
           arguments[i].first_gid_x = first_gid_x;
           if (i + 1 == num_threads) last_gid_x += leftover_wgs;
           arguments[i].last_gid_x = last_gid_x;


#ifdef DEBUG_MT       
       iprintf("### creating wg thread: first_gid_x==%%d, last_gid_x==%%d\\n",
                first_gid_x, last_gid_x);
#endif
       
           pthread_attr_t attr;
           pthread_attr_init(&attr);
          /* TODO: set stack size according to WG size so we don't waste space nor
             allocate too small stack  */
           pthread_attr_setstacksize(&attr, 32*1024); 
           pthread_create(&threads[i], &attr, &__opencl_wg_thread_%(kernel_name)s, &arguments[i]);
        }

        for (i = 0; i < num_threads; ++i) {
            pthread_join(threads[i], NULL);
        /*
        iprintf("### thread %%x finished\\n", (unsigned)threads[i]);
        */
        }
        free(arguments);
        free(threads);
#endif
   } else {
        /* single thread version: execute all work groups in
           a single trampoline call as fast as possible. 

           Do not create any threads. */
        /* allocate data for the local buffers. TODO: this is not
           thread safe due to the shared 'args' struct. In MT we need
           to copy the local pointers for each invocation. */
        int arg_is_local[] = _%(kernel_name)s_ARG_IS_LOCAL;
        for (int i = 0; i < _%(kernel_name)s_NUM_ARGS; ++i) {
            if (!arg_is_local[i]) continue;
            args[i] = malloc(sizeof(void*));             
            *((void**)args[i]) = malloc(sizes[i]);
            if (*((void**)args[i]) == NULL) {
#ifdef DEBUG_OCL
               puts("### out of memory while allocating the local buffers\\n");
#endif
               exit(1);
            }
        }
        /* Allocate data for the automatic local buffers which have
           been converted to pointer arguments in the kernel launcher
           function. They are the last arguments always. */
        int alocal_sizes[] = _%(kernel_name)s_LOCAL_SIZE;
        for (int i = _%(kernel_name)s_NUM_ARGS; 
             i < _%(kernel_name)s_NUM_ARGS + _%(kernel_name)s_NUM_LOCALS; 
             ++i) {
           args[i] = malloc(sizeof(void*));
            *((void**)args[i]) = malloc(alocal_sizes[i - _%(kernel_name)s_NUM_ARGS]);
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
        __opencl_trampoline_%(kernel_name)s(
           args, work_dim, local_work_size, global_work_size,
           0, num_groups_x - 1);

        /* free the local buffers */
        for (int i = 0; i < _%(kernel_name)s_NUM_ARGS; ++i) {
            if (!arg_is_local[i]) continue;
            free(*((void**)args[i]));
            free(args[1]);
        }
        /* free the automatic local buffers */
        for (int i = _%(kernel_name)s_NUM_ARGS; 
             i < _%(kernel_name)s_NUM_ARGS + _%(kernel_name)s_NUM_LOCALS; 
             ++i) {
#ifdef DEBUG_OCL
            iprintf("### freed automatic local arg %%d\\n", i);
#endif
            free(*((void**)args[i]));
            free(args[1]);
        }
   }
}

