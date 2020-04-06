/*
    Copyright (c) 2002-2020 Tampere University.

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
 * @file LoopPrologAndEpilogBuilder.cc
 *
 * Definition of LoopPrologAndEpilogBuilder interface.
 *
 * @author Mikael Lepist� 2008 (mikael.lepisto-no.spam-tut.fi)
 * @author HeikkI Kultala 2015-2020 (heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "LoopPrologAndEpilogBuilder.hh"

#include "DataDependenceGraph.hh"
#include "SimpleResourceManager.hh"
#include "BasicBlock.hh"
#include "Instruction.hh"
#include "ControlFlowGraph.hh"
#include "TerminalInstructionReference.hh"
#include "InstructionReference.hh"
#include "InstructionReferenceManager.hh"
#include "Immediate.hh"
#include "Move.hh"
#include "CopyingDelaySlotFiller.hh"
#include "CodeGenerator.hh"
#include "TerminalInstructionReference.hh"
#include "BasicBlockPass.hh"
#include "ImmediateUnit.hh"
#include "BF2Scheduler.hh"
#include "ControlFlowEdge.hh"

#include <set>

LoopPrologAndEpilogBuilder::LoopPrologAndEpilogBuilder() {
}

LoopPrologAndEpilogBuilder::~LoopPrologAndEpilogBuilder() {
}

void
LoopPrologAndEpilogBuilder::moveJumpDestination(
    TTAProgram::InstructionReferenceManager& irm,
    BasicBlockNode& tail, BasicBlockNode& dst,
    ControlFlowEdge& jumpEdge) {
    TTAProgram::BasicBlock& tailBB = tail.basicBlock();

    auto predicate = jumpEdge.edgePredicate();
    std::pair<int, TTAProgram::Move*> jumpData =
        CopyingDelaySlotFiller::findJump(tailBB, &predicate);

    std::pair<TTAProgram::Move*, TTAProgram::Immediate*> jumpAddressData;

    // should be the same
    int jumpIndex = jumpData.first;
    TTAProgram::Move* jumpMove = jumpData.second;
    if (jumpMove == NULL) {
        abortWithError("incoming jump not found");
    }

    TTAProgram::InstructionReference ir = irm.createReference(
        dst.basicBlock().instructionAtIndex(
            dst.basicBlock().skippedFirstInstructions()));

    if (!jumpMove->source().isInstructionAddress()) {
        // address comes from LIMM, search the write to limm reg.
        jumpAddressData =
            CopyingDelaySlotFiller::findJumpImmediate(
                jumpIndex, *jumpMove, irm);
        if (jumpAddressData.first == NULL &&
            jumpAddressData.second == NULL) {
            //Imm source not found, aborting
            abortWithError("Unknown immediate contains jump address");
            return;
        }
        if (jumpAddressData.first != NULL) {
            jumpAddressData.first->setSource(
                new TTAProgram::TerminalInstructionReference(ir));
        } else {
            jumpAddressData.second->setValue(
                new TTAProgram::TerminalInstructionReference(ir));
        }
    } else {
        jumpMove->source().setInstructionReference(ir);
    }
}

/**
 * Adds a prolog into control flow graph.
 *
 * Creates the node and fixed edges and instruction references.
 *
 * @param cfg cfg where to add the prolog
 * @param prologBB bb which contains the prolog
 * @param loopBBN node of cfg which contains the loop
 */
void
LoopPrologAndEpilogBuilder::addPrologIntoCfg(
    ControlFlowGraph& cfg, BasicBlockNode& prologBBN,
    BasicBlockNode& loopBBN) {

    if (Application::verboseLevel() > 1) {
        std::cerr << "adding prolog into cfg" << std::endl;
    }
    cfg.addNode(prologBBN);

    // update incoming cfg edges to point into prolog
    for (int i = 0; i < cfg.inDegree(loopBBN); i++) {
        ControlFlowEdge&e = cfg.inEdge(loopBBN,i);
        BasicBlockNode& tail = cfg.tailNode(e);
        if (&tail != &loopBBN) {
            cfg.moveInEdge(loopBBN, prologBBN,e);
            i--;
            if (e.isJumpEdge()) {
                if (Application::verboseLevel() > 1) {
                    std::cerr << "\tmoving jump into loop into prolog"
                              << std::endl;
                }
                moveJumpDestination(cfg.instructionReferenceManager(),
                                    tail, prologBBN, e);
            }
        }
    }

    // connect prolog and loop
    cfg.connectNodes(
        prologBBN, loopBBN, *(new ControlFlowEdge(
            ControlFlowEdge::CFLOW_EDGE_NORMAL,
            ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH)));

}

