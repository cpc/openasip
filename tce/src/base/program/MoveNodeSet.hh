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
 * @file MoveNodeSet.hh
 *
 * Declaration of MoveNodeSet class.
 *
 * @author Heikki Kultala 2006 (hkultala-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TCE_MOVE_NODE_SET_HH
#define TCE_MOVE_NODE_SET_HH

#include "MoveNode.hh"
#include "Exception.hh"
#include <vector>

class MoveNodeSet{
public:

    MoveNodeSet();

    int count() const;
    MoveNode& at(int index) throw(OutOfRange);
    void addMoveNode(MoveNode&);
    void removeMoveNode(MoveNode&) throw (IllegalRegistration);

    std::string toString() const;

private:
    // Vector holding pointers to MoveNodes in a collection
    std::vector<MoveNode*> moveNodes_;
};

#endif
