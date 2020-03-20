/*
    Copyright (c) 2002-2015 Tampere University.

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
 * This class is used for copying moves from a basic block to another
 * corresponding basic block, copying also the programoperation structure.
 */

#include "MoveNodeDuplicator.hh"

#include "BasicBlockNode.hh"
#include "AssocTools.hh"
#include "DataDependenceGraph.hh"
#include "ProgramAnnotation.hh"
#include "HWOperation.hh"
#include "Operation.hh"
#include "UniversalMachine.hh"
#include "Terminal.hh"
#include "MoveGuard.hh"
#include "UniversalFunctionUnit.hh"
#include "Move.hh"
#include "TerminalFUPort.hh"
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"


MoveNodeDuplicator::MoveNodeDuplicator(
    DataDependenceGraph& oldDDG, DataDependenceGraph& newDDG) :
    oldDDG_(&oldDDG), newDDG_(&newDDG),
    bigDDG_(static_cast<DataDependenceGraph*>(oldDDG.rootGraph())) { }

void MoveNodeDuplicator::disposeMoveNode(MoveNode* newMN) {
    if (newMN == NULL) {
        return;
    }
    if (newMN->isScheduled()) {
        std::cerr << "\t\t\t\t\tCannot dispose scheduled mn: "
                  << newMN->toString() << std::endl;
        assert(false);
    }
    assert(!newMN->isScheduled());
    std::map<MoveNode*,MoveNode*,MoveNode::Comparator>::iterator i =
        oldMoveNodes_.find(newMN);
    // already disposed?
    if (i == oldMoveNodes_.end()) {
        return;
    }
    MoveNode* old = i->second;
    moveNodes_.erase(old);
    oldMoveNodes_.erase(newMN);
    TTAProgram::Move* oldMove = &old->move();
    moves_.erase(oldMove);
    if (newDDG_->hasNode(*newMN)) {
        newDDG_->removeNode(*newMN);
    }
    delete newMN;
}

/*
 * Gets already duplicated move, but does not create a new one.
 * If none found, returns null
 */
MoveNode*
MoveNodeDuplicator::getMoveNode(MoveNode& old) {
    std::map<MoveNode*,MoveNode*,MoveNode::Comparator>::iterator iter =
        moveNodes_.find(&old);
if (iter != moveNodes_.end()) {
        return iter->second;
    }
    return NULL;
}


/**
 *
 * Duplicates a corresponding MoveNode a given move in the next BB.
 *
 * If no corresponding MoveNode created, creates one
 *
 * @param old ProgramOperation in jump target BB.
 * @return new MoveNode for this BB.
 */

std::pair<MoveNode*, bool>
MoveNodeDuplicator::duplicateMoveNode(
    MoveNode& old, bool addToDDG, bool ignoreSameBBBackEdges) {
    std::map<MoveNode*,MoveNode*,MoveNode::Comparator>::iterator iter =
        moveNodes_.find(&old);

    if (iter != moveNodes_.end()) {
        MoveNode* node = iter->second;
        if (addToDDG) {
            if (!newDDG_->hasNode(*node)) {
                newDDG_->addNode(*node, *bbn_);
                bigDDG_->copyDependencies(old,*node, ignoreSameBBBackEdges, false);
            }
        }
    }
    if (AssocTools::containsKey(moveNodes_,&old)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\t\told mn found: "
                  << moveNodes_[&old]->toString() << std::endl;
#endif
        return std::make_pair(moveNodes_[&old], false);
    } else {
        auto movePtr = duplicateMove(old.move());
        MoveNode *newMN = new MoveNode(movePtr);

        // TODO: only add to ddg if really scheduling, not for testing?
        if (addToDDG) {
            newDDG_->addNode(*newMN, *bbn_);
            bigDDG_->copyDependencies(old,*newMN, ignoreSameBBBackEdges, false);
        }

        moveNodes_[&old] = newMN;
        oldMoveNodes_[newMN] = &old;
        if (old.isSourceOperation()) {
            newMN->setSourceOperationPtr(
                duplicateProgramOperationPtr(old.sourceOperationPtr()));
            newMN->sourceOperation().addOutputNode(*newMN);
            assert(newMN->isSourceOperation());
        }
        if (old.isDestinationOperation()) {
            for (unsigned int i = 0;
                 i < old.destinationOperationCount(); i++) {
                newMN->addDestinationOperationPtr(
                    duplicateProgramOperationPtr(
                        old.destinationOperationPtr(i)));
                newMN->destinationOperation(i).addInputNode(*newMN);
            }
            assert(newMN->isDestinationOperation());
        }
        return std::make_pair(newMN, true);
    }
}

