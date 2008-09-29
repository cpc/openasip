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

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException) = 0;
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
