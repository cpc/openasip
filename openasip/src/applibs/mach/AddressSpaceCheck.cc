/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file AddressSpaceCheck.hh
 *
 * Implementation of AddressSpaceCheck class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "AddressSpaceCheck.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "Machine.hh"
#include "Operation.hh"
#include "OperationPool.hh"
#include "TCEString.hh"
#include "MachineCheckResults.hh"

using namespace TTAMachine;

/**
 * The constructor.
 * 
 * @param operationPool operation pool to search the operations for
 */
AddressSpaceCheck::AddressSpaceCheck(OperationPool& operationPool) :    
    MachineCheck("Address space check."),
    operationPool_(operationPool) {
}

/**
 * The destructor.
 */
AddressSpaceCheck::~AddressSpaceCheck() {
}

/**
 * Checks that if the FUs' operations access memory, 
 * there is a memory address space available for them.
 *
 * @param mach Machine to be checked.
 * @param results Check results.
 * @return True if the check passed
 */
bool
AddressSpaceCheck::check(
    const TTAMachine::Machine& mach, 
    MachineCheckResults& results) const {
        
    Machine::FunctionUnitNavigator FUs = mach.functionUnitNavigator();    
    for (int i = 0; i < FUs.count(); i++) {
        FunctionUnit* unit = FUs.item(i);
        
        if (unit->addressSpace() != NULL) {
            continue;
        }
        
        for (int j = 0; j < unit->operationCount(); ++j) {
            Operation& op = 
                operationPool_.operation(unit->operation(j)->name().c_str());

            // skip any Nulloperations because the check is done elsewhere
            if (&op == &NullOperation::instance()) {
                continue;
            }
            
            if (op.usesMemory() && unit->addressSpace() == NULL) {
                results.addError(*this, 
                    std::string("operation ") + op.name() + " accesses memory" +
                    " but the memory address space was not set.");
                return false;
            }
        }
    } // end for all FUs

    return true;
}

