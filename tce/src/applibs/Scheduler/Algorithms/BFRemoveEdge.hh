/*
    Copyright (c) 2002-2018 Tampere University.

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
 * @file BFRemoveEdge.hh
 *
 * Declaration of BF framework operation which removes a ddg edge.
 *
 * @author Heikki Kultala 2018 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */
#ifndef TTA_BF_REMOVE_EDGE_HH
#define TTA_BF_REMOVE_EDGE_HH

#include "BFOptimization.hh"

class MoveNode;
class DataDependenceEdge;

class BFRemoveEdge : public BFOptimization {

public:
    BFRemoveEdge(
        BF2Scheduler& sched,
        MoveNode& nTail, MoveNode& nHead,
        DataDependenceEdge& e);

    bool operator()() override;
    void undoOnlyMe() override;

    virtual ~BFRemoveEdge();
private:
    DataDependenceEdge& e_;
    MoveNode& nTail_;
    MoveNode& nHead_;
};

#endif
