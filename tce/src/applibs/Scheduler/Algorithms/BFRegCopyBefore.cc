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
 * @file BFRegCopyBefore.cc
 *
 * Definition of BFRegCopyBefore class
 *
 * Creates a register-to-register copy before a move, modifying the source
 * of the original move into the temporaty register and making the source
 * of the temp move the original souce register.
 *
 * Used for temp-reg-copies of operand moves.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFRegCopyBefore.hh"
#include "RegisterFile.hh"
#include "TerminalRegister.hh"
#include "BF2Scheduler.hh"
#include "MoveNode.hh"
#include "Move.hh"
#include "BFRemoveEdge.hh"
#include "BFConnectNodes.hh"


bool
BFRegCopyBefore::splitMove(BasicBlockNode& bbn) {

    std::set<const TTAMachine::RegisterFile*,
        TTAMachine::MachinePart::Comparator>
        rfs = sched_.possibleTempRegRFs(mn_, false, forbiddenRF_);

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

    regCopy_->move().setDestination(tempWrite);
    mn_.move().setSource(tempRead);

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
        } else if (iEdge->isRAW()) {
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
        if (oEdge->dependenceType() == DataDependenceEdge::DEP_WAR &&
            !oEdge->guardUse()) {
            MoveNode& head = ddg().rootGraph()->headNode(*oEdge);
            runPostChild(new BFRemoveEdge(sched_, mn_, head, *oEdge));
            runPostChild(new BFConnectNodes(sched_, *regCopy_, head, oEdge));
        }
    }

    TCEString tempRegName = rf.name() + '.' +
        Conversion::toString(lastRegisterIndex);

    DataDependenceEdge* newEdge =
        new DataDependenceEdge(
            DataDependenceEdge::EDGE_REGISTER,
            DataDependenceEdge::DEP_RAW, tempRegName);

    runPostChild(new BFConnectNodes(sched_, *regCopy_, mn_, newEdge));

    createAntidepsForReg(
        *regCopy_, mn_, rf, lastRegisterIndex,
        tempRegName, bbn, ii()!= 0);

    return true;
}

void BFRegCopyBefore::undoSplit() {
    mn_.move().setSource(regCopy_->move().source().copy());
}
