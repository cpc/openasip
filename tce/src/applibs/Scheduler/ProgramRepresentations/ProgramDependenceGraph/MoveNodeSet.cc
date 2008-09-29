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
 * @file MoveNodeSet.cc
 *
 * Implementation of MoveNodeSet class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "MoveNodeSet.hh"
#include "NullMove.hh"

/**
 * Fairly empty constructor.
 */
 MoveNodeSet::MoveNodeSet() {
 }

/**
 * Returns number of MoveNodes in this set
 * @return Number of nodes in this set
 */
int
MoveNodeSet::count() const {
    return moveNodes_.size();
}

/**
 * Returns a copy of a MoveNode identified by index
 *
 * @param index Index of MoveNode to return
 * @return MoveNode identified by index
 */
MoveNode&
MoveNodeSet::at(const int index) throw(OutOfRange) {
    if (index >= count()) {
        std::string msg = "MoveNodeSet at() index too high.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }
    return *moveNodes_[index];
}

/**
 * Returns the disassembly of the moves in the set in a single line.
 *
 * @return the disassembly.
 */
std::string
MoveNodeSet::toString() const {
    std::string disasm = "";
    for (int i = 0; i < count(); ++i) {
        disasm += moveNodes_.at(i)->toString() + " ; ";
    }
    return disasm;
}

/**
 * Add MoveNode to the MoveNodeSet
 *
 * @param newMove New MoveNode to add to set
 */
void
MoveNodeSet::addMoveNode(MoveNode& newMove){
    moveNodes_.push_back(&newMove);
}

void MoveNodeSet::removeMoveNode(MoveNode& node) 
    throw (IllegalRegistration) {
    for (std::vector<MoveNode*>::iterator i = moveNodes_.begin();
         i != moveNodes_.end(); i++) {
        if (*i == &node) {
            moveNodes_.erase(i);
            return;
        }
    }
    throw IllegalRegistration(
        __FILE__,__LINE__,__func__,"MoveNode not in MoveNodeSet");
}
