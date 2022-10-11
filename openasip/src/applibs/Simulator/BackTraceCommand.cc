/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file BackTraceCommand.cc
 *
 * Implementation of BackTraceCommand class
 *
 * @author Pekka Jääskeläinen 2010
 * @note rating: red
 */

#include "BackTraceCommand.hh"
#include "SimulatorInterpreterContext.hh"
#include "CallPathTracker.hh"
#include "Procedure.hh"
#include "Program.hh"
#include "Instruction.hh"

BackTraceCommand::BackTraceCommand() : SimControlLanguageCommand("bt") {
}

BackTraceCommand::~BackTraceCommand() {
}

bool
BackTraceCommand::execute(const std::vector<DataObject>& arguments) {
    if (!checkArgumentCount(arguments.size() - 1, 0, 0)) {
        return false;
    }

    if (!checkSimulationStopped()) {
        return false;
    }

    SimulatorInterpreterContext& interpreterContext = 
        dynamic_cast<SimulatorInterpreterContext&>(interpreter()->context());

    SimulatorFrontend& simulatorFrontend = 
        interpreterContext.simulatorFrontend();

    if (simulatorFrontend.callHistoryLength() == 0) {
        setErrorMessage(
            "Call history logging not enabled.\n"
            "Use 'setting call_history_length' to enable.");
        return false;
    }

    const CallPathTracker& cpt = simulatorFrontend.callPathTracker();

#if 0
    // move this to a separate call_path command or similar
    const CallPathTracker::ProcedureTransferQueue& callHistory = 
        cpt.transfers();

    for (CallPathTracker::ProcedureTransferQueue::const_iterator i = 
             callHistory.begin(); i != callHistory.end(); ++i) {
        CallPathTracker::ProcedureTransfer tr = *i;
        TCEString procName = 
            dynamic_cast<const TTAProgram::Procedure&>(
                simulatorFrontend.program().instructionAt(tr.address).
                parent()).name();
        if (tr.isEntry) {
            outputStream()
                << "called ";
        } else {
            outputStream()
                << "returned to ";
        }           
        outputStream() 
            << procName << "@" << tr.sourceAddress << std::endl;
    }
#endif

    const CallPathTracker::ProcedureTransferQueue& calls = 
        cpt.backTrace();
    int callLevel = 0;
    CallPathTracker::ProcedureTransferQueue::const_iterator i = 
        calls.begin();
    for (; i != calls.end(); ++i) {
        CallPathTracker::ProcedureTransfer tr = *i;
        TCEString procName = 
            dynamic_cast<const TTAProgram::Procedure&>(
                simulatorFrontend.program().instructionAt(tr.address).
                parent()).name();
        assert(tr.isEntry);
        outputStream() 
            << "#" << callLevel << " called " 
            << procName << "@" << tr.sourceAddress << std::endl;
        ++callLevel;
    }

    return true;
}

std::string 
BackTraceCommand::helpText() const {
    return std::string(
        "Prints the current call stack (back trace).\n"
        "Requires call history saving to be on.\n"
        "See 'setting call_history_length'.");
}

