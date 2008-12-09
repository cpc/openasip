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
