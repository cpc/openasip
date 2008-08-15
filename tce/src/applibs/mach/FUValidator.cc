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