/**
 * Adds an epilog into control flow graph.
 *
 * Creates the node and fixed edges.
 *
 * @param cfg cfg where to add the epilog
 * @param prologBB bb which contains the epilog
 * @param loopBBN node of cfg which contains the loop
 */

void
LoopPrologAndEpilogBuilder::addEpilogIntoCfg(
    ControlFlowGraph& cfg, BasicBlockNode& epilogBBN,
    BasicBlockNode& loopBBN) {

    cfg.addNode(epilogBBN);
    assert(cfg.outDegree(loopBBN) == 2);

    for (int i = 0; i < cfg.outDegree(loopBBN); i++) {
        ControlFlowEdge& loopExitEdge = cfg.outEdge(loopBBN,i);
        if (!loopExitEdge.isFallThroughEdge()) {
            continue;
        }
        BasicBlockNode& succNode = cfg.headNode(loopExitEdge);
        cfg.moveInEdge(succNode, epilogBBN, loopExitEdge);
        cfg.connectNodes(
            epilogBBN, succNode, *(new ControlFlowEdge(
                                       ControlFlowEdge::CFLOW_EDGE_NORMAL,
                                       ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH
                                       )));

        return;
    }
    assert(false&&"Loop exit fall-thru not in ddg!");
}

int
LoopPrologAndEpilogBuilder::build(
    DataDependenceGraph& ddg, SimpleResourceManager& rm,
    ControlFlowGraph& cfg, BasicBlockNode& loopBBN, int endCycle,
    bool createEpilog) {

    if (Application::verboseLevel() > 1) {
        Application::logStream() << "Building epilog and prolog for the loop."
                                 << std::endl;
        std::cerr << "rm.smallestCycle: " << rm.smallestCycle() << std::endl;
        std::cerr << "end cycle: " << endCycle << std::endl;
    }

    int ii = rm.initiationInterval();
    if (Application::verboseLevel() > 1) {
        Application::logStream() << "Investigate nodes. ii: " << ii
                                 << std::endl;
    }

    // resolve how many times loop overlaps
    int overlap_count;
    if (endCycle != -1) {
        overlap_count = ((endCycle - rm.smallestCycle()) / ii);
    } else {
        overlap_count = ddg.largestCycle() / ii;
    }

    TTAProgram::Move* jumpMove = NULL;
    if (overlap_count > 0) {
        if (Application::verboseLevel() > 1) {
            Application::logStream()
                << "Building prolog and epilog..... "
                << " Overlapcount: " << overlap_count << std::endl;
        }

        DataDependenceGraph* rootDDG = static_cast<DataDependenceGraph*>(
            ddg.rootGraph());

        // make room for moves of prolog and epilog
        TTAProgram::BasicBlock *prolog =
            new TTAProgram::BasicBlock(loopBBN.originalStartAddress());
        BasicBlockNode *prologNode = new BasicBlockNode(*prolog, true, true,
            loopBBN.originalStartAddress(), false);

        TTAProgram::BasicBlock *epilog = NULL;
        BasicBlockNode* epilogNode = NULL;
        if (createEpilog) {
            epilog =
                new TTAProgram::BasicBlock(loopBBN.originalStartAddress());
            epilogNode = new BasicBlockNode(
                *epilog, true, true, loopBBN.originalStartAddress(), false);
        }
        for (int i = 0; i < overlap_count * ii; i++) {
            prolog->add(new TTAProgram::Instruction());
            if (createEpilog) {
                epilog->add(new TTAProgram::Instruction());
            }
        }

        // scan for long immediates. these can be copied to both
        // prolog and epilog.
        for (int j = 0; j < overlap_count; j++) {
            for (int i = 0; i < ii; i++) {
                TTAProgram::Instruction* ins = rm.instruction(i);
                for (int k = 0; k < ins->immediateCount(); k++) {
                    auto newImm = ins->immediate(k).copy();
                    auto newImm2 = ins->immediate(k).copy();
                    prolog->instructionAtIndex(i+j*ii).addImmediate(newImm);
                    if (createEpilog) {
                        epilog->instructionAtIndex(i+j*ii).addImmediate(newImm2);
                    }
                }
            }
        }
        int newZero = endCycle + 1  - (ii * (1 + overlap_count));
        for (int i = 0 ; i < overlap_count; i++) {
            for (int j = 0; j < ddg.nodeCount(); j++) {
                MoveNode& n = ddg.node(j);

                // ignore jump
                if (n.move().isControlFlowMove()) {
                    jumpMove = &n.move();
                    continue;
                }

                int cycle;
                if (endCycle != -1) {
                    cycle = n.cycle() - newZero;
                } else {
                    cycle = n.cycle();
                }
                int round = cycle / ii;
                int place = cycle % ii;

                MoveNode* nodeCopy;
                if (createEpilog || round == 0) {
                    nodeCopy = n.copy();
                    nodeCopy->move().setGuard(NULL);

                    if (endCycle != -1) {
                        nodeCopy->setCycle(place + newZero);
                    } else {
                        nodeCopy->setCycle(place);
                    }

                    if (Application::verboseLevel() > 1) {
                        Application::logStream()
                            << "Node cycle: " << cycle
                            << " round: " << round
                            << " place: " << place << " "
                            << n.toString()<< "\t" ;
                    }
                } else {
                    continue;
                }
/*
                if (createEpilog) {
                    if (n.move().isControlFlowMove()) {
                        assert(!n.move().isUnconditional());
                        if (Application::verboseLevel() > 1) {
                            std::cerr << "jumpmove reversing guard at address: "
                                      << &nodeCopy->move()<< std::endl;
                        }
                        nodeCopy->move().setGuard(
                            TTAProgram::CodeGenerator::createInverseGuard(
                                n.move().guard(), &n.move().bus()));
                        // TODO: set jump target to first of epilog
                        nodeCopy->move().setSource(
                            new TTAProgram::TerminalInstructionReference(
                                cfg.instructionReferenceManager().createReference(
                                    epilog->instructionAtIndex(0))));
                    }
                }
*/
                // select if node should go to prolog or epilog
                if (round <= i) {
                    prolog->instructionAtIndex(
                        (i*ii)+place).addMove(nodeCopy->movePtr());
                    if (Application::verboseLevel() > 1) {
                        Application::logStream() << " to prolog" << std::endl;
                    }
                    rootDDG->addNode(*nodeCopy, *prologNode);
                    rootDDG->copyExternalInEdges(*nodeCopy, n);
                    rootDDG->copyExternalOutEdges(*nodeCopy, n);

                } else {
                    if (createEpilog) {
                        epilog->instructionAtIndex(
                            (i*ii)+place).addMove(nodeCopy->movePtr());
                        if (Application::verboseLevel() > 1) {
                            Application::logStream() << " to epilog" << std::endl;
                        }
                        rootDDG->addNode(*nodeCopy, *epilogNode);
                        rootDDG->copyExternalInEdges(*nodeCopy, n);
                        rootDDG->copyExternalOutEdges(*nodeCopy, n);
                    }
                }
            }
        }
        assert(jumpMove != NULL);

        if (Application::verboseLevel() > 1) {
            Application::logStream()
                << "Disassembly of prolog" << std::endl <<
                prolog->disassembly() << std::endl;
            if (createEpilog) {
                Application::logStream()
                    << "Disassembly of epilog" << std::endl <<
                    epilog->disassembly() << std::endl;
            }
        }

        if (optimizeProlog(*prolog)) {
            if (Application::verboseLevel() > 1) {
                Application::logStream()
                    << "Disassembly of optimized prolog" << std::endl <<
                    prolog->disassembly() << std::endl;
            }
            addPrologIntoCfg(cfg, *prologNode, loopBBN);
        } else {
            delete prologNode;
        }
        if (Application::verboseLevel() > 1) {
            Application::logStream()
                << "Prolog optimized."
                << std::endl;
        }
        if (createEpilog) {
            if (optimizeEpilog(*epilog)) {
                if (Application::verboseLevel() > 1) {
                    Application::logStream()
                        << "Disassembly of optimized epilog" << std::endl <<
                        epilog->disassembly() << std::endl;
                }
                addEpilogIntoCfg(cfg, *epilogNode, loopBBN);
            } else {
                delete epilogNode; // TODO: what about the BB?
            }
            if (Application::verboseLevel() > 1) {
                Application::logStream()
                    << "Epilog optimized."
                    << std::endl;
            }
        }
    } else {
        if (Application::verboseLevel() > 1) {
            Application::logStream()
                << "No overlapping instructions.No need for prolog or epilog."
                << "Should have used normal scheduler."
                << std::endl;
        }
    }

    return overlap_count;
}

