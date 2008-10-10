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

    // list of movenodegroups
    std::list<MoveNodeGroup*> mngs_;

    // returns the movenodegroups in order.
    // thiskeep track where we are going.
    std::list<MoveNodeGroup*>::iterator mngIter_;

    // list of all programoperations.
    std::list<ProgramOperation*> programOperations_;
};

#endif
