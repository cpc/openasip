/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file LLVMTCEIRBuilder.hh
 *
 * This builder builds a CFG and DDG from the new LLVM TTA backend format.
 *
 * @author Heikki Kultala 2011
 * @note reting: red
 */

#ifdef NDEBUG
#undef NDEBUG
#endif

#include "LLVMTCEIRBuilder.hh"
#include "ControlFlowGraph.hh"
#include "Procedure.hh"
#include "AddressSpace.hh"
#include "ControlUnit.hh"
#include "DataDependenceGraph.hh"
#include "TerminalBasicBlockReference.hh"
#include "TerminalSymbolReference.hh"
#include "TerminalFUPort.hh"
#include "SequentialScheduler.hh"
#include "PreOptimizer.hh"
#include "BBSchedulerController.hh"
#include "CycleLookBackSoftwareBypasser.hh"
#include "CopyingDelaySlotFiller.hh"

#include "InstructionReferenceManager.hh"

#include "RegisterCopyAdder.hh"
#include "LLVMTCECmdLineOptions.hh"

#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "FUPort.hh"

#include <llvm/ADT/SmallString.h>
#include <llvm/MC/MCContext.h>
#include <llvm/MC/MCSymbol.h>
#include <llvm/CodeGen/MachineJumpTableInfo.h>

//#define WRITE_DDG_DOTS
//#define WRITE_CFG_DOTS

