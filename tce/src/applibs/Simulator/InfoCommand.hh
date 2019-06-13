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
 * @file InfoCommand.hh
 *
 * Declaration of InfoCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INFO_COMMAND
#define TTA_INFO_COMMAND

#include <string>
#include <vector>
#include <map>

#include "DataObject.hh"
#include "Exception.hh"
#include "SimControlLanguageCommand.hh"


/**
 * Implementation of the "info" command of the Simulator Control Language.
 */
class InfoCommand : public SimControlLanguageCommand {
public:
    InfoCommand(bool isCompiledSimulation = false);
    virtual ~InfoCommand();

    virtual bool execute(const std::vector<DataObject>& arguments);
    virtual std::string helpText() const;
protected:
    /// container type for subcommands indexed by name
    typedef std::map<std::string, SimControlLanguageSubCommand*> SubCommandMap;
    /// storage for subcommands
    SubCommandMap subCommands_;
    
    /// Is it a compiled simulation?
    bool isCompiledSimulation_;
};
#endif