/** Stupid immediates still have to be copied from the main loop :( */
BasicBlockNode* LoopPrologAndEpilogBuilder::addPrologFromRM(
    SimpleResourceManager& prologRM,
    SimpleResourceManager& loopRM,
    ControlFlowGraph& cfg, BasicBlockNode& loopBBN) {

    int startCycle = prologRM.smallestCycle();

    TTAProgram::BasicBlock* prologBB = new TTAProgram::BasicBlock;
    BasicBlockNode* prologNode = new BasicBlockNode(*prologBB);

    if (Application::verboseLevel() > 1) {
        Application::logStream()
            << "Copying prolog rm to bb" << prologNode->toString()
            << " Start cycle: " << startCycle << std::endl;
    }
    BasicBlockPass::copyRMToBB(prologRM, *prologBB, prologRM.machine(),
                               cfg.instructionReferenceManager(),
                               loopRM.initiationInterval()-1
                               + BF2Scheduler::PROLOG_CYCLE_BIAS);

    if (optimizeProlog(*prologBB)) {
        addPrologIntoCfg(cfg, *prologNode, loopBBN);
        prologNode->setScheduled();

        if (Application::verboseLevel() > 1) {
            Application::logStream()
                << "Prolog added to cfg" << prologNode->toString() << std::endl
                << "Disassembly of prolog" << std::endl <<
                prologBB->disassembly() << std::endl;
        }
    } else {
        if (Application::verboseLevel() > 1) {
            Application::logStream()
                << "Empty prolog, not added to cfg" << std::endl;
        }
        delete prologNode;
    }
    return prologNode;
}

