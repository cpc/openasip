/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file MoveNode.hh
 *
 * Declaration of MoveNode class.
 *
 * MoveNodes are the minimum independent unit of information in a
 * minimally-ordered program representation. Typically, but not necessarily,
 * the nodes in a program representation are linked together by dependences
 * and thus form a graph.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @author Ari Mets‰halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2010, 2011
 * @note rating: red
 */

#ifndef TTA_MOVE_NODE_HH
#define TTA_MOVE_NODE_HH

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include "Exception.hh"
#include "GraphNode.hh"

// dummy classes until real implementations are written
class Scope{} ;
// Implementation in header file that includes this header file
class ProgramOperation;
typedef boost::shared_ptr<ProgramOperation> ProgramOperationPtr;

namespace TTAProgram{
    class Move;
}

/**
 * Node of the program representation.
 *
 * A MoveNode represents one move of a TTA program.
 */
class MoveNode : public GraphNode {
public:

    explicit MoveNode(std::shared_ptr<TTAProgram::Move> newmove);
    virtual ~MoveNode();

    MoveNode* copy();

    bool isSourceOperation() const;
    inline bool isDestinationOperation() const;
    inline unsigned int destinationOperationCount() const;

    bool isOperationMove() const;

    bool isSourceVariable() const;
    bool isSourceConstant() const;
    bool isSourceRA() const;
    bool isSourceImmediateRegister() const;
    bool isDestinationVariable() const;
    bool isBypass() const;

    bool isRegisterMove() const;
    bool inSameOperation(const MoveNode& other) const;

    bool isPlaced() const;
    bool isAssigned() const;
    bool isScheduled() const;
    void setCycle(const int newcycle);
    void unsetCycle();
    int cycle() const;

    int earliestResultReadCycle() const;
    int latestTriggerWriteCycle() const;
    int guardLatency() const;

    Scope& scope();
    ProgramOperation& sourceOperation() const;
    ProgramOperation& destinationOperation(unsigned int index = 0) const;
    ProgramOperationPtr sourceOperationPtr() const;
    ProgramOperationPtr destinationOperationPtr(unsigned int index = 0) const;

    std::shared_ptr<TTAProgram::Move> movePtr();
    std::shared_ptr<const TTAProgram::Move> movePtr() const;

    TTAProgram::Move& move();
    const TTAProgram::Move& move() const;

    void addDestinationOperationPtr(ProgramOperationPtr po);
    void setSourceOperationPtr(ProgramOperationPtr po);

    void clearDestinationOperation();
    void removeDestinationOperation(const ProgramOperation* po);
    void unsetSourceOperation();

    void finalize();
    bool isFinalized() const;

    void setIsInFrontier(bool inFrontier = true);
    bool isInFrontier() const;
    bool isLastUnscheduledMoveOfDstOp() const;

    // to allow printing of graph
    int type();
    std::string toString() const;
    std::string dotString() const;

    // is move or entry node?
    inline bool isMove() const;
    bool isSourceReg(const std::string& reg) const;

    /// Node can be entry node
    MoveNode();
private:
    /// Copying forbidden. Use copy() for a deep copy.
    MoveNode(const MoveNode&);
    /// Assignment forbidden
    MoveNode& operator=(const MoveNode&);

    /// Pointer to Move this node represents, Node itself do not change move
    const std::shared_ptr<TTAProgram::Move> move_;

    std::vector<ProgramOperationPtr> dstOps_;

    ProgramOperationPtr srcOp_;

    /// Cycle in which the node is placed. Each cycle uniquely identifies an
    /// instruction slot within the current scheduling scope.
    int cycle_;

    /// True when the node placed (is given a cycle in program).
    bool placed_;

    /// The movenode cannot be unscheduled anymore, fixed in place.
    bool finalized_;

    /// This is in scheduling frontier(used in Bubblefish scheduler)
    bool isInFrontier_;
};

#include "MoveNode.icc"

#endif
