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
 * @file PostpassOperandSharer.hh
 *
 * Declaration of PostpassOperandSharer class.
 *
 * This optimizer removes operand writes which are redundant due
 * other operations using same value. 
 * This is executed after the main scheduler, so this mainly helps
 * to reduce power consumption, not to improve performance.
 *
 * @author Heikki Kultala 2013 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "BasicBlockPass.hh"
#include "ControlFlowGraphPass.hh"

namespace TTAProgram {
    class BasicBlocK;
    class Move;
    class InstructionReferenceManager;
}

class PostpassOperandSharer : 
    public BasicBlockPass, public ControlFlowGraphPass {

public:
    PostpassOperandSharer(
        InterPassData& ipd, 
        TTAProgram::InstructionReferenceManager& irm) : 
        BasicBlockPass(ipd), ControlFlowGraphPass(ipd), irm_(&irm) {}

    virtual ~PostpassOperandSharer();

    virtual void handleBasicBlock(
        TTAProgram::BasicBlock& basicBlock,
        const TTAMachine::Machine& targetMachine,
        TTAProgram::InstructionReferenceManager& irm,
        BasicBlockNode* bbn = NULL);

    bool tryRemoveOperandWrite(
        TTAProgram::BasicBlock& basicBlock, 
        int insIndex, 
        TTAProgram::Move& move);

    static void printStats();
    
    virtual std::string shortDescription() const {
        return "Post-pass that reduced redundant operand writes";
    }
private:
    TTAProgram::InstructionReferenceManager* irm_;
    static unsigned int moveCount_;
    static unsigned int operandCount_;
    static unsigned int removedOperands_;
    static unsigned int registerReads_;
    static unsigned int triggerCannotRemove_;
};
