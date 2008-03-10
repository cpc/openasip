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