BasicBlockNode* LoopPrologAndEpilogBuilder::addEpilogFromRM(
    SimpleResourceManager& prologEpilogRM,
    int ii, ControlFlowGraph& cfg, BasicBlockNode& loopBBN) {

    TTAProgram::BasicBlock* epilogBB = new TTAProgram::BasicBlock;
    BasicBlockNode* epilogNode = new BasicBlockNode(*epilogBB);

    if (Application::verboseLevel() > 1) {
        Application::logStream()
            << "Copying epilog rm to bb" << epilogNode->toString()
            << std::endl;
    }

    for (int i = BF2Scheduler::PROLOG_CYCLE_BIAS + ii;
         i <= prologEpilogRM.largestCycle(); i++) {
        TTAProgram::Instruction* newInstruction =
            prologEpilogRM.instruction(i);
        prologEpilogRM.loseInstructionOwnership(i);
        epilogBB->add(newInstruction);
    }
    if (optimizeEpilog(*epilogBB)) {
        addEpilogIntoCfg(cfg, *epilogNode, loopBBN);
        epilogNode->setScheduled();
        if (Application::verboseLevel() > 1) {
            Application::logStream()
                << "epilog added to cfg" << epilogNode->toString() << std::endl;

            Application::logStream()
                << "Disassembly of epilog" << std::endl <<
                epilogBB->disassembly() << std::endl;
        }
    } else {
        if (Application::verboseLevel() > 1) {
            Application::logStream()
                << "epilog empty, not added to cfg." << std::endl;
        }
        delete epilogNode;
    }

    return epilogNode;

}