namespace llvm {

char LLVMTCEIRBuilder::ID = -1;

LLVMTCEIRBuilder::LLVMTCEIRBuilder(
    const llvm::TargetMachine& tm, TTAMachine::Machine* mach, 
    InterPassData& ipd, bool functionAtATime, bool modifyMF) :
    LLVMTCEBuilder(tm, mach, ID, functionAtATime), ipData_(&ipd), 
    ddgBuilder_(ipd), modifyMF_(modifyMF) {
    RegisterCopyAdder::findTempRegisters(*mach, ipd);

    if (functionAtATime_) {
        umach_ = new UniversalMachine();        
        const TTAMachine::Machine::FunctionUnitNavigator fuNav =
            mach->functionUnitNavigator();

        // the supported operation set
        for (int i = 0; i < fuNav.count(); i++) {
            const TTAMachine::FunctionUnit& fu = *fuNav.item(i);
            for (int o = 0; o < fu.operationCount(); o++) {
                opset_.insert(
                    StringTools::stringToLower(fu.operation(o)->name()));
            }
        }

    } // otherwise the umach_ will be used from the Program instance
}

bool
LLVMTCEIRBuilder::writeMachineFunction(MachineFunction& mf) {

    if (tm_ == NULL)
        tm_ = &mf.getTarget();

    clearFunctionBookkeeping();

    if (!functionAtATime_) {
        // ensure data sections have been initialized when compiling
        // the whole program at a time
        initDataSections();
        emitConstantPool(*mf.getConstantPool());	
    } else {
#ifdef LLVM_2_9
        MCContext* ctx = new MCContext(*tm_->getMCAsmInfo(), NULL);
#else // LLVM-3.x
        MCContext* ctx = new MCContext(
	    *tm_->getMCAsmInfo(), *tm_->getRegisterInfo(), NULL);
#endif
        mang_ = new llvm::Mangler(*ctx, *tm_->getTargetData()); 
    }

    // omit empty functions..
    if (mf.begin() == mf.end()) return true;   

    TTAMachine::AddressSpace* as = mach_->controlUnit()->addressSpace();
    
    SmallString<256> Buffer;
    mang_->getNameWithPrefix(Buffer, mf.getFunction(), false);
    TCEString fnName(Buffer.c_str());

    TTAProgram::Procedure* procedure = 
        new TTAProgram::Procedure(fnName, *as);

    if (!functionAtATime_) {
        prog_->addProcedure(procedure);
    } 
    

    TTAProgram::InstructionReferenceManager* irm = NULL;
    if (functionAtATime_) {
        irm = new TTAProgram::InstructionReferenceManager();
    } else {        
        irm = &prog_->instructionReferenceManager();
    }


    ControlFlowGraph* cfg = buildTCECFG(mf);
#ifdef WRITE_CFG_DOTS
    cfg->writeToDotFile(fnName + "_cfg1.dot");
#endif
    
    LLVMTCECmdLineOptions* options = NULL;
    bool fastCompilation = false;
    if (Application::cmdLineOptions() != NULL) {
        options = 
            dynamic_cast<LLVMTCECmdLineOptions*>(
                Application::cmdLineOptions());
        fastCompilation = options->optLevel() == 0;
    }
    markJumpTableDestinations(mf, *cfg);
    if (fastCompilation) {
        compileFast(*cfg);
    } else {
        compileOptimized(*cfg, *irm);
    }

    if (!modifyMF_) {
        cfg->convertBBRefsToInstRefs(*irm);
    }
    cfg->copyToProcedure(*procedure, irm);
#ifdef WRITE_CFG_DOTS
    cfg->writeToDotFile(fnName + "_cfg4.dot");
#endif
    if (procedure->instructionCount() > 0) {
        codeLabels_[fnName] = &procedure->firstInstruction();
    }

    if (modifyMF_) {
        cfg->copyToLLVMMachineFunction(mf, irm); 
        fixJumpTableDestinations(mf, *cfg);               
        delete cfg;
        return true;
    }

    delete cfg;
    if (functionAtATime_) delete irm;
    return false;
}

ControlFlowGraph*
LLVMTCEIRBuilder::buildTCECFG(llvm::MachineFunction& mf) {

    SmallString<256> Buffer;
    mang_->getNameWithPrefix(Buffer, mf.getFunction(), false);
    TCEString fnName(Buffer.c_str());

    ControlFlowGraph* cfg = new ControlFlowGraph(fnName, prog_);
    
/*
    // TODO: antidep level bigger on trunk where loop scheduling.
    DataDependenceGraph* ddg = new DataDependenceGraph(
        allParamRegs_, fnName, DataDependenceGraph::INTRA_BB_ANTIDEPS,
        NULL, true, false);
*/

    bbMapping_.clear();
    skippedBBs_.clear();

    std::set<const MachineBasicBlock*> endingCallBBs;
    std::set<const MachineBasicBlock*> endingCondJumpBBs;
    std::set<const MachineBasicBlock*> endingUncondJumpBBs;
    std::map<const BasicBlockNode*, BasicBlockNode*> callSuccs;
    std::map<const BasicBlockNode*, const MachineBasicBlock*> condJumpSucc;
    std::map<const BasicBlockNode*, BasicBlockNode*> ftSuccs;
    std::set<const MachineBasicBlock*> emptyMBBs;
    std::map<const BasicBlockNode*, bool> bbPredicates;

    BasicBlockNode* entry = new BasicBlockNode(0, 0, true);
    cfg->addNode(*entry);
    bool firstInsOfProc = true;

    // 1st loop create all BB's. do not fill them yet.
    for (MachineFunction::const_iterator i = mf.begin(); i != mf.end(); i++) {
        const MachineBasicBlock& mbb = *i;
        //TODO: what does the parameter do? start address?
        TTAProgram::BasicBlock* bb = new TTAProgram::BasicBlock(0);

        // first BB of the program
        if (!functionAtATime_ && prog_->procedureCount() == 1 && 
            cfg->nodeCount() == 1) {
            LLVMTCEBuilder::emitSPInitialization(*bb);
        }
        
        TCEString bbName = mbbName(mbb);
        BasicBlockNode* bbn = new BasicBlockNode(*bb);
        bbn->setBBOwnership(true);

        bool newMBB = true;
        bool newBB = true;

        // 1st loop: create all BB's. Do not fill them with instructions.
        for (MachineBasicBlock::const_iterator j = mbb.begin();
             j != mbb.end(); j++) {

            if (!isRealInstruction(*j)) {
                continue;
            }

            if (newBB) {
                newBB = false;
                cfg->addNode(*bbn);
                if (firstInsOfProc) {
                    ControlFlowEdge* edge = new ControlFlowEdge;
                    cfg->connectNodes(*entry, *bbn, *edge);
                    firstInsOfProc = false;
                }

                if (newMBB) {
                    newMBB = false;
                    bbMapping_[&(mbb)] = bbn;
                    for (std::set<const MachineBasicBlock*>::iterator k = 
                             emptyMBBs.begin(); k != emptyMBBs.end(); k++) {
                        skippedBBs_[*k] = bbn;
                    }
                    emptyMBBs.clear();
                }
            }

            if (j->getDesc().isCall()) {
                // if last ins of bb is call, no need to create new bb.
                if (&(*j) == &(mbb.back())) {
                    endingCallBBs.insert(&(*i));
                } else {
                    if (!hasRealInstructions(j, mbb)) {
                        endingCallBBs.insert(&(*i));
                    } else {
                        // create a new BB for code after the call
                        bb = new TTAProgram::BasicBlock(0);
                        BasicBlockNode* succBBN = new BasicBlockNode(*bb);
                        succBBN->setBBOwnership(true);
                        callSuccs[bbn] = succBBN;
                        bbn = succBBN;
                        newBB = true;
                    }
                }
            } else {
                // also need to split BB on cond branch.
                // LLVM BB may contain 2 branches.
                if (j->getDesc().isBranch()) {
                    TCEString opName = operationName(*j);
                    bool pred = false;
                    if (j->getDesc().isConditionalBranch() &&
                        j->getNumOperands() == 2) {

                        if (opName == "?jump") pred = true;

                        bbPredicates[bbn] = pred;
                        const MachineOperand& mo = j->getOperand(1);
                        assert(mo.isMBB());
                        condJumpSucc[bbn] = mo.getMBB();

                        if (&(*j) == &(mbb.back())) {
                            endingCondJumpBBs.insert(&(*i));
                        } else {
                            if (!hasRealInstructions(j, mbb)) {
                                endingCondJumpBBs.insert(&(*i));
                            } else {
                                // create a new BB for code after the call.
                                // this should only contain one uncond jump.
                                bb = new TTAProgram::BasicBlock(0);
                                BasicBlockNode* succBBN = 
                                    new BasicBlockNode(*bb);
                                succBBN->setBBOwnership(true);
                                ftSuccs[bbn] = succBBN;
                                bbn = succBBN;
                                newBB = true;
                            }
                        }
                    } else {
                        // has to be uncond jump, and last ins of bb.
                        assert(&(*j) == &(mbb.back()));
                        endingUncondJumpBBs.insert(&(*i));
                    }
                }
            }
        }
        if (newMBB == true) {
            assert(newBB);
            emptyMBBs.insert(&mbb);
        }
        if (newBB) {
            assert (bb->instructionCount() == 0);
            delete bbn;
        }
    }

    // 2nd loop: create all instructions inside BB's.
    // this can only come after the first loop so that BB's have
    // already been generated.
    for (MachineFunction::const_iterator i = mf.begin(); i != mf.end(); i++) {
        const MachineBasicBlock& mbb = *i;
        
        BasicBlockNode* bbn = NULL;
        std::map<const MachineBasicBlock*, BasicBlockNode*>::iterator
            bbMapIter = bbMapping_.find(&mbb);
        if (bbMapIter == bbMapping_.end()) {
            continue;
        } else {
            bbn = bbMapIter->second;
        }
        TTAProgram::BasicBlock* bb = &bbn->basicBlock();

        /* If this is non-empty, the ProgramOperation of the next 
           instruction should be indexed with the given label. */
        TCEString nextLabel = "";
        for (MachineBasicBlock::const_iterator j = mbb.begin();
             j != mbb.end(); j++) {

            if (!isTTATarget() && 
                (operationName(*j) == "HBR_LABEL" ||
                 operationName(*j) == "PROLOG_LABEL")) {

                /*
                  FIXME: this code fails when there are multiple labels
                  pointing to the same instruction. Only the last label
                  will be indexed. */
                assert(nextLabel == "");
                
                nextLabel = j->getOperand(0).getMCSymbol()->getName().str();
                continue;
            }

            TTAProgram::Instruction* instr = NULL;
            instr = emitInstruction(j, bb);                        

            if (instr == NULL) {
                continue;
            } 

            if (nextLabel != "") {
                TTAProgram::TerminalFUPort& tpo =
                    dynamic_cast<TTAProgram::TerminalFUPort&>(
                        instr->move(0).destination());
                addLabelForProgramOperation(nextLabel, tpo.programOperation());
                nextLabel = "";
            }
            
            // if call, switch to next bb(in callsucc chain)
            if (j->getDesc().isCall() && &(*j) != &(mbb.back())) {
                bbn = callSuccs[bbn];
                bb = &bbn->basicBlock();
            }

            // conditional jump that is not last ins splits a bb.
            if (j->getDesc().isConditionalBranch()
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

        const BasicBlockNode* bbn = NULL;
        std::map<const MachineBasicBlock*,BasicBlockNode*>::iterator
            bbMapIter = bbMapping_.find(&mbb);
        if (bbMapIter == bbMapping_.end()) {
            continue;
        } else {
            bbn = bbMapIter->second;
        }
        
        // is last ins a call?
        bool callPass = AssocTools::containsKey(endingCallBBs, &mbb);
        bool ftPass = AssocTools::containsKey(endingCondJumpBBs, &mbb);
        bool hasUncondJump = 
            AssocTools::containsKey(endingUncondJumpBBs, &mbb);
        
        const MachineBasicBlock* jumpSucc = condJumpSucc[bbn];
        
        while (true) {
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
                jumpSucc = condJumpSucc[bbn];                
                continue;
            }
            
            // BB has conditional jump which is not last ins.
            if (k != ftSuccs.end()) {
                assert(jumpSucc != NULL);
                assert(MapTools::containsKey(bbPredicates,bbn));
                ControlFlowEdge* cfe = new ControlFlowEdge(
                    bbPredicates[bbn] == true ?
                    ControlFlowEdge::CFLOW_EDGE_TRUE:
                    ControlFlowEdge::CFLOW_EDGE_FALSE,
                    ControlFlowEdge::CFLOW_EDGE_JUMP);
                if (MapTools::containsKey(bbMapping_, jumpSucc)) {
                    cfg->connectNodes(*bbn, *bbMapping_[jumpSucc], *cfe);
                } else {
                    cfg->connectNodes(*bbn, *skippedBBs_[jumpSucc], *cfe);
                }
                
                const BasicBlockNode* ftSucc = k->second;
                assert(ftSucc != NULL);
                cfe = new ControlFlowEdge(
                    bbPredicates[bbn] == true ?
                    ControlFlowEdge::CFLOW_EDGE_FALSE:
                    ControlFlowEdge::CFLOW_EDGE_TRUE,
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
            
            BasicBlockNode* succBBN = NULL;
            std::map<const MachineBasicBlock*,BasicBlockNode*>::iterator 
                bbMapIter = bbMapping_.find(succ);
            if (bbMapIter == bbMapping_.end()) {
                succBBN = skippedBBs_[succ];
            } else {
                succBBN = bbMapIter->second;
            }
            
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
                    // fall-through is a pass to next mbb.
                    if (ftPass) {
                        assert(MapTools::containsKey(bbPredicates,bbn));
                        if (succ == jumpSucc) {
                            cfe = new ControlFlowEdge(
                                bbPredicates[bbn] == true ?
                                ControlFlowEdge::CFLOW_EDGE_TRUE:
                                ControlFlowEdge::CFLOW_EDGE_FALSE,
                                ControlFlowEdge::CFLOW_EDGE_JUMP);
                        } else {
                            cfe = new ControlFlowEdge(
                                bbPredicates[bbn] == true ?
                                ControlFlowEdge::CFLOW_EDGE_FALSE:
                                ControlFlowEdge::CFLOW_EDGE_TRUE,
                                ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH);
                        }
                    } else {
                        // split a bb. ft edges created earlier.
                        // cond.jump edge also created earlier. 
                        // just needs to add the uncond jump edge.
                        
                        if (succ == jumpSucc) {
                            continue;
                        }                        
                        cfe = new ControlFlowEdge;
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

    fixProgramOperationReferences();
    
    // create jumps to exit node
    BasicBlockNode* exit = new BasicBlockNode(0, 0, false, true);
    cfg->addNode(*exit);
    cfg->addExitFromSinkNodes(exit);
    // add back edge properties.
    cfg->detectBackEdges();
    
    return cfg;
}

void
LLVMTCEIRBuilder::compileFast(ControlFlowGraph& cfg) {
    SequentialScheduler sched(*ipData_);
    sched.handleControlFlowGraph(cfg, *mach_);
}

void
LLVMTCEIRBuilder::compileOptimized(
    ControlFlowGraph& cfg, 
    TTAProgram::InstructionReferenceManager& irm) {
    // TODO: on trunk single bb loop(swp), last param true(rr, threading)
    DataDependenceGraph* ddg = ddgBuilder_.build(
        cfg, DataDependenceGraph::INTRA_BB_ANTIDEPS, NULL, true, true);

    TCEString fnName = cfg.name();
#ifdef WRITE_DDG_DOTS
    ddg.writeToDotFile(fnName + "_ddg1.dot");
#endif

    PreOptimizer preOpt(*ipData_);
    preOpt.handleCFGDDG(cfg, *ddg);

    cfg.optimizeBBOrdering(true, irm, ddg);

#ifdef WRITE_DDG_DOTS
    ddg->writeToDotFile(fnName + "_ddg2.dot");
#endif

    CycleLookBackSoftwareBypasser bypasser;
    CopyingDelaySlotFiller dsf;
    BBSchedulerController bbsc(*ipData_, &bypasser, &dsf);
    bbsc.handleCFGDDG(cfg, *ddg, *mach_ );

#ifdef WRITE_CFG_DOTS
    cfg.writeToDotFile(fnName + "_cfg2.dot");
#endif
#ifdef WRITE_DDG_DOTS
    ddg->writeToDotFile(fnName + "_ddg3.dot");
#endif

    if (!modifyMF_) {
        // BBReferences converted to Inst references
        // break LLVM->POM ->LLVM chain because we
        // need the BB refs to rebuild the LLVM CFG 
        cfg.convertBBRefsToInstRefs(irm);        
    }

    if (!functionAtATime_) {
        // TODO: make DS filler work with FAAT
        dsf.fillDelaySlots(cfg, *ddg, *mach_, *umach_, true);
    }

#ifdef WRITE_DDG_DOTS
    ddg->writeToDotFile(fnName + "_ddg4.dot");
#endif

#ifdef WRITE_CFG_DOTS
    cfg.writeToDotFile(fnName + "_cfg3.dot");
#endif
    delete ddg;    
}


TTAProgram::Terminal*
LLVMTCEIRBuilder::createMBBReference(const MachineOperand& mo) {
    MachineBasicBlock* mbb = mo.getMBB();
    std::map<const MachineBasicBlock*,BasicBlockNode*>::iterator i = 
        bbMapping_.find(mbb);
    
    if (i == bbMapping_.end()) {
        std::map<const MachineBasicBlock*, BasicBlockNode*>::iterator j = 
            skippedBBs_.find(mbb);
        if (j == skippedBBs_.end()) {
            mo.getParent()->dump();
            assert(j != skippedBBs_.end());
        }        
        return new TTAProgram::TerminalBasicBlockReference(
            j->second->basicBlock());
    }

    return new TTAProgram::TerminalBasicBlockReference(
        i->second->basicBlock());
}

TTAProgram::Terminal*
LLVMTCEIRBuilder::createSymbolReference(const TCEString& symbolName) {
    return new TTAProgram::TerminalSymbolReference(symbolName);
}

bool 
LLVMTCEIRBuilder::isRealInstruction(const MachineInstr& instr) {
#ifdef LLVM_2_9    
    const llvm::TargetInstrDesc* opDesc = &instr.getDesc();
#else
    const llvm::MCInstrDesc* opDesc = &instr.getDesc();
#endif
    if (opDesc->isReturn()) {
        return true;
    }

    // when the -g option turn on, this will come up opc with this, therefore
    // add this to ignore however, it is uncertain whether the debug "-g" will
    // generate more opc, need to verify
    if (opDesc->getOpcode() == TargetOpcode::DBG_VALUE) {
        return false;
    }	

    std::string opName = operationName(instr);

    // Pseudo instructions don't require any actual instructions.
    if (opName == "PSEUDO") {
        return false;
    }
    return true;
}

bool 
LLVMTCEIRBuilder::hasRealInstructions(
    MachineBasicBlock::const_iterator i, 
    const MachineBasicBlock& mbb) {
    for (; i != mbb.end(); i++) {
        if (isRealInstruction(*i)) {
            return true;
        }
    }
    return false;
}

bool
LLVMTCEIRBuilder::doInitialization(Module& m) {
    LLVMTCEBuilder::doInitialization(m);
    return false;
}

bool
LLVMTCEIRBuilder::doFinalization(Module& m) { 

    LLVMTCEBuilder::doFinalization(m);
    prog_->convertSymbolRefsToInsRefs();
    return false; 
}

TCEString 
LLVMTCEIRBuilder::operationName(const MachineInstr& mi) const {
    if (dynamic_cast<const TCETargetMachine*>(&targetMachine()) 
        != NULL) {
        return dynamic_cast<const TCETargetMachine&>(targetMachine())
            .operationName(mi.getDesc().getOpcode());
    } else {
        return mi.getDesc().getName();
    }
}

TCEString
LLVMTCEIRBuilder::registerFileName(unsigned llvmRegNum) const { 
    if (isTTATarget()) {
        return dynamic_cast<const TCETargetMachine&>(
            targetMachine()).rfName(llvmRegNum); 
    } else {
        // LLVM does not support explicit register file info
        // at the moment, so we assume there's only one reg file
        // in the machine. Pick the first one that is not
        // a 1-bit reg file.
        const TTAMachine::Machine::RegisterFileNavigator rfNav =
            mach_->registerFileNavigator();

        for (int i = 0; i < rfNav.count(); i++) {
            const TTAMachine::RegisterFile& rf = *rfNav.item(i);
            if (rf.width() > 1) 
                return rf.name();
        }
        abortWithError(
            TCEString("Unable to figure the RF for llvm reg num ") <<
            llvmRegNum);
        
    }
}

int
LLVMTCEIRBuilder::registerIndex(unsigned llvmRegNum) const {
    if (isTTATarget()) {
        return dynamic_cast<const TCETargetMachine&>(
            targetMachine()).registerIndex(llvmRegNum); 
    } else {
        /* Assume for non-TTA targets the register index
           is the final and correct one and that there's only
           one register file. With TTA we have to do conversion 
           due to the multiple register files option which LLVM 
           does not support. */
        // LLVM index registers starting from 1, we start ours from 0
        // decrease index to avoid out of range error.
        return llvmRegNum - 1;
    }
}

void
LLVMTCEIRBuilder::markJumpTableDestinations(
    llvm::MachineFunction& mf,
    ControlFlowGraph& cfg) {
    llvm::MachineJumpTableInfo* jtInfo = mf.getJumpTableInfo();
    if (jtInfo == NULL || jtInfo->isEmpty()) {
        return;
    } 

    std::vector<llvm::MachineJumpTableEntry> entries = jtInfo->getJumpTables();
    jumpTableRecord_.clear();
    for (unsigned int i = 0; i < entries.size(); i++) {
        std::vector<BasicBlockNode*> nodes;
        jumpTableRecord_.push_back(nodes);
        std::vector<MachineBasicBlock*> blocks =
            entries.at(i).MBBs;
        for (unsigned j = 0; j < blocks.size(); j++) {
            MachineBasicBlock* mbb = blocks.at(j);
            BasicBlockNode* bbn = NULL;
            std::map<const MachineBasicBlock*, BasicBlockNode*>::iterator
                bbMapIter = bbMapping_.find(mbb);
            assert(bbMapIter != bbMapping_.end() && 
                "The Basic Block Node for Machine Basic Block is missing!");
            bbn = bbMapIter->second;
            jumpTableRecord_.at(i).push_back(bbn);
        }
    }
   
}   

void
LLVMTCEIRBuilder::fixJumpTableDestinations(
    llvm::MachineFunction& mf,
    ControlFlowGraph& cfg) {
    
    llvm::MachineJumpTableInfo* jtInfo = mf.getJumpTableInfo();
    if (jtInfo == NULL) {
        return;
    }     
    for (unsigned int i = 0; i < jumpTableRecord_.size(); i++) {
        std::vector<BasicBlockNode*> nodes = jumpTableRecord_.at(i);
        std::vector<MachineBasicBlock*> oldTable = 
            jtInfo->getJumpTables().at(i).MBBs;
        for (unsigned int j = 0; j < nodes.size(); j++) {
            const BasicBlockNode* bbn = nodes.at(j);
            MachineBasicBlock* newMBB = &cfg.getMBB(mf, bbn->basicBlock());
            MachineBasicBlock* oldMBB = oldTable.at(j);
            jtInfo->ReplaceMBBInJumpTable(i, oldMBB, newMBB);
            // Slight cheating to force LLVM to emit machine basic block label
            // to avoid missing references from Jump Table Records to basic
            // blocks. TODO: Proper fix is needed.
            newMBB->setIsLandingPad();                        
        }
    }
}

}
