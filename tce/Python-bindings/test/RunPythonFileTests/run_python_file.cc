/*
    Copyright (c) 2002-2009 Tampere University.

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
/*
 * @file run_python_file.cc
 * 
 * Run the Python file given as argument.
 *
 */


#include "Python.h"

#include <boost/python.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <stdexcept>
#include <cassert>

using namespace boost::python;

int main(int argc, char *argv[]) {

    /* Initialize the Python interpreter. */
    Py_Initialize();
    
    /* Open the Python file. */
    FILE *fp = fopen(argv[1], "r");

    /* Execute a Python file (in module __main__) */
    PyRun_SimpleFile(fp, argv[1]);

    Py_Finalize();
}
