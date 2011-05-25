/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file LLVMTCECFGDDGBuilder.hh
 *
 * This builder builds a CFG and DDG from the new LLVM TTA backend format.
 *
 * @author Heikki Kultala 2011
 * @note reting: red
 */

#include "LLVMTCECFGDDGBuilder.hh"
#include "ControlFlowGraph.hh"
#include "Procedure.hh"
#include "AddressSpace.hh"
#include "ControlUnit.hh"
#include "DataDependenceGraph.hh"
#include "TerminalBasicBlockReference.hh"
#include "TerminalSymbolReference.hh"

#include "PreOptimizer.hh"
#include "BBSchedulerController.hh"
#include "CycleLookBackSoftwareBypasser.hh"
#include "CopyingDelaySlotFiller.hh"

#include "InstructionReferenceManager.hh"

#include "POMDisassembler.hh"

#include "LLVMTCECmdLineOptions.hh"

#if (!(defined(LLVM_2_8) || defined(LLVM_2_7)))
#include <llvm/ADT/SmallString.h>
#endif



namespace llvm {

char LLVMTCECFGDDGBuilder::ID = -1;


LLVMTCECFGDDGBuilder::LLVMTCECFGDDGBuilder(
    llvm::TargetMachine& tm, TTAMachine::Machine* mach, InterPassData& ipd) :
    LLVMTCEBuilder(tm, mach, ID), ipData_(&ipd), ddgBuilder_(ipd) {
}

/**
 * Writes machine function to POM.
 *
 * Actually does things to MachineFunction which was supposed to be done
 * in runOnMachineFunction, but which cannot be done during that, because
 * MachineDCE is not ready yet at that time...
 */
bool
LLVMTCECFGDDGBuilder::writeMachineFunction(MachineFunction& mf) {

    if (tm_ == NULL)
        tm_ = &mf.getTarget();
    // ensure data sections have been initialized
    initDataSections();


    // omit empty functions..
    if (mf.begin() == mf.end()) return true;

#if (defined(LLVM_2_7) || defined(LLVM_2_8))
    TCEString fnName = mang_->getNameWithPrefix(mf.getFunction());
#else
    SmallString<256> Buffer;
    mang_->getNameWithPrefix(Buffer, mf.getFunction(), false);
    TCEString fnName(Buffer.c_str());
#endif

    emitConstantPool(*mf.getConstantPool());

    TTAMachine::AddressSpace* as = mach_->controlUnit()->addressSpace();
    
    TTAProgram::Procedure* procedure = 
        new TTAProgram::Procedure(fnName, *as);

    prog_->addProcedure(procedure);

    ControlFlowGraph* cfg = new ControlFlowGraph(fnName, prog_);
    
/*
    // TODO: antidep level bigger on trunk where loop scheduling.
    DataDependenceGraph* ddg = new DataDependenceGraph(
        allParamRegs_, fnName, DataDependenceGraph::INTRA_BB_ANTIDEPS,
        NULL, true, false);
*/

    bbMapping_.clear();

    std::set<const MachineBasicBlock*> endingCallBBs;
    std::set<const MachineBasicBlock*> endingCondJumpBBs;
    std::set<const MachineBasicBlock*> endingUncondJumpBBs;
    std::map<const BasicBlockNode*,BasicBlockNode*> callSuccs;
    std::map<const BasicBlockNode*, const MachineBasicBlock*> condJumpSucc;
    std::map<const BasicBlockNode*, BasicBlockNode*> ftSuccs;

    BasicBlockNode* entry = new BasicBlockNode(0, 0, true);
    cfg->addNode(*entry);

    // 1st loop create all BB's. do not fill them yet.
    for (MachineFunction::const_iterator i = mf.begin(); i != mf.end(); i++) {
        const MachineBasicBlock& mbb = *i;
        //TODO: what does the parameter do? start address?
        ::BasicBlock* bb = new ::BasicBlock(0);

        // first BB of the program
        if (prog_->procedureCount() == 1 && cfg->nodeCount() == 1) {
            LLVMTCEBuilder::emitSPInitialization(*bb);
        }

        // TODO: what should these contain?
        std::set<std::string> emptyMBBs;
        
        TCEString bbName = mbbName(mbb);
        BasicBlockNode* bbn = new BasicBlockNode(*bb);


        bbMapping_[&(mbb)] = bbn;
        cfg->addNode(*bbn);
        // if this was first, create edge from entry
        if (i == mf.begin()) {
            ControlFlowEdge* edge = new ControlFlowEdge;
            cfg->connectNodes(*entry, *bbn, *edge);
        }

        // 1st loop: create all BB's. Do not fill them with instructions.
        for (MachineBasicBlock::const_iterator j = mbb.begin();
             j != mbb.end(); j++) {
            if (j->getDesc().isCall()) {
                // if last ins of bb is call, no need to create new bb.
                if (&(*j) == &(mbb.back())) {
                    endingCallBBs.insert(&(*i));
                } else {
                    // create a new BB for code after the call
                    bb = new ::BasicBlock(0);
                    BasicBlockNode* succBBN = new BasicBlockNode(*bb);
                    callSuccs[bbn] = succBBN;
                    bbn = succBBN;
                    cfg->addNode(*bbn);
                }
            } else {
                // also need to split BB on cond branch.
                // LLVM BB may contain 2 branches.
                if (j->getDesc().isBranch()) {
		    // TODO: correctly detect conditional branches
                    if (operationName(*j) == "?jump") {
                        assert(j->getNumOperands() == 2);
                        const MachineOperand& mo = j->getOperand(1);
                        assert(mo.isMBB());
                        condJumpSucc[bbn] = mo.getMBB();

                        if (&(*j) == &(mbb.back())) {
                            endingCondJumpBBs.insert(&(*i));
                        } else {
                            // create a new BB for code after the call.
                            // this should only contain one uncond jump.
                            bb = new ::BasicBlock(0);
                            BasicBlockNode* succBBN = new BasicBlockNode(*bb);
                            ftSuccs[bbn] = succBBN;
                            bbn = succBBN;
                            cfg->addNode(*bbn);
                        }
                    } else {
                        // has to be uncond jump, and last ins of bb.
                        assert(operationName(*j) == "jump");
                        assert(&(*j) == &(mbb.back()));
                        endingUncondJumpBBs.insert(&(*i));
                    }
                }
            }
        }
    }

    // 2nd loop: create all instructions inside BB's.
    for (MachineFunction::const_iterator i = mf.begin(); i != mf.end(); i++) {
        const MachineBasicBlock& mbb = *i;
        
        TCEString bbName = mbbName(mbb);
        BasicBlockNode* bbn = bbMapping_[&mbb];
        ::BasicBlock* bb = &bbn->basicBlock();

        for (MachineBasicBlock::const_iterator j = mbb.begin();
             j != mbb.end(); j++) {

            TTAProgram::Instruction* instr = NULL;
            instr = emitInstruction(j, bb);

            // Pseudo instructions:
            if (instr == NULL) continue;
            
            // if call, switch tto next bb(in callsucc chain)
            if (instr->hasCall() && &(*j) != &(mbb.back())) {
                bbn = callSuccs[bbn];
                bb = &bbn->basicBlock();
            }

            // conditional jump that is not last ins splits a bb.
            if (j->getDesc().isBranch() && operationName(*j) == "?jump"
                && &(*j) != &(mbb.back())) {
                bbn = ftSuccs[bbn];
                bb = &bbn->basicBlock();
            }
        }

//        ddgBuilder_->constructIndividualBB(REGISTERS_AND_PROGRAM_OPERATIONS);

    }

    // 3rd loop: create edges?
    for (MachineFunction::const_iterator i = mf.begin(); i != mf.end(); i++) {
        const MachineBasicBlock& mbb = *i;
        const BasicBlockNode* bbn = bbMapping_[&mbb];

        // is last ins a call?
        bool callPass = AssocTools::containsKey(endingCallBBs, &mbb);
        bool ftPass = AssocTools::containsKey(endingCondJumpBBs, &mbb);
        bool hasUncondJump = 
            AssocTools::containsKey(endingUncondJumpBBs, &mbb);

        const MachineBasicBlock* jumpSucc = NULL;

        while(true) {
            std::map<const BasicBlockNode*, BasicBlockNode*>::iterator j =
                callSuccs.find(bbn);
            
            std::map<const BasicBlockNode*, BasicBlockNode*>::iterator k =
                ftSuccs.find(bbn);

            // same BB should not be in both.
            assert(j == callSuccs.end() || k == ftSuccs.end());

            if (j != callSuccs.end()) {
                const BasicBlockNode* callSucc = j->second;
                assert(callSucc != NULL);
                ControlFlowEdge* cfe = new ControlFlowEdge(
                    ControlFlowEdge::CFLOW_EDGE_NORMAL, 
                    ControlFlowEdge::CFLOW_EDGE_CALL);
                cfg->connectNodes(*bbn, *callSucc, *cfe);
                bbn = callSucc;
                continue;
            }

            // BB has conditional jump..
            if (k != ftSuccs.end()) {
                    jumpSucc = condJumpSucc[bbn];
                assert(jumpSucc != NULL);
                ControlFlowEdge* cfe = new ControlFlowEdge(
                    ControlFlowEdge::CFLOW_EDGE_TRUE,
                    ControlFlowEdge::CFLOW_EDGE_JUMP);
                cfg->connectNodes(*bbn, *bbMapping_[jumpSucc], *cfe);

                const BasicBlockNode* ftSucc = k->second;
                assert(ftSucc != NULL);
                cfe = new ControlFlowEdge(
                    ControlFlowEdge::CFLOW_EDGE_FALSE,
                    ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH);
                cfg->connectNodes(*bbn, *ftSucc, *cfe);
                bbn = ftSucc;
                continue;
            }
            break;
        }

        for (MachineBasicBlock::const_succ_iterator si = mbb.succ_begin();
             si != mbb.succ_end(); si++) {
            const MachineBasicBlock* succ = *si;
            BasicBlockNode *succBBN = bbMapping_[succ];
            // TODO: type of the edge
            ControlFlowEdge* cfe = NULL;
            // if last ins of bb was call, cheyte call-pass edge.
            if (callPass) {
                cfe = new ControlFlowEdge(
                    ControlFlowEdge::CFLOW_EDGE_NORMAL, 
                    ControlFlowEdge::CFLOW_EDGE_CALL);
            } else {
                // do we have conditional jump?
                if (jumpSucc != NULL) {
                    if (succ != jumpSucc) {
                        // cond jump was last ins of bb.
                        // fall-through to next bb.
                        if (ftPass) {
                            // fall through. But did we create a new BB?
                            cfe = new ControlFlowEdge(
                                ControlFlowEdge::CFLOW_EDGE_FALSE,
                                ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH);
                        } else {
                            // has jump to some other BB.
                            cfe = new ControlFlowEdge;
                        }
                    } else {
                        // the jumps itself should already have been generated
                        // earlier.
                        continue;
                    }
                } else { // no conditional jump. ft to next bb.
                    // unconditional jump to nxt bb
                    if (hasUncondJump) {
                        cfe = new ControlFlowEdge;
                    } else {
                        // no unconditional jump to next bb. limits bb
                        // reordering
                        cfe = new ControlFlowEdge(
                            ControlFlowEdge::CFLOW_EDGE_NORMAL,
                            ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH);
                    }
                }
            }
            cfg->connectNodes(*bbn, *succBBN, *cfe);
        }
    }
    // create jumps to exit node
    BasicBlockNode* exit = new BasicBlockNode(0, 0, false, true);
    cfg->addNode(*exit);
    cfg->addExitFromSinkNodes(exit);

    // add back edge properties.
    cfg->detectBackEdges();

    // TODO: on trunk single bb loop(swp), last param true(rr, threading)
    DataDependenceGraph* ddg = ddgBuilder_.build(
        *cfg, DataDependenceGraph::INTRA_BB_ANTIDEPS, NULL, true, false);

    PreOptimizer preOpt(*ipData_);
    preOpt.handleCFGDDG(*cfg, *ddg);

    CycleLookBackSoftwareBypasser bypasser;
    CopyingDelaySlotFiller dsf;
    BBSchedulerController bbsc(*ipData_, &bypasser, &dsf);
    bbsc.handleCFGDDG(*cfg, *ddg, *mach_ );

    cfg->convertBBRefsToInstRefs(prog_->instructionReferenceManager());

    ddg->writeToDotFile(fnName + "_ddg.dot");
    cfg->writeToDotFile(fnName + "_cfg.dot");

    dsf.fillDelaySlots(*cfg, *ddg, *mach_, prog_->universalMachine(), true);

    cfg->copyToProcedure(*procedure);//, &prog_->instructionReferenceManager());
    codeLabels_[fnName] = &procedure->firstInstruction();

    delete ddg;
    delete cfg;
    return false;
}

TTAProgram::Terminal*
LLVMTCECFGDDGBuilder::createMBBReference(const MachineOperand& mo) {
    return new TTAProgram::TerminalBasicBlockReference(
        bbMapping_[mo.getMBB()]->basicBlock());
}

TTAProgram::Terminal*
LLVMTCECFGDDGBuilder::createSymbolReference(const TCEString& symbolName) {
    return new TTAProgram::TerminalSymbolReference(symbolName);
}

bool
LLVMTCECFGDDGBuilder::doFinalization(Module& m ) { 

    LLVMTCEBuilder::doFinalization(m);
    prog_->convertSymbolRefsToInsRefs();

    LLVMTCECmdLineOptions* options =
        dynamic_cast<LLVMTCECmdLineOptions*>(Application::cmdLineOptions());

    std::string outputFileName = "cfgddgbuilder.tpef";
    if (options->isOutputFileDefined()) {
        outputFileName = options->outputFile();
    }

//    TTAProgram::Program::writeToTPEF(*prog_, "cfgddgbuilder.tpef");
    TTAProgram::Program::writeToTPEF(*prog_, outputFileName);
    exit(0);
    return false; 
}
    
}


