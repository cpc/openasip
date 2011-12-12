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
 * @file oclc_initialized_object.cc
 *
 * A template C++ file for generating a C++ object that automatically
 * registers the OpenCL C kernel launcher functions to the host API's
 * kernel registry. Used for standalone mode to enable separate
 * linking of multiple kernel files and host code without an adhoc
 * data file for collecting the kernel information.
 *
 * @author Pekka Jääskeläinen 2010-2011
 * @note rating: red
 */
class %(kernel_name)s_kernel {
public:    
    %(kernel_name)s_kernel() {
        kernel_obj.name = "%(kernel_name)s";
        kernel_obj.call = __opencl_trampoline_mt_%(kernel_name)s;
        _register_opencl_kernel(&kernel_obj);
    }
private:
    _OpenCLKernel kernel_obj;
} _kernel_initializer_obj_%(kernel_name)s;
