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
#include <vector>
#include <boost/shared_ptr.hpp>

#include "Exception.hh"

class MoveNode;
class MoveNodeSet;
class Operation;
class ProgramOperation;

namespace llvm {
    class MachineInstr;
}
namespace TTAProgram {
    class Move;
}

namespace TTAMachine {
    class Unit;
    class FunctionUnit;
    class HWOperation;
}

// use this smart_ptr type to point to POs to allow more safe sharing of
// POs between POM and DDG, etc.
typedef boost::shared_ptr<ProgramOperation> ProgramOperationPtr;


/**
 * Represents a single execution of an operation in a program.
 */
class ProgramOperation {
public:
    ProgramOperation(
        const Operation &operation, 
        const llvm::MachineInstr* instr=NULL);
    ProgramOperation();
    ~ProgramOperation();

    void addNode(MoveNode& node);
    void addInputNode(MoveNode& node);
    void addOutputNode(MoveNode& node);

    const TTAMachine::FunctionUnit* scheduledFU() const;

    const TTAMachine::HWOperation* hwopFromOutMove(
        const MoveNode& outputNode) const;

    void removeOutputNode(MoveNode& node);
    void removeInputNode(MoveNode& node);

    bool isComplete();
    bool isReady();
    bool isMultinode();
    bool isAssigned();
    bool isAnyNodeAssigned();
    bool isAnyInputAssigned();
    bool isAnyOutputAssigned();    
    bool areInputsAssigned();
    bool areOutputsAssigned();

    MoveNode& opcodeSettingNode();
    MoveNodeSet& inputNode(int in) const;
    MoveNodeSet& outputNode(int out) const;

    bool hasOutputNode(int out) const;
    bool hasInputNode(int in) const;

    const Operation& operation() const;

    int inputMoveCount() const;
    int outputMoveCount() const;

    bool hasMoveNodeForMove(const TTAProgram::Move& move) const;
    MoveNode& moveNode(const TTAProgram::Move& move) const;
    MoveNode& inputMove(int index) const;
    MoveNode& outputMove(int index) const;

    MoveNode* triggeringMove() const;
    
    MoveNode* findTriggerFromUnit(const TTAMachine::Unit& unit) const;
    bool isLegalFU(const TTAMachine::FunctionUnit& fu) const;

    const llvm::MachineInstr* machineInstr() const { return mInstr_; }

    void switchInputs(int idx1 = 1, int idx2 = 2);

    std::string toString() const;

    unsigned int poId() const;

    bool hasConstantOperand() const;
    // Comparator for maps and sets
    class Comparator {
    public:
        bool operator()(
            const ProgramOperation* po1, const ProgramOperation* po2) const;
        bool operator()(
            const ProgramOperationPtr &po1, const ProgramOperationPtr &po2) const;

    };

private:
    typedef std::vector<MoveNode*> MoveVector;
    // copying forbidden
    ProgramOperation(const ProgramOperation&);
    // assignment forbidden
    ProgramOperation& operator=(const ProgramOperation&);
    // OSAL Operation this program operation is executing.
    const Operation& operation_;
    // map from operand index to MoveNodeSet
    std::map<int,MoveNodeSet*> inputMoves_;
    // map from operand index to MoveNodeSet
    std::map<int,MoveNodeSet*> outputMoves_;
    // all input moves
    MoveVector allInputMoves_;
    // all output moves
    MoveVector allOutputMoves_;
    unsigned int poId_;
    static unsigned int idCounter;
    // Reference to original LLVM MachineInstruction
    const llvm::MachineInstr* mInstr_;
};

class ProgramOperationPtrComparator {
public:
    bool operator() (
        const ProgramOperationPtr po1, const ProgramOperationPtr po2) const {
        return ProgramOperation::Comparator()(po1.get(), po2.get());
    }
};

#endif
