/**
 * @file MachineInfo.hh
 *
 * Declaration of MachineInfo class.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_INFO_HH
#define TTA_MACHINE_INFO_HH

namespace TTAMachine {
    class Machine;
}

#include "OperationDAGSelector.hh"

/**
 * Validator for function units.
 */
class MachineInfo {
public:
    static OperationDAGSelector::OperationSet getOpset(
        const TTAMachine::Machine& mach);
private:
    MachineInfo();
};

#endif
