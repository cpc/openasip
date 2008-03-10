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
