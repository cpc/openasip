/**
 * @file AddressSpaceCheck.hh
 *
 * Implementation of AddressSpaceCheck class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
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

