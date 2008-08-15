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
 * @file AddressSpaceMapper.cc
 *
 * Implementation of AddresSpaceMapper class
 *
 */


#include "AddressSpaceMapper.hh"
#include "ControlUnit.hh"
#include "Program.hh"

#include "Machine.hh"

using namespace TTAMachine;
using namespace TTAProgram;

/** Changes data address spaces of program 
 * 
 * @param program Program whose address space is being changed
 *
 * @machine machine to which we are scehduling
 */
void 
AddressSpaceMapper::mapAddressSpace(
    TTAProgram::Program& program, const TTAMachine::Machine& machine) {
    
    AddressSpace& instructionAS = *machine.controlUnit()->addressSpace();
    Machine::AddressSpaceNavigator asNav =
        machine.addressSpaceNavigator();
    AddressSpace* dataAS = NULL;
    for (int i = 0; i < asNav.count(); i++) {
        if (asNav.item(i) != &instructionAS) {
            dataAS = asNav.item(i);
            break;
        }
    }
    program.replaceUniversalAddressSpaces(*dataAS);
}
