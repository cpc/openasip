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
 * @file SequentialMoveNodeSelector.hh
 *
 * Declaration of SequentialModeNodeSelector class.
 *
 * @author Heikki Kultala 2008 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SEQUENTIAL_MOVE_NODE_SELECTOR_HH
#define TTA_SEQUENTIAL_MOVE_NODE_SELECTOR_HH

#include <list>

#include "MoveNodeSelector.hh"
#include "BasicBlock.hh"



/**
 * Selects move nodes from a basic block and prioritizes move nodes on
 * the critical path of the data dependence graph.
 */
class SequentialMoveNodeSelector : public MoveNodeSelector {
public:
    SequentialMoveNodeSelector(BasicBlock& bb);

    virtual ~SequentialMoveNodeSelector();

    virtual MoveNodeGroup candidates();
    virtual void notifyScheduled(MoveNode& node);

    void mightBeReady(MoveNode& node);
private:
    void createMoveNodes(BasicBlock& bb);

    typedef std::list<ProgramOperation*> ProgramOperationList;
    typedef std::list<MoveNodeGroup*> MNGList;

    MNGList mngs_;
    // returns the movenodegroups in order.
    // thiskeep track where we are going.
    MNGList::iterator mngIter_;
    // list of all programoperations.
    ProgramOperationList programOperations_;
};

#endif
