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
 * @file CustomCommand.hh
 *
 * The declaration of CustomCommand class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 27 May 2004 by pj, jn, vpj, ll
 * @note rating: green
 */

#ifndef TTA_CUSTOM_COMMAND_HH 
#define TTA_CUSTOM_COMMAND_HH

#include <string>
#include <vector>

#include "LineReader.hh"
#include "InterpreterContext.hh"
#include "ScriptInterpreter.hh"
#include "DataObject.hh"
#include "Exception.hh"

class ScriptInterpreter;

/**
 * Abstract base class for all CustomCommands for Interpreter.
 *
 * CustomCommand is a user defined command designed for a particular task.
 */
class CustomCommand {

public:
    explicit CustomCommand(std::string name);
    CustomCommand(const CustomCommand& cmd);
    virtual ~CustomCommand();

    std::string name() const;

    void setContext(InterpreterContext* context);
    InterpreterContext* context() const;

    void setInterpreter(ScriptInterpreter* si);
    ScriptInterpreter* interpreter() const;

    virtual bool execute(const std::vector<DataObject>& arguments) = 0;
    virtual std::string helpText() const = 0;

    bool checkArgumentCount(int argumentCount, int minimum, int maximum);
    bool checkIntegerArgument(const DataObject& argument);    
    bool checkPositiveIntegerArgument(const DataObject& argument);
    bool checkUnsignedIntegerArgument(const DataObject& argument);
    bool checkDoubleArgument(const DataObject& argument);

private:
    /// Assignment not allowed.
    CustomCommand& operator=(const CustomCommand&);
    
    /// The name of the command.
    std::string name_;
    /// Context of the command.
    InterpreterContext* context_;
    /// Interpreter for the command.
    ScriptInterpreter* interpreter_;
};

#include "CustomCommand.icc"

#endif
