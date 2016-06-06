/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file FUValidator.cc
 *
 * Implementation of FUValidator class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "FUValidator.hh"
#include "MachineValidator.hh"
#include "MachineValidatorResults.hh"

#include "FunctionUnit.hh"
#include "ExecutionPipeline.hh"
#include "HWOperation.hh"
#include "FUPort.hh"

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


/**
 * Checks that the FU has at least one operation which is valid, i.e.
 * has at least one triggering input port
 *
 * @param fu The function unit.
 * @param results Results of the validation are added to the given instance.
 */
void
FUValidator::checkOperations(
    const TTAMachine::FunctionUnit& fu,
    MachineValidatorResults& results) {

    for (int i = 0; i < fu.operationCount(); i++) {
        HWOperation* operation = fu.operation(i);
        ExecutionPipeline* pLine = operation->pipeline();
        ExecutionPipeline::OperandSet usedOperands;
        for (int i = 0; i < pLine->latency(); i++) {
            ExecutionPipeline::OperandSet readOperands =
                pLine->readOperands(i);
            for (ExecutionPipeline::OperandSet::const_iterator iter =
                    readOperands.begin(); iter != readOperands.end(); iter++) {
                FUPort* port = operation->port(*iter);
                if (port != NULL && port->isTriggering()) {
                    return;
                }
            }
        }
    }

    boost::format errorMsg("FU %1% has no valid operations.");
    errorMsg % fu.name();
    results.addError(MachineValidator::FU_NO_VALID_OPERATIONS, errorMsg.str());
}
