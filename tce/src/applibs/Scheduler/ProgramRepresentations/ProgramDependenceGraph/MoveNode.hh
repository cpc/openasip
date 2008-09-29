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
#include "TerminalRegister.hh"
#include "TerminalFUPort.hh"
#include "TerminalImmediate.hh"
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
    bool isDestinationOperation() const;
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
    bool isMove() const;

    /// Node can be entry node
    MoveNode();
private:
    /// Copying forbidden. Use copy() for a deep copy.
    MoveNode(const MoveNode&);
    /// Assignment forbidden
    MoveNode& operator=(const MoveNode&);

    /// True when the node placed (is given a cycle in program).
    bool placed_;

    /// Cycle in which the node is placed. Each cycle uniquely identifies an
    /// instruction slot within the current scheduling scope.
    int cycle_;

    /// Pointer to Move this node represents, Node itself do not change move
    const TTAProgram::Move* move_;

    /// True in case the Move is owned by the MoveNode.
    /// Ownership changes during scheduling when Move is placed into
    /// Instruction
    bool moveOwned_;

    ProgramOperation *srcOp_;
    ProgramOperation *dstOp_;
};

#endif
