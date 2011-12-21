/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file SequentialMoveNodeSelector.cc
 *
 * Implementation of SequentialModeNodeSelector class.
 *
 * @author Heikki Kultala 2008 (hkultala-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2010 (pjaaskel)
 * @note rating: red
 */

#include "SequentialMoveNodeSelector.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "SpecialRegisterPort.hh"
#include "TerminalFUPort.hh"
#include "ProgramOperation.hh"
#include "FunctionUnit.hh"
#include "TCEString.hh"
#include "MoveNodeGroupBuilder.hh"
#include "MoveNode.hh"

namespace TTAProgram {
}

namespace TTAMachine {
    class SpecialRegisterPort;
}

/**
 * Constructor. 
 * 
 * @param bb Basic block containing moves to be selected.
 */
SequentialMoveNodeSelector::SequentialMoveNodeSelector(
    TTAProgram::BasicBlock& bb) {
    MoveNodeGroupBuilder builder;
    mngs_ = builder.build(bb);
    mngIter_ = mngs_->begin();
}

SequentialMoveNodeSelector::~SequentialMoveNodeSelector() {
    while(!mngs_->empty()) {
        std::list<MoveNodeGroup*>::iterator iter = mngs_->begin();
        MoveNodeGroup* mng = *iter;
        for (int i = 0; i < mng->nodeCount(); i++) {
            delete &mng->node(i);
        }
        delete mng;
        mngs_->erase(iter);
    }
    delete mngs_;
}

/**
 * Gives a group of unselected movenodes to schedules.
 *
 * This always returns them in order; and every one just once.
 * Returns an empty MNG when there's no more MNGS to return.
 */
MoveNodeGroup 
SequentialMoveNodeSelector::candidates() {
    // are there any unselected movenodegroups left?
    if (mngIter_ != mngs_->end()) {
        return **mngIter_++;
    } else {
        // return empty mng
        return MoveNodeGroup();
    }
}

void 
SequentialMoveNodeSelector::mightBeReady(MoveNode&) {
}

void 
SequentialMoveNodeSelector::notifyScheduled(MoveNode&) {
}

