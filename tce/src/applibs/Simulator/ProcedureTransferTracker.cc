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
