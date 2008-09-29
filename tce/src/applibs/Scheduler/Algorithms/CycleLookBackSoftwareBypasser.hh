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
 * @file CycleLookBackSoftwareBypasser.hh
 *
 * Declaration of CycleLookBackSoftwareBypasser interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CYCLE_LOOK_BACK_SOFTWARE_BYPASSER_HH
#define TTA_CYCLE_LOOK_BACK_SOFTWARE_BYPASSER_HH

#include <map>
#include "SoftwareBypasser.hh"

class MoveNodeSelector;
class MoveNode;
/**
 * A simple implementation of software bypassing that reschedules operand
 * writes as bypassed moves in case the result has been produced in n
 * previous cycles.
 */
class CycleLookBackSoftwareBypasser : public SoftwareBypasser {
public:
    CycleLookBackSoftwareBypasser(int cyclesToLookBack=1,
                                  bool killDeadResult=true);
    virtual ~CycleLookBackSoftwareBypasser();

    virtual int bypass(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm);

    virtual void removeBypass(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm);
    virtual int removeDeadResults(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm);

    void setSelector(MoveNodeSelector* selector);
    
private:
    /// count of cycles before the operand write to look for the producer
    /// of the read value
    int cyclesToLookBack_;

    // whether dead resutls should be killed.
    bool killDeadResults_;

    int bypassNode(
        MoveNode& nodeToBypass,
        int& lastOperandCycle,
        DataDependenceGraph& ddg,
        ResourceManager& rm);
        
    /// Stores sources and bypassed moves in case they
    /// have to be unassigned (case when operands are scheduled
    /// and bypassed but result can not be scheduled with such operands
    // First is bypassed node, second is original source
    std::map<MoveNode*, MoveNode*> storedSources_;
    MoveNodeSelector* selector_;
};

#endif
