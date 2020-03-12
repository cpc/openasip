/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @file BFShareOperand.cc
 *
 * Definition of BFShareOperand class.
 *
 * Performs an operands sharing between two (or more) operations
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFShareOperand.hh"
#include "MoveNode.hh"
#include "DataDependenceGraph.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "MoveNodeDuplicator.hh"

bool BFShareOperand::operator()() {
    if (removed_.move().destination().operationIndex()
        != shared_.move().destination().operationIndex()) {
        return false;
    }
    while (removed_.destinationOperationCount()) {
        ProgramOperationPtr op = removed_.destinationOperationPtr(0);
        removed_.removeDestinationOperation(&*op);
        op->removeInputNode(removed_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\t\tRemoved: " << removed_.toString()
                  << " from: " << op->toString() << std::endl;
#endif

        shared_.addDestinationOperationPtr(op);
        op->addInputNode(shared_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\t\tAdded: " << shared_.toString()
                  << " to: " << op->toString() << std::endl;
#endif

        // update also prolog movenodes.
        if (ii()) {
            MoveNode* prologMNOfRemoved = duplicator().getMoveNode(removed_);
            ProgramOperationPtr prologPO;
            if (prologMNOfRemoved) {
                prologPO = prologMNOfRemoved->destinationOperationPtr(0);
                prologMNOfRemoved->removeDestinationOperation(&*prologPO);
                prologPO->removeInputNode(*prologMNOfRemoved);
            } else {
                prologPO = duplicator().getProgramOperation(op);
            }

            MoveNode* prologMNOfShared = duplicator().getMoveNode(shared_);
            if (prologMNOfShared) {
                if (prologPO == nullptr) {
                    prologPO = duplicator().duplicateProgramOperationPtr(op);
                    createdPrologPOs_.insert(prologPO);
                }
                prologMNOfShared->addDestinationOperationPtr(prologPO);
                prologPO->addInputNode(*prologMNOfShared);
            }
        }
        ops_.push_back(op);
    }
    return true;
}

void
BFShareOperand::undoOnlyMe() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\t\tBFShareOperand undoing itseld, shared: "
              << shared_.toString() << std::endl;
#endif
    while(ops_.size()) {
        ProgramOperationPtr op = *ops_.rbegin();

        if (ii()) {
            ProgramOperationPtr prologPO =
                duplicator().getProgramOperation(op);

            MoveNode* prologMNOfShared = duplicator().getMoveNode(shared_);
            MoveNode* prologMNOfRemoved = duplicator().getMoveNode(removed_);

            if (prologMNOfShared) {
                // if the mn is duplicated, also the PO should be duplicated
                assert(prologPO != nullptr);
                prologPO->removeInputNode(*prologMNOfShared);
                prologMNOfShared->removeDestinationOperation(&*prologPO);
            }

            if (prologMNOfRemoved) {
                // if the mn is duplicated, also the PO should be duplicated
                assert(prologPO != nullptr);
                prologPO->addInputNode(*prologMNOfRemoved);
                prologMNOfRemoved->addDestinationOperationPtr(prologPO);
            }
        }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\t\t\tRestoring, op: "
                  << op->toString() << std::endl;
#endif
        op->removeInputNode(shared_);
        shared_.removeDestinationOperation(&*op);
        op->addInputNode(removed_);
        removed_.addDestinationOperationPtr(op);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\t\t\tRestored: " << removed_.toString()
                  << " to: " << op->toString() << std::endl;
#endif
        ops_.pop_back();
    }
}
