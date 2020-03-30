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
 * @file PostpassOperandSharer.cc
 *
 * Implementation of PostpassOperandSharer class.
 *
 * This optimizer removes operand writes which are redundant due
 * other operations using same value. 
 * This is executed after the main scheduler, so this mainly helps
 * to reduce power consumption, not to improve performance.
 *
 * @author Heikki Kultala 2009 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "PostpassOperandSharer.hh"

#include "Move.hh"
#include "Instruction.hh"
#include "Terminal.hh"
#include "BasicBlock.hh"
#include "Guard.hh"
#include "MoveGuard.hh"
#include "RegisterFile.hh"
#include "InstructionReferenceManager.hh"

using TTAProgram::Move;
using TTAProgram::Instruction;
using TTAProgram::Terminal;
using TTAMachine::Guard;
using TTAMachine::RegisterGuard;
using TTAMachine::RegisterFile;

// this makes the code run slower but gives the trigger operand
// statistics
#define GET_BETTER_STATISTICS

void
PostpassOperandSharer::handleBasicBlock(
    TTAProgram::BasicBlock& basicBlock, const TTAMachine::Machine&,
    TTAProgram::InstructionReferenceManager&, BasicBlockNode*) {
    for (int i = basicBlock.instructionCount()-1; i >= 0; i--) {
        Instruction& ins = basicBlock.instructionAtIndex(i);
        for (int j = 0; j < ins.moveCount(); j++) {
            Move& move = ins.move(j);
            moveCount_++;
            if (tryRemoveOperandWrite(basicBlock, i, move)) {
                ins.removeMove(move);
                // TODO: make sure there is no movenode to this.
//                delete &move;
                j--;
            }
        }
    }
}

/**
 * 
 */
bool PostpassOperandSharer::tryRemoveOperandWrite(
    TTAProgram::BasicBlock& basicBlock, int insIndex, TTAProgram::Move& move) {

    bool guardOK = false;
    Terminal& dest = move.destination();
    Terminal& src = move.source();

    if (src.isGPR()) {
        registerReads_++;
    }

#ifdef GET_BETTER_STATISTICS
    if (!dest.isFUPort())
    {
        return false;
    }
#else

    if (!dest.isFUPort() ||
        dest.isTriggering() ||
        dest.isOpcodeSetting()) {
        return false;
    }
#endif

    operandCount_++;
    if (src.isFUPort()) {
        return false;
    }

    if (src.isImmediateRegister()) {
        return false;
    }

    TTAMachine::RegisterFile* guardRF = NULL;
    int guardIndex = -1;
    if (!move.isUnconditional()) {
        const TTAMachine::Guard& guard = move.guard().guard();
        const RegisterGuard* rg = dynamic_cast<const TTAMachine::RegisterGuard*>(&guard);
        if (rg) {
            guardRF = rg->registerFile();
            guardIndex = rg->registerIndex();
        }
    }

    int srcIndex = -1;
    const TTAMachine::RegisterFile* srcRF = NULL;
    if (src.isGPR()) {
        srcIndex = src.index();
        srcRF = &src.registerFile();
    }

    int glat = move.guardLatency();
    bool lastDueJump = false;

    for (int i = insIndex -1; i >= basicBlock.skippedFirstInstructions() && !lastDueJump; i--) {

        // first check guard writes..
        // if guard has longer latency than 1 , this has to go to previous
        // cycles.
        int k = i+1 - glat;
        if (k>= 0) {
            Instruction& gIns = basicBlock.instructionAtIndex(k);
            
            // check that does not overwrite the source
            for (int j = 0; j < gIns.moveCount(); j++) {
                Move& gMove = gIns.move(j);
                Terminal& gDst = gMove.destination();
                if (gDst.isGPR()) {
                    int pdIndex = gDst.index();
                    const RegisterFile* pdRF = &gDst.registerFile();
                    if (guardIndex == pdIndex && guardRF == pdRF) {
                        guardOK = false;
                    }
                }
            }
        }

        Instruction& prevIns = basicBlock.instructionAtIndex(i);
        if (irm_->hasReference(prevIns)) {
            lastDueJump = true;
        }
        // check that does not overwrite the source
        for (int j = 0; j < prevIns.moveCount(); j++) {
            Move& prevMove = prevIns.move(j);
            Terminal& prevDst = prevMove.destination();

            if (prevDst.isGPR()) {
                int pdIndex = prevDst.index();
                const RegisterFile* pdRF = &prevDst.registerFile();
                if ((srcIndex == pdIndex && srcRF == pdRF) &&
                    (!guardOK || prevMove.isUnconditional() ||
                     move.isUnconditional() || 
                     !prevMove.guard().guard().isOpposite(
                         move.guard().guard()))) {
                    return false;
                }
            }
        }

        // check if writes the same operand
        for (int j = 0; j < prevIns.moveCount(); j++) {
            Move& prevMove = prevIns.move(j);
            Terminal& prevDest = prevMove.destination();

            // write to same port..
            if (&prevDest.port() == &dest.port()) {
                if (move.source().equals(prevMove.source())) {

                    // TODO: what if guard is conditional, ok or not?
                    if (!prevMove.isUnconditional() &&
                        (move.isUnconditional() || 
                         (!guardOK || !prevMove.guard().guard().isEqual(
                              move.guard().guard())))) {
                        continue;
                    }
                    
                    if (prevDest.isTriggering() ||
                        prevDest.isOpcodeSetting()) {
                        triggerCannotRemove_++;
                        return false;
                    }
                    
                    removedOperands_++;
                    return true;
                } else {
                    return false;
                }
            }
        }
    }
    return false;
}

PostpassOperandSharer::~PostpassOperandSharer() {}

void PostpassOperandSharer::printStats() {
    std::cerr << "PostpassOperandSharer statistics: " << std::endl 
              << "\tTotal moves: " << moveCount_ << std::endl
              << "\tTotal reg reads: " << registerReads_ << std::endl
              << "\tTotal operands: "<<operandCount_ 
              << std::endl
              << "\tRemoved operands: " << removedOperands_ << std::endl
              << "\tCannot share trigger count: " << triggerCannotRemove_ 
              << std::endl;
    
}

unsigned int PostpassOperandSharer::moveCount_ = 0;
unsigned int PostpassOperandSharer::operandCount_ = 0;
unsigned int PostpassOperandSharer::removedOperands_ = 0;
unsigned int PostpassOperandSharer::registerReads_ = 0;
unsigned int PostpassOperandSharer::triggerCannotRemove_ = 0;
