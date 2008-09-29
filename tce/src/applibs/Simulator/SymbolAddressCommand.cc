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
 * @file SymbolAddressCommand.cc
 *
 * Implementation of SymbolAddressCommand class
 *
 * @author Pekka J‰‰skel‰inen 2008 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "SymbolAddressCommand.hh"
#include "SimulatorFrontend.hh"
#include "MemorySystem.hh"
#include "SimulatorToolbox.hh"
#include "Memory.hh"
#include "StringTools.hh"
#include "Address.hh"
#include "NullAddressSpace.hh"
#include "SimValue.hh"
#include "GlobalScope.hh"
#include "DataLabel.hh"

#include <iostream>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
SymbolAddressCommand::SymbolAddressCommand() : 
    SimControlLanguageCommand("symbol_address") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
SymbolAddressCommand::~SymbolAddressCommand() {
}

/**
 * Executes the "symbol_address" command.
 *
 * Prints the address of a data symbol.
 *
 * @param arguments The symbol to print.
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 */
bool 
SymbolAddressCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    const int argumentCount = arguments.size() - 1;
    if (!checkArgumentCount(argumentCount, 1, 1)) {
        return false;
    } 

    if (!checkProgramLoaded()) {
        return false;
    }

    const TTAProgram::GlobalScope& globalScope = 
        simulatorFrontend().program().globalScopeConst();        
    try {
        TTAProgram::Address address =
            globalScope.dataLabel(arguments.at(1).stringValue()).address();
        interpreter()->setResult(static_cast<int>(address.location()));
        interpreter()->setError(false);
        return true;
    } catch (const KeyNotFound&) {
        // couldn't evaluate the label 
        interpreter()->setResult(
            "Symbol not found. Try adding '_' prefix to the symbol, "
            "e.g. '_foo'.");
        interpreter()->setError(true);
        return false;
    }
}

/**
 * Returns the help text for this command.
 * 
 * Help text is searched from SimulatorTextGenerator.
 *
 * @return The help text.
 */
std::string 
SymbolAddressCommand::helpText() const {
    return "Returns the address of the given data symbol (global variable).";
}
