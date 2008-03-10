/**
 * @file MachineInfo.cc
 *
 * Implementation of MachineInfo class.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#include "MachineInfo.hh"

#include "StringTools.hh"
#include "Machine.hh"
#include "HWOperation.hh"
#include "OperationPool.hh"

using namespace TTAMachine;

/**
 * Checks that the operands used in the operations of the given FU are
 * bound to some port.
 *
 * @param mach The machine whose opset is requested.
 * @return Opset supported by machine hardware.
 */
OperationDAGSelector::OperationSet
MachineInfo::getOpset(const TTAMachine::Machine &mach) {

    OperationDAGSelector::OperationSet opNames;

    const TTAMachine::Machine::FunctionUnitNavigator fuNav =
        mach.functionUnitNavigator();

    OperationPool opPool;

    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit* fu = fuNav.item(i);
        for (int o = 0; o < fu->operationCount(); o++) {
            const std::string opName = fu->operation(o)->name();
            opNames.insert(StringTools::stringToUpper(opName));
        }
    }
    
    return opNames;
}
