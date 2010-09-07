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
 * @file SequentialFirstFitResourceAllocator.cc
 *
 * Implementation of SequentialFirstFitResourceAllocator class.
 *
 * @author Ari Mets�halme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

#include <vector>
#include <string>

#include "SequentialFirstFitResourceAllocator.hh"
#include "CodeSnippet.hh"
#include "Program.hh"
#include "NullProcedure.hh"
#include "NullInstruction.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "TerminalFUPort.hh"
#include "TerminalRegister.hh"
#include "TerminalAddress.hh"
#include "TerminalInstructionAddress.hh"
#include "ControlUnit.hh"
#include "BaseFUPort.hh"
#include "SpecialRegisterPort.hh"
#include "Operation.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "MathTools.hh"
#include "AssocTools.hh"
#include "Segment.hh"
#include "Guard.hh"
#include "MoveGuard.hh"
#include "MoveNode.hh"
#include "ProgramOperation.hh"
#include "POMDisassembler.hh"
#include "TCEString.hh"
#include "InstructionReferenceManager.hh"
                 
using std::vector;
using std::list;
using std::string;
using namespace TTAMachine;
using namespace TTAProgram;


/////////////////////////////////////////////////////////////////////////////
// SequentialFirstFitResourceAllocator
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
SequentialFirstFitResourceAllocator::SequentialFirstFitResourceAllocator():
    resMan_(NULL) {
}


/**
 * Destructor.
 */
SequentialFirstFitResourceAllocator::~SequentialFirstFitResourceAllocator() {
    delete resMan_;
}

void
SequentialFirstFitResourceAllocator::start()
    throw (Exception) {
    if (program_ == NULL || target_ == NULL) {
        string method = "SequentialFirstFitResourceAllocator::start()";
        string msg =
            "Source program and/or target architecture not defined!";
        throw ObjectNotInitialized(__FILE__, __LINE__, method, msg);
    }
    resMan_ = new SimpleResourceManager(*target_);
    assignResources();
}

bool
SequentialFirstFitResourceAllocator::needsTarget() const {
    return true;
}

bool
SequentialFirstFitResourceAllocator::needsProgram() const {
    return true;
}

/**
 * Assigns resources on the given program.
 *
 * @param prog
 * @param mach
 */
