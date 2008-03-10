/** 
 * 
 * @file OperationBindingCheck.hh
 *
 * Declaration of OperationBindingCheck class.
 *
 * @author Heikki Kultala 2008 (hkultala@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_BINDING_CHECK
#define TTA_OPERATION_BINDING_CHECK

#include "MachineCheck.hh"

/**
 * Check that tests port bindings in machine
 */
class OperationBindingCheck : public MachineCheck {
public:
    OperationBindingCheck();

    virtual bool check(
        const TTAMachine::Machine& mach, MachineCheckResults& results) const;
};

#endif
