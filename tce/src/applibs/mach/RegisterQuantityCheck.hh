/**
 * @file RegisterQuantityCheck.hh
 * 
 * Declaration of RegisterQuantityCheck class.
 * 
 * Tests that given machine has enough registers.
 *
 * @author Heikki Kultala (hkultala@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_REGISTER_QUANTITY_CHECK_HH
#define TTA_REGISTER_QUANTITY_CHECK_HH

#include "MachineCheck.hh"

#include <string>

class RegisterQuantityCheck : public MachineCheck {
public:
    RegisterQuantityCheck();
    virtual ~RegisterQuantityCheck();

    virtual bool check(
        const TTAMachine::Machine& mach, MachineCheckResults& results) const;
};

#endif
