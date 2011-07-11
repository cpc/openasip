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
 * @author Ari Metsähalme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_MOVE_NODE_HH
#define TTA_MOVE_NODE_HH

#include <string>
#include "Exception.hh"
#include "Move.hh"
#include "GraphNode.hh"

// dummy classes until real implementations are written
class Scope{} ;
// Implementation in header file that includes this header file
class ProgramOperation;

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

    explicit MoveNode(TTAProgram::Move& newmove);
    explicit MoveNode(TTAProgram::Move* newmove);
    virtual ~MoveNode();

    MoveNode* copy();

    bool isSourceOperation() const;
    inline bool isDestinationOperation() const;
    bool isOperationMove() const;

    bool isSourceVariable() const;
    bool isSourceConstant() const;
    bool isSourceImmediateRegister() const;
    bool isDestinationVariable() const;
    bool isBypass() const;

    bool isRegisterMove() const;
    bool inSameOperation(const MoveNode& other) const;

    bool isPlaced() const;
    bool isAssigned() const;
    bool isScheduled() const;
    void setCycle(const int newcycle) throw(InvalidData);
    void unsetCycle() throw(InvalidData);
    int cycle() const throw(InvalidData);

    int earliestResultReadCycle() const;
    int guardLatency() const;

    Scope& scope();
    ProgramOperation& sourceOperation() const throw (InvalidData);
    ProgramOperation& destinationOperation() const throw (InvalidData);
    TTAProgram::Move& move();
    const TTAProgram::Move& move() const;

    void setDestinationOperation(ProgramOperation &po);
    void setSourceOperation(ProgramOperation &po);
    void setMoveOwned();

    void unsetDestinationOperation();
    void unsetSourceOperation();
    void unsetMoveOwned();

    bool isMoveOwned() const;
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
    const TTAProgram::Move* move_;

    ProgramOperation *srcOp_;
    ProgramOperation *dstOp_;

    /// Cycle in which the node is placed. Each cycle uniquely identifies an
    /// instruction slot within the current scheduling scope.
    int cycle_;

    /// True in case the Move is owned by the MoveNode.
    /// Ownership changes during scheduling when Move is placed into
    /// Instruction
    bool moveOwned_;

    /// True when the node placed (is given a cycle in program).
    bool placed_;
};

#include "MoveNode.icc"

#endif
