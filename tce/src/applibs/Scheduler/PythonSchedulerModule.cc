/**
 * @file PythonSchedulerModule.cc
 *
 * Implementation of PythonSchedulerModule class.
 *
 * @author Pertti Kellomäki 2007 (pertti.kellomaki@tut.fi)
 * @note rating: red
 */

#ifdef PYTHON_ENABLED
#include "Python.h"
#endif

#include "PythonSchedulerModule.hh"
#include "Program.hh"
#include "Machine.hh"

#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <stdexcept>
#include <cassert>

#ifdef PYTHON_ENABLED
using namespace boost::python;
#endif

/**
 * Constructor.
 */
PythonSchedulerModule::PythonSchedulerModule(string pythonFile):
    pythonFile_(pythonFile) {
#ifdef PYTHON_ENABLED

    // Initialize the Python interpreter exactly once
    if (!isPythonInitialized) {
	Py_Initialize();
	isPythonInitialized = true;
    }
    // TODO: error handling
    // Open the Python file.
    FILE *fp = fopen(pythonFile_.c_str(), "r");

    // Execute a Python file (in module __main__) and close the file after done.
    PyRun_SimpleFileEx(fp, pythonFile_.c_str(), 1);

    // Retrieve the main module
    object main_module((handle<>(boost::python::borrowed(PyImport_AddModule("__main__")))));

    // Get the class whose name is the same as the Python file, sans the ".py"
    std::string passClassName(pythonFile_, 0, pythonFile_.length() - 3);
    object passClass(main_module.attr(passClassName.c_str()));
    
    // Create an instance and store it in a member variable
    passInstance = passClass();

    // Find out what the pass needs
    needsTarget_ = call_method<bool>(passInstance.ptr(), "needsTarget");
    needsProgram_ = call_method<bool>(passInstance.ptr(), "needsProgram");
    needsProgramRepresentation_ = call_method<bool>(passInstance.ptr(), "needsProgramRepresentation");
#else
    std::cerr << "Unable to run " << pythonFile_ << std::endl;
    std::cerr << "You need to enable Python when you configure TCE ";
    std::cerr << "in order to use passes written in Python." << std::endl;
    exit(1);
#endif
}

/**
 * Destructor.
 */
PythonSchedulerModule::~PythonSchedulerModule() {
}

void
PythonSchedulerModule::start()
    throw (ObjectNotInitialized, WrongSubclass, ModuleRunTimeError) {
#ifdef PYTHON_ENABLED
    if (needsProgram_ && program_ == NULL || 
	needsTarget_ && target_ == NULL) {
        string method = "PythonSchedulerModule::start()";
        string msg =
            "Source program and/or target architecture not defined!";
        throw ObjectNotInitialized(__FILE__, __LINE__, method, msg);
    }

    // Initialize the target and program if needed
    if (needsProgram_) {
	call_method<void>(passInstance.ptr(), "setProgram", ptr(program_));
    }
    if (needsTarget_) {
	call_method<void>(passInstance.ptr(), "setTarget", ptr(target_));
    }

    // Start the pass
    call_method<bool>(passInstance.ptr(), "start");
#endif
}

bool
PythonSchedulerModule::needsTarget() const {
    return needsTarget_;
}

bool
PythonSchedulerModule::needsProgram() const {
    return needsProgram_;
}

bool
PythonSchedulerModule::needsProgramRepresentation() const {
    return needsProgramRepresentation_;
}

void
PythonSchedulerModule::setTarget(const TTAMachine::Machine& target) {
    target_ = &target;
}

void
PythonSchedulerModule::setProgram(TTAProgram::Program& program) {
    program_ = &program;
}

bool PythonSchedulerModule::isPythonInitialized = false;
