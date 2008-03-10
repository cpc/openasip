/**
 * @file FUValidator.cc
 *
 * Implementation of FUValidator class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "FUValidator.hh"
#include "MachineValidator.hh"
#include "MachineValidatorResults.hh"

#include "FunctionUnit.hh"
#include "ExecutionPipeline.hh"
#include "HWOperation.hh"

using namespace TTAMachine;

/**
 * Checks that the operands used in the operations of the given FU are
 * bound to some port.
 *
 * @param fu The function unit.
 * @param results Results of the validation are added to the given instance.
 */
void
FUValidator::checkOperandBindings(
    const TTAMachine::FunctionUnit& fu,
    MachineValidatorResults& results) {

    for (int i = 0; i < fu.operationCount(); i++) {
        HWOperation* operation = fu.operation(i);
        ExecutionPipeline* pLine = operation->pipeline();
        ExecutionPipeline::OperandSet usedOperands;
        for (int i = 0; i < pLine->latency(); i++) {
            ExecutionPipeline::OperandSet readOperands =
                pLine->readOperands(i);
            ExecutionPipeline::OperandSet writtenOperands =
                pLine->writtenOperands(i);
            usedOperands.insert(
                readOperands.begin(), readOperands.end());
            usedOperands.insert(
                writtenOperands.begin(), writtenOperands.end());
        }


        for (ExecutionPipeline::OperandSet::const_iterator iter =
                 usedOperands.begin();
             iter != usedOperands.end(); iter++) {
            int io = *iter;
            FUPort* port = operation->port(io);
            if (port == NULL) {
                boost::format errorMsg(
                    "IO number %1% is not bound to any port in "
                    "operation %2% in FU %3%.");
                errorMsg % io % operation->name() % fu.name();
                results.addError(
                    MachineValidator::USED_IO_NOT_BOUND, errorMsg.str());
            }
        }
    }
}
