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
 * @file ProgramOperation.hh
 *
 * Declaration of ProgramOperation class.
 *
 * @author Heikki Kultala 2006 (hkultala-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
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