bool
LoopPrologAndEpilogBuilder::optimizeProlog(TTAProgram::BasicBlock& prolog) {

    if (prolog.instructionCount() == 0) {
        return false;
    }
    // optimize away immediate values not used
    for (int i = 0; i < prolog.instructionCount() ;i++) {
        TTAProgram::Instruction& ins = prolog.instructionAtIndex(i);
        for (int j = 0; j < ins.immediateCount(); j++) {
            bool used = false;
            bool overwritten = false;
            TTAProgram::Immediate& imm = ins.immediate(j);
            const TTAProgram::Terminal& t = imm.destination();
            const TTAProgram::Terminal& dst = imm.destination();
            const TTAMachine::ImmediateUnit& immu = dst.immediateUnit();
            for (int k = i + immu.latency() ;
                 k  < prolog.instructionCount() &&
                     !used && !overwritten; k++) {

                TTAProgram::Instruction& ins2 = prolog.instructionAtIndex(k);
                // if immu has latency 0, check immediate before moves.
                if (immu.latency() == 0 && k > i) {
                    for (int l = 0; l < ins2.immediateCount(); l++) {
                        const TTAProgram::Immediate& imm2 = ins2.immediate(l);
                        const TTAProgram::Terminal& t2 = imm2.destination();
                        if (&t2.immediateUnit() == & t.immediateUnit() &&
                            t2.index() == t.index()) {
                            overwritten = true;
                            break;
                        }
                    }
                }

                for (int l = 0; l < ins2.moveCount(); l++) {
                    const TTAProgram::Move& m = ins2.move(l);
                    const TTAProgram::Terminal& t2 = m.source();
                    if (t2.isImmediateRegister() &&
                        &t2.immediateUnit() == &dst.immediateUnit() &&
                        t2.index() == dst.index()) {
                        used = true;
                        break;
                    }
                }
                // if immu has latency 1, check moves before imms
                if (immu.latency() == 1) {
                    for (int l = 0; l < ins2.immediateCount(); l++) {
                        const TTAProgram::Immediate& imm2 = ins2.immediate(l);
                        const TTAProgram::Terminal& t2 = imm2.destination();
                        if (&t2.immediateUnit() == & t.immediateUnit() &&
                            t2.index() == t.index()) {
                            overwritten = true;
                            break;
                        }
                    }
                }
            }
            if (!used && overwritten) {
                ins.removeImmediate(imm);
            }
        }
    }
    for (int i = 0; i <= prolog.instructionCount() -1 ;i++) {
        TTAProgram::Instruction& ins = prolog.instructionAtIndex(i);
        // check for moves is enough. no imms after moves.
        if (ins.moveCount() > 0 || ins.immediateCount() > 0) {
            break;
        } else {
            // empty? leave one ins behind just for
            if (i == prolog.instructionCount() - 1) {
                return false;
            }
            prolog.remove(ins);
            delete &ins;
            // We removed current instruction, there shell be new current
            // instruction once we increse i at beginning of the loop.
            i--;
        }
    }
    return true;
}


bool
LoopPrologAndEpilogBuilder::optimizeEpilog(TTAProgram::BasicBlock& epilog) {

    if (epilog.instructionCount() == 0) {
        return false;
    }

    for (int i = epilog.instructionCount() -1 ; i >= 0 ; i--) {
        TTAProgram::Instruction& ins = epilog.instructionAtIndex(i);
        // check for moves is enough. no imms after moves.
        if (ins.moveCount() > 0) {
            break;
        } else {
            // empty? leave one ins behind just for
            if (i == 0) {
                return false;
            }
            assert(i!= 0);
            epilog.remove(ins);
            delete &ins;
        }
    }
    return true;
}
