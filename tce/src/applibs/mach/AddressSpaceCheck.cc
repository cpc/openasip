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

