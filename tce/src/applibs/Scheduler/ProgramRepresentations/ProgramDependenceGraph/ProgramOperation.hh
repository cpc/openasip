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
 * @file ProgramOperation.hh
 *
 * Declaration of ProgramOperation class.
 *
 * @author Heikki Kultala 2006 (hkultala@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TCE_PROGRAM_OPERATION_HH
#define TCE_PROGRAM_OPERATION_HH

#include <string>
#include <map>
#include "Exception.hh"
#include "Operation.hh"
#include "MoveNode.hh"
#include "MoveNodeSet.hh"


/**
 * Represents a single execution of an operation in a program.
 */
class ProgramOperation {
public:
    ProgramOperation(Operation &operation);
    ProgramOperation();
    ~ProgramOperation();

    void addNode(MoveNode& node) throw (IllegalParameters);
    void addInputNode(MoveNode& node) throw (IllegalParameters);
    void addOutputNode(MoveNode& node) throw (IllegalParameters);

    void removeOutputNode(MoveNode& node) throw (IllegalRegistration);
    void removeInputNode(MoveNode& node) throw (IllegalRegistration);

    bool isComplete();
    bool isReady();
    bool isMultinode();
    bool isAssigned();
    bool areInputsAssigned();

    MoveNode& opcodeSettingNode() throw (InvalidData);
    MoveNodeSet& inputNode(int in) throw (OutOfRange,KeyNotFound);
    MoveNodeSet& outputNode(int out) throw (OutOfRange,KeyNotFound);

    bool hasOutputNode(int out) const;
    bool hasInputNode(int in) const;

    Operation& operation() const;

    int inputMoveCount() const;
    int outputMoveCount() const;

    MoveNode& inputMove(int index) const;
    MoveNode& outputMove(int index) const;

    MoveNode& triggeringMove() const throw (InvalidData);

    std::string toString() const;

    unsigned int poId() const;

    // Comparator for maps and sets
    class Comparator {
    public:
        bool operator()(
            const ProgramOperation* po1, const ProgramOperation* po2) const;
    };
private:
    // copying forbidden
    ProgramOperation(const ProgramOperation&);
    // assignment forbidden
    ProgramOperation& operator=(const ProgramOperation&);
    // OSAL Operation this program operation is executing.
    Operation& operation_;
    // map from operand index to MoveNodeSet
    std::map<int,MoveNodeSet*> inputMoves_;
    // map from operand index to MoveNodeSet
    std::map<int,MoveNodeSet*> outputMoves_;
    // all input moves
    std::vector<MoveNode*> allInputMoves_;
    // all output moves
    std::vector<MoveNode*> allOutputMoves_;
    unsigned int poId_;
    static unsigned int idCounter;
};

#endif
