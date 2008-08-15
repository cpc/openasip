/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
