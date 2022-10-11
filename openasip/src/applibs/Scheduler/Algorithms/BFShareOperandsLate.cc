/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @file BFShareOperandsLate.cc
 *
 * Definition of BFShareOperandsLate class
 *
 * After scheduling a move,
 * Searches for potential operations to share operand with, and
 * then calls BFShareOperandLate to fo the actual operand sharing.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */


#include "BFShareOperandsLate.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "MoveNode.hh"
#include "SimpleResourceManager.hh"
#include "Instruction.hh"
#include "DataDependenceGraph.hh"
#include "BFShareOperandLate.hh"
#include "SchedulerCmdLineOptions.hh"

BFShareOperandsLate::BFShareOperandsLate(BF2Scheduler& sched, MoveNode& mn) :
    BFOptimization(sched), mn_(mn), operandShareDistance_(6) {
    SchedulerCmdLineOptions* opts =
        dynamic_cast<SchedulerCmdLineOptions*>(Application::cmdLineOptions());
    if (opts != NULL) {
        if (opts->operandShareDistance() > -1) {
            operandShareDistance_ = opts->operandShareDistance();
        }
    }

}


bool BFShareOperandsLate::operator()() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tTrying share operands late for: " << mn_.toString()
              << std::endl;
#endif
    TTAProgram::Move& currMove = mn_.move();
    TTAProgram::Terminal& currDest = currMove.destination();
    TTAProgram::Terminal& currSrc = currMove.source();
    if (currSrc.isImmediateRegister() ||
        currSrc.isFUPort()) {
        // lets not analyze yet when data in these change
        return false;
    }

    if (!currDest.isFUPort() ||
        currDest.isTriggering() ||
        currDest.isOpcodeSetting()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tdest not operand port" << std::endl;
#endif
        return false;
    }

//    TTAMachine::RegisterFile* guardRF = NULL;
//    int guardIndex = -1;
//    bool guardInverted = false;
    if (!currMove.isUnconditional()) {
        return false;
        /*
        TTAMachine::Guard& guard = move.guard().guard();
        RegisterGuard* rg = dynamic_cast<TTAMachine::RegisterGuard*>(&guard);
        if (rg) {
            guardRF = rg->registerFile();
            guardIndex = rg->registerIndex();
            guardInverted = rg->isInverted();
        }
        */
    }


    int cycle = mn_.cycle();
    unsigned int idx = rm().instructionIndex(cycle);
    unsigned int limit = ii() ? std::min(ii(), idx+operandShareDistance_) :
        std::min((unsigned)rm().largestCycle()+1, idx+operandShareDistance_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\tidx: " << idx << " limit:" << limit << std::endl;
#endif
    for (unsigned int i = idx; i < limit; i++) {
        const TTAProgram::Instruction& ins = *rm().instruction(i);
        for (int j = 0; j < ins.moveCount(); j++) {
            const TTAProgram::Move& laterMove = ins.move(j);
            const TTAProgram::Terminal& laterDest = laterMove.destination();
            if (&laterMove == &currMove) {
                continue;
            }
            if (!laterDest.equals(currDest)) {
                continue;
            }

            const TTAProgram::Terminal& laterSrc = laterMove.source();
            if (laterSrc.equals(currSrc)) {
                MoveNode& laterNode = ddg().nodeOfMove(laterMove);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\t\t\tOriginal scheduled move: " << mn_.toString()
                          << std::endl;
                std::cerr << "\t\t\t\tLate op sharing Could remove later move: "
                          << laterNode.toString() << std::endl;
#endif
                BFShareOperandLate* bfsol =
                    new BFShareOperandLate(sched_,laterNode, mn_);
                return runPostChild(bfsol);
            } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\t\tAnother move overwrites my dest"
                          << ddg().nodeOfMove(laterMove).toString() << std::endl;
#endif
                return false;
            }
        }
        for (int j = 0; j < ins.moveCount(); j++) {
            TTAProgram::Move& laterMove = ins.move(j);
            TTAProgram::Terminal& laterDest = laterMove.destination();
            // src is overwritten here
            if (laterDest.equals(currSrc)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\t\tOverwrites src: " <<
                    ddg().nodeOfMove(laterMove).toString() << std::endl;
#endif
                return false;
            }
        }
    }
    return false;
}

void BFShareOperandsLate::undoOnlyMe() {

}
