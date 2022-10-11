/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file MoveNodeGroupBuilder.cc
 *
 * @author Pekka Jääskeläinen 2010-2011 (pjaaskel)
 */
#include <map>

#include "MoveNodeGroupBuilder.hh"
#include "ProgramOperation.hh"
#include "MoveNode.hh"
#include "BasicBlock.hh"
#include "MoveNodeGroup.hh"
#include "Machine.hh"
#include "FunctionUnit.hh"
#include "TCEString.hh"
#include "SpecialRegisterPort.hh"
#include "Terminal.hh"
#include "Instruction.hh"
#include "Move.hh"

//#define DEBUG_MNGBUILDER

/**
 * Creates movenodes and programoperations from the given BB.
 *
 * Also builds MoveNodes and ProgramOperations and ensures the produced
 * MNG list is in the input sequential order and that each MNG contains
 * either one MoveNode or a set of MoveNodes consisting a ProgramOperation.
 * The returned objects become owned by the caller.
 * This supports also input that is already bypassed. 
 */
MoveNodeGroupBuilder::MoveNodeGroupList*
MoveNodeGroupBuilder::build(TTAProgram::BasicBlock& bb) {

    /*
      Process each input move from the sequential unscheduled stream. 
      Input moves can be already bypassed and operand shared.

      In case the move
      a) is a reg/imm -> FU.in: 

         And if the 'in' is a non-triggering input move,
         add the move to a PO that has not been triggered yet in the FU. 

         If it's a trigger move, set the PO to "triggered" state. The previous
         PO in "triggered" state can be set to "finished" state as there cannot
         be no more bypassed reads from the previous one as the new PO
         is overwriting its results and the input sequential code is not cycle
         accurately scheduled.
      b) is an FU.out -> reg: 
         add it to an PO that has been triggered in the FU.
      c) is a FUA.out -> FUB.in: find the last PO started in FUA, add it there
         as an output move. Find the unstarted PO in FUB, add it there as
         an input move.
      d) reg/imm -> reg:
         just create the MNG

      Thus, there are two types of POs during the algorithm execution:
      1) untriggered: POs with operand moves still coming in, trigger not
         encountered.
      2) triggered: POs which are "on flight", thus the POs of which results
         can be still read to the upcoming POs.
      3) finished: these POs are just in the finished PO list

      There can be only one unstarted and only one started PO per FU in the
      sequential input, thus the indices are be per FU.

      The same input move can belong to multiple POs (operand sharing).
      How to treat those?

      At the moment an operation is moved to "started", check if all its
      input operand moves are found. If not, find the PO which had the
      original OperandMove (TODO: how?) and add them as inputs.

     */

    typedef std::map<TCEString, ProgramOperationPtr> 
        FUProgramOperationIndex;

    FUProgramOperationIndex untriggered, triggered;
    // programOperations_ is the finished set

    MoveNodeGroupList* mngs = new MoveNodeGroupList();
    MoveNodeGroup* mng = NULL;
    // PO which is being constructed.
    ProgramOperationPtr po;
    for (int i = 0; i < bb.instructionCount(); i++) {
        TTAProgram::Instruction& ins = bb.instructionAtIndex(i);
        for (int j = 0; j < ins.moveCount(); j++) {
            // handle one move
            auto movePtr = ins.movePtr(j);
            TTAProgram::Move& move = *movePtr;
            MoveNode* moveNode = new MoveNode(movePtr);
            TTAProgram::Terminal& source = move.source();
            TTAProgram::Terminal& dest = move.destination();
            
            if (mng == NULL)
                mng = new MoveNodeGroup();

            bool sourceIsFU = 
                (source.isFUPort() &&
                 !(dynamic_cast<const TTAMachine::SpecialRegisterPort*>(
                       &source.port())));

            bool destIsFU = (dest.isFUPort() &&
                !(dynamic_cast<const TTAMachine::SpecialRegisterPort*>(
                      &dest.port())));

#ifdef DEBUG_MNGBUILDER
            Application::logStream()
                << "processing move: " << move.toString() << " " 
                << sourceIsFU << " " << destIsFU;
            Application::logStream() << std::endl;
#endif
            if (!sourceIsFU && destIsFU) {
                po = untriggered[dest.functionUnit().name()];
                if (po == NULL) {
                    Operation* op;
                    if (dest.isOpcodeSetting()) {
                        op = &dest.operation();
                    } else {
                        op = &dest.hintOperation();
                    }
                    po = ProgramOperationPtr(new ProgramOperation(*op));
                    untriggered[dest.functionUnit().name()] = po;

                    if (mng->nodeCount() > 0) {
                        // start a new move node group as it has to be
                        // a new operation as no previous untriggered
                        // operation was found for this FU
                        mngs->push_back(mng);
                        mng = new MoveNodeGroup();
                    }
                    mng->setProgramOperationPtr(po);
                }
                po->addInputNode(*moveNode);
                moveNode->addDestinationOperationPtr(po);
                mng->addNode(*moveNode);
                if (dest.isTriggering()) {
                    untriggered[dest.functionUnit().name()] = ProgramOperationPtr();
                    triggered[dest.functionUnit().name()] = po;
                } 
            } else if (sourceIsFU && !destIsFU) {
                po = triggered[source.functionUnit().name()];
                assert(
                    po != NULL &&
                    "Encountered an FU read without a triggered operation.");

                po->addOutputNode(*moveNode);
                moveNode->setSourceOperationPtr(po);
                mng->addNode(*moveNode);
            } else if (sourceIsFU && destIsFU) {
                po = triggered[source.functionUnit().name()];
                assert(
                    po != NULL &&
                    "Encountered an FU read without a triggered operation.");
                po->addOutputNode(*moveNode);
                moveNode->setSourceOperationPtr(po);
                po = untriggered[dest.functionUnit().name()];
                if (po == NULL) { /* The 1st operand move? */
                    Operation* op;
                    if (dest.isOpcodeSetting()) {
                        op = &dest.operation();
                    } else {
                        op = &dest.hintOperation();
                    }
                    po = ProgramOperationPtr(new ProgramOperation(*op));
                    untriggered[dest.functionUnit().name()] = po;
                    // define the first operand move to start an operation 
                    // always, thus split MNG at this point
                    mngs->push_back(mng);
                    mng = new MoveNodeGroup();
                    mng->setProgramOperationPtr(po);
                }
                mng->addNode(*moveNode);
                po->addInputNode(*moveNode);
                moveNode->addDestinationOperationPtr(po);
                if (dest.isTriggering()) {
                    untriggered[dest.functionUnit().name()] = ProgramOperationPtr();
                    triggered[dest.functionUnit().name()] = po;
                }
            } else if (!sourceIsFU && !destIsFU) {

                if (mng->nodeCount() > 0)
                    mngs->push_back(mng);

                // reg copies are always scheduled independently
                mng = new MoveNodeGroup();
                mng->addNode(*moveNode);
                mngs->push_back(mng);

                mng = new MoveNodeGroup();
            } else {
                abortWithError(moveNode->toString() + " is not implemented.");
            }  
        }             
    }

    if (mng != NULL && mng->nodeCount() > 0) {
        mngs->push_back(mng);      
    }
    // untriggered POs are considered errors at this point as no
    // BB-crossing operations are supported in the sequential code
    for (FUProgramOperationIndex::const_iterator i = untriggered.begin();
         i != untriggered.end(); ++i) {
        ProgramOperationPtr po = (*i).second;
        if (po != NULL && po->inputMoveCount() > 0) {
            abortWithError(
                TCEString(
                    "Encountered an untriggered&unfinished "
                    "ProgramOperation\nat the end of the BB. "
                    "BB-crossing operations are not\nsupported "
                    "at this point." + po->toString()));
        }
    }

#ifdef DEBUG_MNGBUILDER
    int count = 0;
    Application::logStream() << "### ProgramOperations built:" << std::endl;
    for (ProgramOperationList::const_iterator i = programOperations_.begin();
         i != programOperations_.end(); ++i, ++count) {
        ProgramOperation* po = *i;
        Application::logStream() << count << " " <<po->toString() << std::endl;
    }

    Application::logStream() << "### MoveNodeGroups built:" << std::endl;
    count = 0;
    for (MoveNodeGroupList::const_iterator i = mngs->begin(); 
         i != mngs->end(); ++i, ++count) {
        MoveNodeGroup* mng = *i;
        Application::logStream() 
            << count << ": " << mng->toString() << std::endl;
    }

#endif
    return mngs;
}                
