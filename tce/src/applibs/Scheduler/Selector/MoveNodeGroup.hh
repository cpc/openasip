/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file MoveNodeGroup.hh
 * 
 * Declaration of MoveNodeGroup class 
 * 
 * @author Heikki Kultala 2006 (heikki.kultala-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011
 * @note rating: red
 */

#ifndef TTA_MOVE_NODE_GROUP_HH
#define TTA_MOVE_NODE_GROUP_HH

#include <vector>

#include "Exception.hh"
#include "ProgramOperation.hh"

class DataDependenceGraph;
class MoveNode;

/**
 * Class which contains group of moves which are to be scheduled.
 */
class MoveNodeGroup {
public:
    MoveNodeGroup();
    MoveNodeGroup(const DataDependenceGraph& ddg);

    virtual ~MoveNodeGroup();

    void addNode(MoveNode& node);
    
    int earliestCycle() const;
    int latestCycle() const;

    int maxSinkDistance() const;
    int maxSourceDistance() const;

    int nodeCount() const;
    MoveNode& node(int index) const;

    bool isScheduled() const;
    std::string toString() const;
    bool isAlive() const;

    /// in case this MNG contains strictly the nodes of a single operation,
    /// it can be set and queried with these methods
    void setProgramOperationPtr(ProgramOperationPtr op) { operation_ = op; }
    bool isOperation() const { return operation_ != NULL; }
    ProgramOperationPtr programOperationPtr() const { return operation_; }

private:
    std::vector<MoveNode*> nodes_;
    /// The data dependence graph the moves in this group belong to 
    /// (optional).
    const DataDependenceGraph* ddg_;
    /// in case this MNG contains strictly the nodes of a single operation
    /// this can be set to point to it (optional)
    ProgramOperationPtr operation_;
};

#endif