/**
 * Gets a corresponding ProgramOperation to a given move in the next BB.
 *
 * If no corresponding ProgramOperation created, creates one
 *
 * @param old ProgramOperation in jump target BB.
 * @return new ProgramOperation for this BB.
 */
ProgramOperationPtr
MoveNodeDuplicator::duplicateProgramOperationPtr(
    ProgramOperationPtr old) { // , BasicBlockNode& bbn) {
    if (AssocTools::containsKey(programOperations_, old.get())) {
        return programOperations_[old.get()];
    } else {
        ProgramOperationPtr po =
            ProgramOperationPtr(
                new ProgramOperation(old->operation()));
        programOperations_[old.get()] = po;
        oldProgramOperations_[po.get()] = old;
        return po;
    }
}

ProgramOperationPtr
MoveNodeDuplicator::getProgramOperation(ProgramOperationPtr old) {
    auto i = programOperations_.find(old.get());
    if (i != programOperations_.end()) {
        return i->second;
    } else {
        return ProgramOperationPtr(nullptr);
    }
}


std::shared_ptr<TTAProgram::Move>
MoveNodeDuplicator::duplicateMove(TTAProgram::Move& old) {
    if (AssocTools::containsKey(moves_,&old)) {
        return moves_[&old];
    } else {
        MoveNode& oldMN = oldDDG_->nodeOfMove(old);
        std::shared_ptr<TTAProgram::Move> newMove(old.copy());
        newMove->setBus(UniversalMachine::instance().universalBus());

        TTAProgram::Terminal& source = newMove->source();
        if (oldMN.isSourceOperation()) {
            if (oldMN.isScheduled()) {
                assert(source.isFUPort());
                std::string fuName = source.functionUnit().name();
                //TODO: which is the correct annotation here?
                TTAProgram::ProgramAnnotation srcUnit(
                    TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_SRC,
                    fuName);
                newMove->setAnnotation(srcUnit);
                const Operation &srcOp = oldMN.sourceOperation().operation();
                const TTAMachine::HWOperation& hwop =
                    *UniversalMachine::instance().universalFunctionUnit().
                    operation(srcOp.name());
                newMove->setSource(new TTAProgram::TerminalFUPort(
                                       hwop, old.source().operationIndex()));
            }
        } else {
            if (source.isRA()) {
                newMove->setSource(
                    new TTAProgram::TerminalFUPort(
                        *UniversalMachine::instance().controlUnit()->
                        returnAddressPort()));
            }
        }

        TTAProgram::Terminal& dest = newMove->destination();
        if (oldMN.isDestinationOperation()) {
            assert(dest.isFUPort());

            if (oldMN.isScheduled()) {
                std::string fuName = dest.functionUnit().name();
                //TODO: which is the correct annotation here?
                TTAProgram::ProgramAnnotation dstUnit(
                    TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_DST,
                    fuName);
                newMove->setAnnotation(dstUnit);
                const Operation &dstOp =
                    oldMN.destinationOperation().operation();
                TTAMachine::HWOperation& hwop =
                    *UniversalMachine::instance().universalFunctionUnit().
                    operation(dstOp.name());
                newMove->setDestination(new TTAProgram::TerminalFUPort(
                                            hwop, old.destination().
                                            operationIndex()));
            } else {
                if (dest.isRA()) {
                    newMove->setDestination(
                        new TTAProgram::TerminalFUPort(
                            *UniversalMachine::instance().controlUnit()->
                            returnAddressPort()));
                }
            }
        }

        moves_[&old] = newMove;

        // set guard of new move to be same as old move.
        if (!old.isUnconditional()) {
            TTAProgram::MoveGuard* g = old.guard().copy();
            newMove->setGuard(g);
        }
        return newMove;
    }
}

void MoveNodeDuplicator::dumpDDG() {
    newDDG_->writeToDotFile("prolog_ddg.dot");
}
