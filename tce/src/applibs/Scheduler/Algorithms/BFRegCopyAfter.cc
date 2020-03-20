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
 * @file BFRegCopyAfter.cc
 *
 * Definition of BFRegCopyAfter class
 *
 * Creates a register-to-register copy after a move, modifying the destination
 * of the original move into the temporaty register and makint the destination
 * of the temp move the original destination register.
 *
 * Used for tempreg copies of result moves.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFRegCopyAfter.hh"
#include "MoveNode.hh"
#include "DataDependenceGraph.hh"
#include "RegisterFile.hh"
#include "TerminalRegister.hh"
#include "Move.hh"
#include "BF2Scheduler.hh"
#include "BFConnectNodes.hh"
#include "BFRemoveEdge.hh"
#include "BFScheduleBU.hh"
bool
BFRegCopyAfter::splitMove(BasicBlockNode& bbn) {

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tBFRegCopyAfter splitting move: "
              << mn_.toString() << std::endl;
#endif

    std::set<const TTAMachine::RegisterFile*,
        TTAMachine::MachinePart::Comparator>
        rfs = sched_.possibleTempRegRFs(mn_, true);

    // cannot create reg copy if no temp regs available.
    if (rfs.empty()) {
        return false;
    }

    const TTAMachine::RegisterFile& rf = **rfs.rbegin();

    int lastRegisterIndex = rf.size()-1;
    TTAMachine::Port* dstRFPort = rf.firstWritePort();
    TTAMachine::Port* srcRFPort = rf.firstReadPort();

    TTAProgram::TerminalRegister* tempWrite =
        new TTAProgram::TerminalRegister(*dstRFPort, lastRegisterIndex);

    TTAProgram::TerminalRegister* tempRead =
        new TTAProgram::TerminalRegister(*srcRFPort, lastRegisterIndex);

    regCopy_->move().setSource(tempRead);
    mn_.move().setDestination(tempWrite);

    auto iEdges = ddg().rootGraph()->inEdges(mn_);
    for (auto iEdge : iEdges) {
        if (!iEdge->isRegisterOrRA() || iEdge->headPseudo()) {
            continue;
        }

        //copy guard use edge to the regcopy.
        if (iEdge->guardUse() &&
            iEdge->dependenceType() == DataDependenceEdge::DEP_RAW) {
            MoveNode& tail = ddg().rootGraph()->tailNode(*iEdge);
            runPostChild(
                new BFConnectNodes(sched_, tail, *regCopy_, iEdge, true));
        } else if (iEdge->isFalseDep()) {
            MoveNode& tail = ddg().rootGraph()->tailNode(*iEdge);
            runPostChild(new BFRemoveEdge(sched_, tail, mn_, *iEdge));
            runPostChild(new BFConnectNodes(sched_, tail, *regCopy_, iEdge));
        }
    }

    auto oEdges = ddg().rootGraph()->outEdges(mn_);
    for (auto oEdge : oEdges) {
        if (!oEdge->isRegisterOrRA() || oEdge->tailPseudo()) {
            continue;
        }

        if (oEdge->dependenceType() == DataDependenceEdge::DEP_WAW ||
            oEdge->dependenceType() == DataDependenceEdge::DEP_RAW) {

            MoveNode& head = ddg().rootGraph()->headNode(*oEdge);
            runPostChild(new BFRemoveEdge(sched_, mn_, head, *oEdge));
            runPostChild(new BFConnectNodes(sched_, *regCopy_, head, oEdge));
        }

        if (oEdge->dependenceType() == DataDependenceEdge::DEP_WAR &&
            oEdge->guardUse()) {
            MoveNode& head = ddg().rootGraph()->headNode(*oEdge);
            runPostChild(
                new BFConnectNodes(sched_, *regCopy_, head, oEdge, true));
        }
    }

    TCEString tempRegName = rf.name() + '.' +
        Conversion::toString(lastRegisterIndex);

    DataDependenceEdge* newEdge =
        new DataDependenceEdge(
            DataDependenceEdge::EDGE_REGISTER,
            DataDependenceEdge::DEP_RAW, tempRegName);

    runPostChild(new BFConnectNodes(sched_, mn_, *regCopy_, newEdge));

    createAntidepsForReg(
        mn_, *regCopy_, rf, lastRegisterIndex,
        tempRegName, bbn, ii()!= 0);

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << " \t\t\tMoves after split: " << mn_.toString() << " and " <<
        regCopy_->toString() << std::endl;
#endif

    // TODO: bypass regcopies??
    BFScheduleBU* regCopySched =
	new BFScheduleBU(sched_,*regCopy_, lc_, true, true, false);
    bool ok = runPostChild(regCopySched);
    if (!ok) {
        undoAndRemovePostChildren();
        undoSplit();
        return false;
    }
    return true;
}

void
BFRegCopyAfter::undoSplit() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tBFRegCopyAfter undoing split move: " << mn_.toString()
              << " and " << regCopy_->toString() << std::endl;
#endif
    mn_.move().setDestination(regCopy_->move().destination().copy());
}
