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
 * @file DisassembleCommand.cc
 *
 * Implementation of DisassembleCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassembleCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "SimControlLanguageCommand.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimValue.hh"
#include "Program.hh"
#include "Procedure.hh"
#include "Address.hh"
#include "Instruction.hh"

#include <iostream>

using namespace TTAProgram;

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
DisassembleCommand::DisassembleCommand() : 
    SimControlLanguageCommand("disassemble") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
DisassembleCommand::~DisassembleCommand() {
}

/**
 * Executes the "disassemble" command.
 *
 * Prints a range of meory addresses as machine instructions. When two 
 * arguments addr1, addr2 are given, addr1 specifies the first address of the 
 * range to display, and addr2 specifies the last address (not displayed). If 
 * only one argument, addr1, is given, then the function that contains addr1 
 * is disassembled. If no argument is given, the default memory range is the 
 * function surrounding the program counter of the selected frame.
 *
 * @param arguments The range of instruction memory to disassemble.
 * @return Always true if arguments are valid.
 */
bool 
DisassembleCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    assert(interpreter() != NULL);

    if (!checkArgumentCount(arguments.size() - 1, 0, 2)) {
        return false;
    } 

    if (!checkProgramLoaded()) {
        return false;
    }

    SimulatorInterpreterContext& interpreterContext = 
        dynamic_cast<SimulatorInterpreterContext&>(interpreter()->context());

    SimulatorFrontend& simFront = interpreterContext.simulatorFrontend();

    int firstAddress = -1;
    int lastAddress = -1; 

    const int programLastAddress = 
        simFront.program().lastProcedure().endAddress().location() - 1;

    if (arguments.size() > 1) {
        try {
            firstAddress = parseInstructionAddressExpression(
                arguments[1].stringValue());
            
            if (arguments.size() == 3) {
                lastAddress = parseInstructionAddressExpression(
                    arguments[2].stringValue());
            }
        } catch (const IllegalParameters&) {
            return false;
        }
    } 

    if (firstAddress > programLastAddress) {
        firstAddress = programLastAddress;
    }

    if (firstAddress == -1 && lastAddress == -1) {
        firstAddress = simFront.currentProcedure().startAddress().location();
        lastAddress =  simFront.currentProcedure().endAddress().location() - 1;
    } else if (firstAddress != -1 && lastAddress == -1) {
        const Procedure& procedureAtAddress = 
            dynamic_cast<const Procedure&>(
                simFront.program().instructionAt(firstAddress).parent());
        firstAddress = procedureAtAddress.startAddress().location();
        lastAddress =  procedureAtAddress.endAddress().location() - 1;
    } 

    if (lastAddress < 0) {
        lastAddress = 0;
    }

    if (lastAddress > programLastAddress) {
        lastAddress = programLastAddress;
    }

    if (lastAddress < firstAddress) {
        lastAddress = firstAddress;
    }

    for (; firstAddress <= lastAddress; ++firstAddress) {
        outputStream() << simFront.disassembleInstruction(firstAddress) 
                       << std::endl;
    }

    return true;
}

/**
 * Returns the help text for this command.
 * 
 * Help text is searched from SimulatorTextGenerator.
 *
 * @return The help text.
 * @todo Use SimulatorTextGenerator to get the help text.
 */
std::string 
DisassembleCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_DISASSEMBLE).str();
}
