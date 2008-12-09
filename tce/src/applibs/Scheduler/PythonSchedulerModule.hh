/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file PythonSchedulerModule.hh
 *
 * Declaration of PythonSchedulerModule class.
 *
 * The class stores the name of the Python file containing the actual
 * scheduler pass. The class implementing the pass should be named
 * PythonSchedulerPass in Python.
 * 
 * When a PythonSchedulerModule object is created, it initializes the
 * Python interpreter and loads the Python file. It then creates an
 * instance of the Python class PythonSchedulerPass and calls its
 * methods needsTarget(), needsProgram(), and
 * needsProgramRepresentation(). The values returned by these
 * invocations are stored in needsTarget_, needsProgram_, and
 * needsProgramRepresentation_, respectively. 
 *
 * When the pass is started by the scheduler, the Python file is again
 * loaded to the interpreter, an instance of PythonSchedulerPass is
 * created, the appropriate set*() methods are called, and the pass is
 * started by calling start().
 *
 * @author Pertti Kellomäki 2007 (pertti.kellomaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PYTHON_SCHEDULER_MODULE_HH
#define TTA_PYTHON_SCHEDULER_MODULE_HH

#include "StartableSchedulerModule.hh"
#include "HelperSchedulerModule.hh"

#include "tce_config.h"

#include <boost/python.hpp>

#include <string>

using namespace std;

/**
 * Scheduler module that runs a Python script.
 */
class PythonSchedulerModule : public StartableSchedulerModule {
public:
    PythonSchedulerModule(string pythonFile);
    virtual ~PythonSchedulerModule();

    virtual void start()
        throw (ObjectNotInitialized, WrongSubclass, ModuleRunTimeError);
    virtual bool needsTarget() const;
    virtual bool needsProgram() const;
    virtual bool needsProgramRepresentation() const;

    // TODO: these should be inherited from BaseSchedulerModule
    void setTarget(const TTAMachine::Machine& target);
    void setProgram(TTAProgram::Program& program);

    virtual void registerHelperModule(HelperSchedulerModule& module)
        throw (IllegalRegistration);

private:
    string pythonFile_;
    bool needsTarget_;
    bool needsProgram_;
    bool needsProgramRepresentation_;

    // An instance of the Python class which implements the pass.
#ifdef PYTHON_ENABLED
    boost::python::object passInstance;
#endif

    static bool isPythonInitialized;
};

#endif
