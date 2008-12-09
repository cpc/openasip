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
