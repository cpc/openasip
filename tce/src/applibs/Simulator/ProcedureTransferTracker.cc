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
 * @file ProcedureTransferTracker.cc
 *
 * Definition of ProcedureTransferTracker class.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProcedureTransferTracker.hh"
#include "ExecutionTrace.hh"
#include "Application.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorToolbox.hh"
#include "SimulationEventHandler.hh"
#include "Instruction.hh"
#include "ControlUnit.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "TerminalFUPort.hh"
#include "SpecialRegisterPort.hh"
#include "StringTools.hh"
#include "Operation.hh"

/**
 * Constructor.
 *
 * @param subject The SimulationController which is observed.
 * @param traceDB The Execution Trace Database instance in which the
 *                trace is stored. Expects that the database is open
 *                for writing.
 */
ProcedureTransferTracker::ProcedureTransferTracker(
    SimulatorFrontend& subject,
    ExecutionTrace& traceDB) : Listener(),
    subject_(subject), traceDB_(traceDB), previousInstruction_(NULL) {
    subject.eventHandler().registerListener(
        SimulationEventHandler::SE_CYCLE_END, this);
}

/**
 * Destructor.
 */
ProcedureTransferTracker::~ProcedureTransferTracker() {
    subject_.eventHandler().unregisterListener(
        SimulationEventHandler::SE_CYCLE_END, this);
}

/**
 * Stored procedure transfer data to execution database.
 *
 * Last executed instruction is saved and its procedure is compared to the
 * procedure of the current instruction. If they differ, a procedure transfer
 * has happened and data of it is stored in trace database.
 */
void 
ProcedureTransferTracker::handleEvent() {

    // the instruction that WAS executed in the current cycle (this handles
    // clock cycle *end* events)
    const TTAProgram::Instruction& currentInstruction =
        subject_.program().instructionAt(subject_.lastExecutedInstruction());

    if (!currentInstruction.isInProcedure() ||
        (previousInstruction_ != NULL && &(currentInstruction.parent()) == 
         &(previousInstruction_->parent()))) {
        // the instrution is not in a procedure (probably hand coded 
        // assembly program without procedure info), or there was no
        // procedure transfer
        previousInstruction_ = &currentInstruction;
        return;
    }

    bool entry = true;
    // in case this is the first instruction, consider this a procedure
    // entry (to the first procedure)
    if (previousInstruction_ != NULL) {
        // find the instruction which is responsible for the procedure 
        // change: it should be the instruction at the last executed
        // instruction at the source procedure minus the count of delay
        // slots
        const InstructionAddress lastControlFlowInstructionAddress = 
            previousInstruction_->address().location() - 
            subject_.machine().controlUnit()->delaySlots();
        const TTAProgram::Instruction& lastControlFlowInstruction =
            subject_.program().instructionAt(
                lastControlFlowInstructionAddress);

        // check if the instruction contains a move to gcu.jump.
        // if there's no such a move, we'll assume it's a call
        for (int i = 0; i < lastControlFlowInstruction.moveCount(); ++i) {
        
            TTAProgram::Move& lastMove = lastControlFlowInstruction.move(i);
            if (dynamic_cast<TTAProgram::TerminalFUPort*>(
                &lastMove.destination()) == 0) {
                continue;
            }
            if (lastMove.destination().isOpcodeSetting() && lastMove.isJump()) {
                entry = false;
                break;
            }
        }
    }  
    previousInstruction_ = &currentInstruction;
    try {
        if (entry) {
            traceDB_.addProcedureTransfer(
                subject_.cycleCount(), subject_.lastExecutedInstruction(), 
                ExecutionTrace::PT_ENTRY);
        } else {
            traceDB_.addProcedureTransfer(
                subject_.cycleCount(), subject_.lastExecutedInstruction(), 
                ExecutionTrace::PT_EXIT);
        }
    } catch (const Exception& e) {
        debugLog("Error while writing TraceDB: " + e.errorMessage());
    }
}
