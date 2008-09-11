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
