/**
 * @file AddressSpaceCheck.hh
 *
 * Declaration of AddressSpaceCheck class that checks two things:
 * 1) if the function units' operations access memory
 * 2) if there is a memory address space available for them
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef ADDRESS_SPACE_CHECK_HH
#define ADDRESS_SPACE_CHECK_HH

#include "MachineCheck.hh"

namespace TTAMachine {
    class Machine;
}

class OperationPool;

/**
 * MachineCheck to check if the FUs' operations access memory and
 * there is a memory address space available for them.
 */
class AddressSpaceCheck : public MachineCheck {
public:
    AddressSpaceCheck(OperationPool& operationPool);
    virtual ~AddressSpaceCheck();

    virtual bool check(const TTAMachine::Machine& mach,
                       MachineCheckResults& results) const;
    
private:
    OperationPool& operationPool_;

};

#endif