void
SequentialFirstFitResourceAllocator::assignResources() {
    try {
        MoveNodeList moveNodes = createMoveNodes();
        MoveNodeList::iterator nodeIter = moveNodes.begin();
        int cycle = 0;
        MoveNode* pendingTrigger = NULL;
        ProgramOperation* destPo = NULL;
        bool needRefFixFromTrigger = false;
        int remainingOperands = 0;

        while (nodeIter != moveNodes.end()) {
            
            MoveNode* node = *nodeIter;
            bool advanceToNext = true;
            string disAsmMove = POMDisassembler::disassemble(node->move());
            Instruction& currentIns = node->move().parent();
            CodeSnippet& parent = currentIns.parent();
            InstructionReferenceManager& refMan =
                program_->instructionReferenceManager();
            
            // know what PO we are scheduling
            if( node->isDestinationOperation()) { //operand move
                if ( destPo == NULL) {
                    destPo = &(node->destinationOperation());
                    remainingOperands = destPo->operation().numberOfInputs()-1;
                } else {
                    // another operation?
                    if ( &(node->destinationOperation()) != destPo) {
                        if (pendingTrigger != NULL) {
                            node = pendingTrigger;
                            advanceToNext = false;
                            pendingTrigger = NULL;
                        } else {
                            destPo = &(node->destinationOperation());
                            remainingOperands = 
                                destPo->operation().numberOfInputs()-1;
                        } 
                    }
                    else {
                        assert(remainingOperands > 0 && "Illegal operand move");
                        remainingOperands--;
                    }
                }
            } else {
                // first after operan moves , time for trigger
                if (destPo != NULL) {
                    if (pendingTrigger != NULL) {
                        node = pendingTrigger;
                        advanceToNext = false;
                    }
                    destPo = NULL;
                }
                assert(remainingOperands == 0 && "Missing operand move");
            }
            
            
            // test if assignment is impossible because move in node
            // is guarded and there is no correct guard in any bus
            if (!node->move().isUnconditional() && !resMan_->hasGuard(*node)) {
                string msg =
                    "Unable to assign resources for '";
                msg += disAsmMove + "' in cycle ";
                msg += Conversion::toString(cycle);
                msg += "! Machine does not have suitable guard!";
                throw ModuleRunTimeError(
                    __FILE__, __LINE__, __func__, msg);
            }
            if (resMan_->canAssign(cycle, *node)) {
                resMan_->assign(cycle, *node);
                Terminal& dest = node->move().destination();
                if (remainingOperands && dest.isFUPort() && 
                    dest.isTriggering()) {
                    // need to unschedule and delay the trigger
                    if (destPo != NULL) {
                        pendingTrigger = node;
                        if (refMan.hasReference(currentIns)) {
                            needRefFixFromTrigger = true;
                        }
                        parent.remove(currentIns);
                        resMan_->unassign(*node);
                        ++nodeIter;
                        continue;                        
                    } 
                } else {
                    if ( pendingTrigger != NULL && dest.isFUPort() && 
                         dest.isTriggering()) {
                        
                        // we just scheduled a delayed trigger
                        assert(pendingTrigger == node);
                        pendingTrigger = NULL;
                        Instruction* newIns = 
                            resMan_->instruction(cycle)->copy();
                        parent.insertBefore(
                            (*nodeIter)->move().parent(), newIns);
                    }
                    else {
                        // we scheduled something ordinary
                        Instruction* newIns = resMan_->instruction(cycle)->copy();
                        
                        parent.insertBefore(
                            currentIns, newIns);
                        if (refMan.hasReference(currentIns)) {
                            refMan.replace(currentIns, *newIns);
                        }
                        parent.remove(currentIns);
                        
                        // fix references to trigger to point to first
                        if (needRefFixFromTrigger) {
                            refMan.replace(pendingTrigger->move().parent(),
                                           *newIns);
                            needRefFixFromTrigger = false;
                        }
                    }
                }
            } else {
                
                // test if assignment is impossible because immediate in node
                // is too big
#if 0
                if (node->move().source().isImmediate()) {
                    
                    int testedCycle = cycle;
                    int maxCycle = cycle;
                    bool noSpaceFound = true;

                    while (noSpaceFound) {
                        // try to find a template to define long immediate
                        if (resMan_->canAssignLongImmediate(cycle, *node)) {
                            resMan_->assignLongImmediate(cycle, *node);
                            // reassign rest of the resources
                            if (resMan_->canAssign(cycle,*node)) {
                                resMan_->assign(cycle, *node);
                                noSpaceFound = false;

                                Instruction* newIns =
                                    resMan_->instruction(cycle)->copy();
                                Instruction& oldIns =
                                    parent.parent().instructionAt(
                                        testedCycle);
                                parent.insertBefore(currentIns, newIns);
                                Instruction* defIns =
                                    resMan_->instruction(testedCycle)->copy();
                                parent.insertBefore(oldIns, defIns);

                                if (refMan.hasReference(currentIns)) {
                                    refMan.replace(currentIns, *newIns);
                                }
                                parent.remove(
                                    currentIns);
                                if (refMan.hasReference(oldIns)) {
                                    refMan.replace(oldIns, *defIns);
                                }
                                parent.remove(oldIns);

                                break;
                            } else {
                                MoveNodeSet tempSet;
                                tempSet.addMoveNode(*node);
                                if (!resMan_->hasConnection(tempSet)) {
                                    string msg =
                                        "Unable to assign resources for '" ;
                                    msg += disAsmMove + "' in cycle ";
                                    msg += Conversion::toString(cycle);
                                    msg += "! Machine does not have";
                                    msg += "suitable connection!";
                                    throw ModuleRunTimeError(
                                        __FILE__, __LINE__, __func__, msg);
                                }
                                resMan_->unassignLongImmediate(*node);
                            }

                        } else {
                            // Increase use cycle because of immediate unit
                            // latency, change references in case the
                            // move was target of a jump
                            Instruction* newIns = new Instruction;
                            parent.insertBefore(currentIns, newIns);
                            if (refMan.hasReference(currentIns)) {
                                refMan.replace(currentIns, *newIns);
                            }

                            cycle++;
                            if ((cycle - maxCycle) > 10) {
                                string msg =
                                    "Unable to assign resources for '";
                                msg += disAsmMove + "' in cycle ";
                                msg += Conversion::toString(cycle);
                                msg += "! Long immediate assignment not";
                                msg += " possible!";
                                throw ModuleRunTimeError(
                                    __FILE__, __LINE__, __func__, msg);
                            }
                        }
                    }

                } else {
#endif                    
                // test if assignment is impossible because move in node
                // needs connection that does not exist
                    MoveNodeSet tempSet;
                    tempSet.addMoveNode(*node);
                    if (!resMan_->hasConnection(tempSet)) {
                        string msg =
                            "Unable to assign resources for '" ;
                        msg += disAsmMove + "' in cycle ";
                        msg += Conversion::toString(cycle);
                        msg += " Machine does not have suitable connection!";
                        throw ModuleRunTimeError(
                            __FILE__, __LINE__, __func__, msg);
                    } else {
                        string msg =
                            "Unable to assign resources for '" + disAsmMove;
                        msg += "' in cycle ";
                        msg += Conversion::toString(cycle);
                        throw ModuleRunTimeError(
                            __FILE__, __LINE__, __func__, msg);
                    }
//                }
            }
            cycle++;
            
            // insert nops to fill latency slots
            
            if (node->move().destination().isFUPort() &&
                dynamic_cast<const BaseFUPort&>(
                    node->move().destination().port()).isTriggering()) {
                
                std::cout << "inserting nops";
                Operation* currentOp =
                    &node->move().destination().operation();
                int numNops = 0;
                
                // todo add isControlFlowOperation() to Operation
                if (currentOp->name() == "JUMP" ||
                    currentOp->name() == "CALL") {
                    numNops = target_->controlUnit()->delaySlots();
                } else {
                    const FunctionUnit& destinationFU =
                        node->move().destination().functionUnit();
                    HWOperation* hwOp =
                        destinationFU.operation(currentOp->name());
                    numNops = hwOp->latency() - 1;
                }
                
                // this breaks if input has NOPs as this is incorrectly aligned
                Instruction& ins = program_->instructionAt(node->cycle());
                for (int i = 0; i < numNops; i++) {
                    std::cout << ".";
                    parent.insertAfter(
                        ins, new Instruction());
                }
                cycle += numNops;
                std::cout << std::endl;
            }
            if (advanceToNext) {
                ++nodeIter;
            }
        }
    } catch (const Exception& e) {
        throw ModuleRunTimeError(
            e.fileName(), e.lineNum(), e.procedureName(), e.errorMessage());
    }
}

SequentialFirstFitResourceAllocator::MoveNodeList
SequentialFirstFitResourceAllocator::createMoveNodes() {

    Procedure* proc = &program_->firstProcedure();
    MoveNodeList moveNodes;
    while (proc != &NullProcedure::instance()) {
        Instruction* ins = &proc->firstInstruction();

        typedef std::list<ProgramOperation*> POList;
        typedef POList::iterator POLIter;

        POList destPending; // operations lacking operands
        POList readPending;  // operations lacking result read

        while (ins != & NullInstruction::instance()) {

            // Move count should be always 1 so this loop should not be needed
            // for Universalmachine/unscheduled code
            for(int i = 0; i < ins->moveCount(); i++ ) {
                Move &move = ins->move(i);

                MoveNode* moveNode = new MoveNode(move);
                Terminal& source = move.source();
                Terminal& dest = move.destination();
                if (source.isFUPort() &&
                    !(dynamic_cast<const SpecialRegisterPort*>(
                          &source.port()))) {
                    Operation *sop = NULL;
                    TerminalFUPort& tfps=dynamic_cast<TerminalFUPort&>(source);
                    sop = &tfps.hintOperation();

                    for (POLIter poli = readPending.begin();
                         poli != readPending.end(); poli++ ) {
                        ProgramOperation* po = *poli;
                        if( sop == &po->operation()) {
                            po->addOutputNode( *moveNode );
                            moveNode->setSourceOperation(*po);

                            if(po->isComplete())   {
                                readPending.erase(poli);
//                                cout << "program operation complete" << endl;
                            } else {
//                                cout << "not complete,"
//                                     << "lacking some output read" << endl;
//                                cout << "output count: " << sop->numberOfOutputs()
//                                     << endl;
                            }

                            goto srcHandled;
                        }
                    }
//                    cerr << "ERROR:result move without operands" << endl;
                }
            srcHandled:
                if( dest.isFUPort() &&
                    !(dynamic_cast<const SpecialRegisterPort*>(&dest.port()))) {
//                if( moveNode->isDestinationOperation()) {
                    Operation *dop = NULL;
                    TerminalFUPort& tfpd=dynamic_cast<TerminalFUPort&>(dest);
                    if( tfpd.isOpcodeSetting()) {
                        dop = &tfpd.operation();
                        for( POLIter poli = destPending.begin();
                             poli != destPending.end();
                             poli++ ) {
                            ProgramOperation* po = *poli;

                            if( dop == &po->operation()) {
                                po->addInputNode(*moveNode);
                                moveNode->setDestinationOperation( *po );
                                if( po->isReady()) {
//                                    cout << "po ready" << endl;
                                    destPending.erase(poli);
                                    if( dop->numberOfOutputs()) {
                                        readPending.push_back(po);
                                    }
                                } else {
//                                    cerr << "error: trigger too early" << endl;
                                }
                                goto dstHandled;
                            }
                        }
                        // only one triggering input?
                        if (dop->numberOfInputs() == 1 ) {
                            ProgramOperation *po = new ProgramOperation(*dop);
                            moveNode->setDestinationOperation( *po );
                            po->addInputNode(*moveNode);
                            if (dop->numberOfOutputs()) {
                                readPending.push_back(po);
                            }
                        } else { // trigger came too early
//                            cout << "ERROR: inconsistent input code:"
//                                 << "trigger without operand" << endl;
                        }


                    } else {
                        dop = &tfpd.hintOperation();
                        for( POLIter poli = destPending.begin();
                             poli != destPending.end();
                             poli++ ) {
                            ProgramOperation& po = **poli;

                            if( dop == &po.operation()) {
                                // add current move to operation

                                // check that no other move has same input num
                                // not be needed for universalmachine code?

                                // does not yet exist

                                // TBD: check for inconsistent input code
                                po.addInputNode(*moveNode);
                                moveNode->setDestinationOperation( po );
                                goto dstHandled;
                            }
                        }
                        // create new ProgramOperation
                        ProgramOperation *po = new ProgramOperation(*dop);
                        moveNode->setDestinationOperation( *po );
                        po->addInputNode(*moveNode);
                        destPending.push_back(po);
                    }
                }
            dstHandled:
                moveNodes.push_back( moveNode );
            }
            ins = &proc->nextInstruction(*ins);
        }

        if (destPending.size() > 0 || readPending.size() > 0) {

            std::cerr
                << "ERROR: unready program operations's at procedure "
                << proc->name() <<" : " << destPending.size() << " "
                << readPending.size() << std::endl;
        }
        proc = &program_->nextProcedure(*proc);
    }
    return moveNodes;
}

/**
 * A short description of the module, usually the module name,
 * in this case "SequentialFirstFitResourceAllocator".
 *
 * @return The description as a string.
 */   
std::string
SequentialFirstFitResourceAllocator::shortDescription() const {
    return "Startable: SequentialFirstFitResourceAllocator";
}

/**
 * Optional longer description of the Module.
 *
 * This description can include usage instructions, details of choice of
 * helper modules, etc.
 *
 * @return The description as a string.
 */
std::string
SequentialFirstFitResourceAllocator::longDescription() const {
    std::string answer = "Startable: SequentialFirstFitResourceAllocator.";
    answer += " Simple mapping of operations to target architecture, no \
	instruction scheduling performed.";
    return answer;
}

SCHEDULER_PASS(SequentialFirstFitResourceAllocator)
