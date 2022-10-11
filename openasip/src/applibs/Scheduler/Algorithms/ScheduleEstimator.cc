/*
    Copyright (c) 2002-2013 Tampere University.

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
 * @file ScheduleEstimator.cc
 *
 * Definition of ScheduleEstimator class.
 *
 * This class tries to estimate the worst possible cycle count
 * for a basic block.
 *
 * @author Heikki Kultala 2019 (hkultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "ScheduleEstimator.hh"
#include "BasicBlock.hh"
#include "MachineConnectivityCheck.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "Machine.hh"
#include "ControlUnit.hh"
#include "MachineInfo.hh"
#include "Operation.hh"
#include "ControlFlowGraph.hh"

int
ScheduleEstimator::maximumSizeOfBB(
    TTAProgram::BasicBlock& bb, const TTAMachine::Machine& mach) {

    auto tempRegFiles = MachineConnectivityCheck::tempRegisterFiles(mach);
    int tempRegCount = tempRegFiles.size();

    int size = 0;
    int iCount = bb.instructionCount();

    for (int i = 0; i < iCount; i++) {
        auto& ins = bb[i];
        for (int j = 0; j < ins.moveCount(); j++) {
            size++;
            // temp reg copies may make schedule longer.
            size += tempRegCount;
            auto& m = ins.move(j);
            auto& src = m.source();

            if (m.isControlFlowMove()) {
                size += mach.controlUnit()->delaySlots() + 1;
            }
            if (src.isFUPort() && !src.isRA()) {
                const Operation& o = src.hintOperation();
                TCEString opName = o.name();
                int lat = MachineInfo::maxLatency(mach,opName);
                if (lat > 0) {
                    size += lat-1;
                }
            }
        }
    }
    return size;
}


void
ScheduleEstimator::handleControlFlowGraph(
    ControlFlowGraph& cfg, const TTAMachine::Machine& mach) {

    for (int bbIndex = 0; bbIndex < cfg.nodeCount(); ++bbIndex) {
        BasicBlockNode& bbn = cfg.node(bbIndex);
        if (!bbn.isNormalBB()) {
            bbn.setMaximumSize(0);
        } else {
            auto& bb = bbn.basicBlock();
            bbn.setMaximumSize(
                bbn.isScheduled() ?
                bb.instructionCount() - bb.skippedFirstInstructions() :
                maximumSizeOfBB(bb, mach));
        }
    }
}
