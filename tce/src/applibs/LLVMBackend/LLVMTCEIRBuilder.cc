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
 * @file LLVMTCEIRBuilder.hh
 *
 * This builder builds a CFG and DDG from the new LLVM TTA backend format.
 *
 * @author Heikki Kultala 2011
 * @author Henry Linjamäki 2017 (henry.linjamaki-no.spam-tut.fi)
 * @note reting: red
 */

#ifdef NDEBUG
#undef NDEBUG
#endif

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")

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
#include "SimpleIfConverter.hh"
#include "Peel2BBLoops.hh"
#include "passes/InnerLoopFinder.hh"
#include "Machine.hh"
#include "InstructionReferenceManager.hh"
#include "Program.hh"
#include "RegisterCopyAdder.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "Instruction.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "BasicBlock.hh"
#include "Move.hh"
#include "MapTools.hh"
#include "PRegionMarkerAnalyzer.hh"
#include "PostpassOperandSharer.hh"
#include "CallsToJumps.hh"
#include "AbsoluteToRelativeJumps.hh"

#include <stdlib.h>
#include <llvm/ADT/SmallString.h>
#include <llvm/MC/MCContext.h>
#include <llvm/MC/MCSymbol.h>
#include <llvm/CodeGen/MachineJumpTableInfo.h>
#include <llvm/IR/Value.h>
#include <llvm/CodeGen/MachineMemOperand.h>
#include "llvm/Analysis/AliasAnalysis.h"

POP_COMPILER_DIAGS

#define EXIT_IF_THROWS(__X__)                               \
    try {                                                   \
        __X__;                                              \
    } catch (const Exception& e) {                          \
        Application::errorStream()                          \
            << "Error: " << e.errorMessage() << std::endl;  \
        exit(1);                                            \
    }


//#define WRITE_DDG_DOTS
//#define WRITE_CFG_DOTS

namespace llvm {

char LLVMTCEIRBuilder::ID = -1;

LLVMTCEIRBuilder::LLVMTCEIRBuilder(
    const llvm::TargetMachine& tm, TTAMachine::Machine* mach, 
    InterPassData& ipd, AliasAnalysis* AA, bool functionAtATime, 
    bool modifyMF) :
    LLVMTCEBuilder(tm, mach, ID, functionAtATime), ipData_(&ipd), 
    ddgBuilder_(ipd), AA_(AA), modifyMF_(modifyMF),
    scheduler_(NULL), dsf_(NULL),
    bypasser_(NULL), loopFinder_(NULL) {
    RegisterCopyAdder::findTempRegisters(*mach, ipd);

    if (functionAtATime_) {
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

    } 
    delaySlotFilling_ = !options_->disableDelaySlotFiller();
}

void
LLVMTCEIRBuilder::getAnalysisUsage(AnalysisUsage &AU) const {
    LLVMTCEBuilder::getAnalysisUsage(AU);
}

bool
LLVMTCEIRBuilder::writeMachineFunction(MachineFunction& mf) {

    if (tm_ == NULL)
        tm_ = &mf.getTarget();

    clearFunctionBookkeeping();

    curFrameInfo_ = &mf.getFrameInfo();
    assert(curFrameInfo_ != NULL);

    if (!functionAtATime_) {
        // ensure data sections have been initialized when compiling
        // the whole program at a time
        initDataSections();
        emitConstantPool(*mf.getConstantPool());    
    } else {
        mang_ = new llvm::Mangler();
    }

    // omit empty functions..
    if (mf.begin() == mf.end()) return true;   

    TTAMachine::AddressSpace* as = mach_->controlUnit()->addressSpace();
    
    SmallString<256> Buffer;
    mang_->getNameWithPrefix(Buffer, &mf.getFunction(), false);
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
    cfg->setInstructionReferenceManager(*irm);
#ifdef WRITE_CFG_DOTS
    cfg->writeToDotFile(fnName + "_cfg1.dot");
#endif

    bool fastCompilation = options_ != NULL && options_->optLevel() == 0;

    markJumpTableDestinations(mf, *cfg);

    if (!mach_->controlUnit()->hasOperation("call")) {
        CallsToJumps ctj(*ipData_);
        ctj.handleControlFlowGraph(*cfg, *mach_);
    }

    if (fastCompilation || !isHotFunction(mf)) {
        verboseLog(TCEString("###      compiling (fast): ") + fnName);
        EXIT_IF_THROWS(compileFast(*cfg));
    } else {
        verboseLog(TCEString("### compiling (optimized): ") + fnName);
        AliasAnalysis* AA = NULL;
        if (!AA_) {
            // Called through LLVMBackend. We are actual module and 
            // can get previous pass analysis!
            AAResultsWrapperPass* AARWPass =
                getAnalysisIfAvailable<AAResultsWrapperPass>();
            if (AARWPass)
                AA = &AARWPass->getAAResults();
        } else {
            // Called through LLVMTCEScheduler. We are not registered
            // module in pass manager, so we do not have previous
            // pass analysis data, but LLVMTCEScheduler kindly
            // got them for us and passed through.        
            AA = AA_;
        }
        EXIT_IF_THROWS(compileOptimized(*cfg, AA));
    }

    if (!modifyMF_) {
        cfg->convertBBRefsToInstRefs();
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

    AbsoluteToRelativeJumps jumpConv(*ipData_);
    jumpConv.handleProcedure(*procedure, *mach_);

    if (Application::verboseLevel() > 0 && spillMoveCount_ > 0) {
        Application::logStream() 
            << "spill moves in " << 
            (std::string)(mf.getFunction().getName()) << ": "
            << spillMoveCount_ << std::endl;
    }

    delete cfg;
    if (functionAtATime_) delete irm;
    return false;
}

/**
 * Returns false in case the given function should be compiled with
 * fast settings without optimizations.
 *
 * This looks into the tcecc --primary-functions function list if it's
 * set. If not set, assumes all functions are "hot".
 */
bool
LLVMTCEIRBuilder::isHotFunction(llvm::MachineFunction& mf) const {

    if (options_ == NULL) return true;

    FunctionNameList* funcs = options_->primaryFunctions();

    if (funcs == NULL || funcs->size() == 0)
        return true;

    SmallString<256> Buffer;
    mang_->getNameWithPrefix(Buffer, &mf.getFunction(), false);
    TCEString fnName(Buffer.c_str());
    return AssocTools::containsKey(*funcs, fnName);
}

ControlFlowGraph*
LLVMTCEIRBuilder::buildTCECFG(llvm::MachineFunction& mf) {

    SmallString<256> Buffer;
    mang_->getNameWithPrefix(Buffer, &mf.getFunction(), false);
    TCEString fnName(Buffer.c_str());

    ControlFlowGraph* cfg = new ControlFlowGraph(fnName, prog_);

    bbMapping_.clear();
    skippedBBs_.clear();

    // TODO: these maps/sets with pointer keys are possible source of
    // indeterminism.
    std::set<const MachineBasicBlock*> endingCallBBs;
    std::set<const MachineBasicBlock*> endingCondJumpBBs;
    std::set<const MachineBasicBlock*> endingUncondJumpBBs;
    std::set<const MachineBasicBlock*> endingInlineAsmBBs;
    std::map<const BasicBlockNode*, BasicBlockNode*> callSuccs;
    std::map<const BasicBlockNode*, const MachineBasicBlock*> condJumpSucc;
    std::map<const BasicBlockNode*, BasicBlockNode*> ftSuccs;
    // This holds BB -> inlineAsmBB mapping.
    std::map<const BasicBlockNode*, BasicBlockNode*> ftSuccsToInlineAsm;
    // This holds inlineAsmBB -> BB and inlineAsmBB -> inlineAsmBB mapping.
    std::map<const BasicBlockNode*, BasicBlockNode*> inlineAsmSuccs;
    std::set<const MachineBasicBlock*> emptyMBBs;
    std::map<const BasicBlockNode*, bool> bbPredicates;
    ControlFlowGraph::NodeSet returningBBs;

    BasicBlockNode* entry = new BasicBlockNode(0, 0, true);
    cfg->addNode(*entry);
    bool firstInsOfProc = true;

    pregions_ = new PRegionMarkerAnalyzer(mf);

    // 1st loop create all BB's. do not fill them yet.
    for (MachineFunction::const_iterator i = mf.begin(); i != mf.end(); i++) {
        const MachineBasicBlock& mbb = *i;
        //TODO: what does the parameter do? start address?
        TTAProgram::BasicBlock* bb = new TTAProgram::BasicBlock(0);

        // this doesn't seem robust: it assumes _start is added to
        // the program first and the BBs added in their program
        // order
        bool firstBBofTheProgram = 
            !functionAtATime_ && prog_->procedureCount() == 1 && 
            cfg->nodeCount() == 1;
        // first BB of the program
        if (firstBBofTheProgram) {
            LLVMTCEBuilder::emitSPInitialization(*bb);
        }
        
        TCEString bbName = mbbName(mbb);
        BasicBlockNode* bbn = new BasicBlockNode(*bb);
        bbn->setBBOwnership(true);

        // if the basic block is detected as an inner loop basic block,
        // transform the info to the built TCE CFG
        if (loopFinder_ != NULL) {
            InnerLoopFinder::InnerLoopInfoIndex loopInfos =
                loopFinder_->innerLoopInfo();

            InnerLoopFinder::InnerLoopInfoIndex::const_iterator loopInfoI = 
                loopInfos.find(mbb.getBasicBlock());
            if (loopInfoI != loopInfos.end()) {
                InnerLoopFinder::InnerLoopInfo loopInfo = 
                    (*loopInfoI).second;
                bb->setInInnerLoop();
                if (loopInfo.isTripCountKnown()) {               
                    bb->setTripCount(loopInfo.tripCount());
                }
            } 
        }
        
        bool newMBB = true;
        bool newBB = true;
        unsigned realInstructionCount = 0;
        bool lastInstrWasInlineAsm = false;

        // 1st loop: create all BB's. Do not fill them with instructions.
        for (MachineBasicBlock::const_iterator j = mbb.begin();
             j != mbb.end(); j++) {

            if (!isRealInstruction(*j)) {
                continue;
            }

            // Put all instances of inline asm blocks to own BB.
            // Split BB if there is instructions already added to BB.
            if (isInlineAsm(*j)) {
                // TODO check if inline asm does not have any instructions?
                // Split BB before the inline asm unless first instruction in
                // BB.
                if (realInstructionCount > 0 && !lastInstrWasInlineAsm) {
                    bb = new TTAProgram::BasicBlock(0);
                    BasicBlockNode* succBBN = new BasicBlockNode(*bb);
                    ftSuccsToInlineAsm[bbn] = succBBN;
                    bbn = succBBN;
                    newBB = true;
                }
                lastInstrWasInlineAsm = true;
            }

            if (newBB) {
                realInstructionCount = 0;
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
            realInstructionCount++;

            // Put all instances of INLINEASM nodes to own BB.
            // This one "closes" inline asm BB and creates new BB for rest
            // of the instructions or next inline asm block.
            if (isInlineAsm(*j)) {
                // New BB after inline asm.
                MachineBasicBlock::const_iterator afterInlineAsm = j;
                ++afterInlineAsm;
                if (hasRealInstructions(afterInlineAsm, mbb)) {
                    bb = new TTAProgram::BasicBlock(0);
                    BasicBlockNode* succBBN = new BasicBlockNode(*bb);
                    inlineAsmSuccs[bbn] = succBBN;
                    bbn = succBBN;
                    newBB = true;
                } else {
                    endingInlineAsmBBs.insert(&mbb);
                }
                continue;
            }
            lastInstrWasInlineAsm = false;

            if (j->getDesc().isCall() || isExplicitReturn(*j)) {
                if (j->getDesc().isCall() && j->getOperand(0).isGlobal()) {
                    // If it's a direct call (not via function pointer),
                    // check that the called function is defined. At this
                    // point we should have a fully linked program.
                    const Function* callee = 
                        dyn_cast<Function>(j->getOperand(0).getGlobal());
                    assert(callee != NULL);
                    if (callee->size() == 0) {
                        TCEString errorMsg = 
                            "error: call to undefined function '";
                        errorMsg << callee->getName().str() << "'.";
                        throw CompileError(
                            __FILE__, __LINE__, __func__, errorMsg);
                    }
                }
                // if last ins of bb is call, no need to create new bb.
                if (&(*j) == &(mbb.back())) {
                    endingCallBBs.insert(&(*i));
                } else {
                    MachineBasicBlock::const_iterator afterCall = j;
                    ++afterCall;
                    if (!hasRealInstructions(afterCall, mbb)) {
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
                continue;
            }
            // also need to split BB on cond branch.
            // LLVM BB may contain 2 branches.
            if (j->getDesc().isBranch()) {
                TCEString opName = operationName(*j);
                bool pred = false;
                if (j->getDesc().isConditionalBranch()) {
                    if (opName == "?jump") pred = true;
                    if (opName == "BNZ") pred = true;
                    if (opName == "BNZ1") pred = true;
                    // TODO: Should this not have BZ/BZ1?
                    if (opName == "BEQ") pred = true;
                    if (opName == "BNE") pred = true;
                    if (opName == "BGT") pred = true;
                    if (opName == "BGTU") pred = true;
                    if (opName == "BLT") pred = true;
                    if (opName == "BLTU") pred = true;
                    if (opName == "BLE") pred = true;
                    if (opName == "BLEU") pred = true;
                    if (opName == "BGE") pred = true;
                    if (opName == "BGEU") pred = true;

                    // TODO: what is the meaning of this?
                    if (opName.find("+") != std::string::npos) pred = true;
                    bbPredicates[bbn] = pred;
                    const MachineOperand& mo = j->getOperand(j->getNumOperands()-1);
                    assert(mo.isMBB());
                    condJumpSucc[bbn] = mo.getMBB();

                    if (&(*j) == &(mbb.back())) {
                        endingCondJumpBBs.insert(&(*i));
                    } else {
                        if (!hasRealInstructions(j, mbb)) {
                            endingCondJumpBBs.insert(&(*i));
                        } else {
                            // create a new BB for the code after the conditional branch.
                            // this should only contain one uncond jump.
                            bb = new TTAProgram::BasicBlock(0);
                            BasicBlockNode* succBBN = new BasicBlockNode(*bb);
                            succBBN->setBBOwnership(true);
                            ftSuccs[bbn] = succBBN;
                            bbn = succBBN;
                            newBB = true;
                        }
                    }
                } else {
                    // has to be uncond jump, and last ins of bb.
                    if (&(*j) != &(mbb.back())) {
                        Application::logStream() << " not at the end of ";
                        if (j->getDesc().isBranch())
                            Application::logStream() << " is branch";
                        abortWithError("Jump was not last ins of BB.");
                    }
                    endingUncondJumpBBs.insert(&(*i));
                }
                continue;
            }
        } // for loop
        if (newMBB) {
            assert(newBB);
            emptyMBBs.insert(&mbb);
        }
        if (newBB) {
            assert(firstBBofTheProgram || bb->instructionCount() == 0);
            delete bbn;
        }
    }

    TTAProgram::InstructionReferenceManager* irm = NULL;
    if (functionAtATime_) {
        irm = new TTAProgram::InstructionReferenceManager();
    } else {
        irm = &prog_->instructionReferenceManager();
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

            if (isInlineAsm(*j)) {
                if (AssocTools::containsKey(ftSuccsToInlineAsm, bbn)) {
                    bbn = ftSuccsToInlineAsm[bbn];
                    bb = &bbn->basicBlock();
                }
                emitInlineAsm(mf, &*j, bb, *irm);
                bbn->setScheduled(true);
                if (AssocTools::containsKey(inlineAsmSuccs, bbn)) {
                    bbn = inlineAsmSuccs[bbn];
                    bb = &bbn->basicBlock();
                }
                continue;
            }

            TTAProgram::Instruction* instr = NULL;
            instr = emitInstruction(&*j, bb);

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
            
            // basic blocks that contain return instruction will have jump
            // edge to exit node in cfg, not callpass edge.
            if (j->getDesc().isReturn()) {
                returningBBs.insert(bbn);
            }

            // if a call or an explicit return instruction,
            // or an unconditional jump from inline asm, switch to the
            // next TCE bb (in callsucc chain) that was created in the
            // previous loop
            if ((j->getDesc().isCall() || isExplicitReturn(*j) ||
                 (!j->getDesc().isBranch() && instr->hasControlFlowMove())) &&
                &(*j) != &(mbb.back())) {
                if (!AssocTools::containsKey(callSuccs, bbn)) {
                    // the call ends the basic block, after this only at most
                    // "non real" instructions (such as debug metadata), which
                    // we can simply ignore
                    break;
                }
                bbn = callSuccs[bbn];
                bb = &bbn->basicBlock();
            }
            
            // conditional jump or indirect jump that is not last ins splits 
            // a bb.
            if (j->getDesc().isBranch() && 
                &(*j) != &(mbb.back())) {
                bbn = ftSuccs[bbn];
                bb = &bbn->basicBlock();
            }
        }

        assert(bb->instructionCount() != 0);
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
            std::map<const BasicBlockNode*, BasicBlockNode*>::iterator l =
                inlineAsmSuccs.find(bbn);
            std::map<const BasicBlockNode*, BasicBlockNode*>::iterator m =
                ftSuccsToInlineAsm.find(bbn);

            // BB should not have 2+ fallthrough successors.
            assert(((j != callSuccs.end())
                + (k != ftSuccs.end())
                + (l != inlineAsmSuccs.end())
                + (m != ftSuccsToInlineAsm.end())) < 2);

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

            if (l != inlineAsmSuccs.end()) {
                const BasicBlockNode* inlineAsmSucc = l->second;
                assert(inlineAsmSucc);
                ControlFlowEdge* cfe = new ControlFlowEdge(
                    ControlFlowEdge::CFLOW_EDGE_NORMAL,
                    ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH);
                cfg->connectNodes(*bbn, *inlineAsmSucc, *cfe);
                bbn = inlineAsmSucc;
                jumpSucc = condJumpSucc[bbn];
                continue;
            }

            if (m != ftSuccsToInlineAsm.end()) {
                const BasicBlockNode* inlineAsmPred = m->second;
                assert(inlineAsmPred);
                ControlFlowEdge* cfe = new ControlFlowEdge(
                    ControlFlowEdge::CFLOW_EDGE_NORMAL,
                    ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH);
                cfg->connectNodes(*bbn, *inlineAsmPred, *cfe);
                bbn = inlineAsmPred;
                jumpSucc = condJumpSucc[bbn];
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

    cfg->setInstructionReferenceManager(*irm);
    // add back edge properties.
    cfg->detectBackEdges();

    /* Split BBs with calls inside. These can be produced 
       from expanding  the pseudo asm blocks. Currently at least 
       the call_global_[cd]tors expands to multiple calls to the
       global object constructors and destructors. */
    cfg->splitBasicBlocksWithCallsAndRefs();
    fixProgramOperationReferences();
    
    // create jumps to exit node
    cfg->addExit(returningBBs);

    delete pregions_;
    pregions_ = NULL;
    
    // add back edge properties.
    cfg->detectBackEdges();
    //cfg->writeToDotFile(fnName + ".cfg.dot");
    return cfg;
}

/**
 * Returns true in case the given MI is an explict return instruction generated
 * from the pseudo assembly string ".return_to".
 */
bool
LLVMTCEIRBuilder::isExplicitReturn(const llvm::MachineInstr& mi) const {

    if (!mi.isInlineAsm()) return false;
    
    // Copied from LLVM's AsmPrinterInlineAsm.cpp. There doesn't
    // seem to be a cleaner way to get the inline assembly text
    // but this hack.
    unsigned numOperands = mi.getNumOperands();

    // Count the number of register definitions to find the asm string.
    unsigned numDefs = 0;
    for (; mi.getOperand(numDefs).isReg() && 
             mi.getOperand(numDefs).isDef();
         ++numDefs);
          
    TCEString asmStr = "";
    if (mi.isInlineAsm() && numDefs < numOperands &&
        mi.getOperand(numDefs).isSymbol()) {
        asmStr = mi.getOperand(numDefs).getSymbolName();
    }
    // The .return_to pseudo assembly is used in threading code
    // to switch execution to another thread when returning from
    // the current function. A very special case.
    return asmStr.startsWith(".return_to ") ||
        asmStr == ".longjmp";
}

void
LLVMTCEIRBuilder::compileFast(ControlFlowGraph& cfg) {
    SequentialScheduler sched(*ipData_);
    sched.handleControlFlowGraph(cfg, *mach_);
}

BBSchedulerController&
LLVMTCEIRBuilder::scheduler() {
    if (scheduler_ == NULL) {
        bypasser_ = new CycleLookBackSoftwareBypasser;
        // disabled for the LLVM->TCE->LLVM scheduling chain as
        // it crashes
        CopyingDelaySlotFiller* dsf = NULL;
        if (!modifyMF_ && delaySlotFilling_) 
            dsf = &delaySlotFiller();
        scheduler_ = 
            new BBSchedulerController(*mach_, *ipData_, bypasser_, dsf);
    } 
    return *scheduler_;
}

CopyingDelaySlotFiller&
LLVMTCEIRBuilder::delaySlotFiller() {
    if (dsf_ == NULL)
        dsf_ = new CopyingDelaySlotFiller;
    return *dsf_;    
}

void
LLVMTCEIRBuilder::compileOptimized(
    ControlFlowGraph& cfg, 
    llvm::AliasAnalysis* llvmAA) {

    SimpleIfConverter ifConverter(*ipData_, *mach_);
    ifConverter.handleControlFlowGraph(cfg, *mach_);
    Peel2BBLoops peel2bbLoops(*ipData_, *mach_);
    peel2bbLoops.handleControlFlowGraph(cfg, *mach_);

#if 0
    SchedulerCmdLineOptions* options =
        dynamic_cast<SchedulerCmdLineOptions*>(
            Application::cmdLineOptions());
#endif

    // TODO: on trunk single bb loop(swp), last param true(rr, threading)
    DataDependenceGraph* ddg = ddgBuilder_.build(
        cfg,
        DataDependenceGraph::INTRA_BB_ANTIDEPS, *mach_,
        NULL, true, true, llvmAA);

    TCEString fnName = cfg.name();
#ifdef WRITE_DDG_DOTS
    ddg->writeToDotFile(cfg.name() + "_ddg1.dot");
#endif
    cfg.optimizeBBOrdering(true, cfg.instructionReferenceManager(), ddg);

    PreOptimizer preOpt(*ipData_);
    preOpt.handleCFGDDG(cfg, *ddg);

    cfg.optimizeBBOrdering(true, cfg.instructionReferenceManager(), ddg);

#ifdef WRITE_DDG_DOTS
    ddg->writeToDotFile(cfg.name() + "_ddg2.dot");
#endif

    if (!modifyMF_) {
        // BBReferences converted to Inst references
        // break LLVM->POM ->LLVM chain because we
        // need the BB refs to rebuild the LLVM CFG 
        cfg.convertBBRefsToInstRefs();
    }

    if (delaySlotFilling_)
        delaySlotFiller().initialize(cfg, *ddg, *mach_);
    scheduler().handleCFGDDG(cfg, ddg, *mach_ );

#ifdef WRITE_CFG_DOTS
    fnName = cfg.name();
    cfg.writeToDotFile(fnName + "_cfg2.dot");
#endif
#ifdef WRITE_DDG_DOTS
    ddg->writeToDotFile(fnName + "_ddg3.dot");
#endif

    if (!functionAtATime_) {
        // TODO: make DS filler work with FAAT
        // sched yield emitter does not work with the delay slot filler

        if (delaySlotFilling_) {
            delaySlotFiller().fillDelaySlots(cfg, *ddg, *mach_);
        } 
    }

#ifdef WRITE_CFG_DOTS
    cfg.writeToDotFile(fnName + "_cfg3.dot");
#endif

    PostpassOperandSharer ppos(*ipData_, cfg.instructionReferenceManager());
    ppos.handleControlFlowGraph(cfg, *mach_);

#ifdef WRITE_DDG_DOTS
    ddg->writeToDotFile(fnName + "_ddg4.dot");
#endif

#ifdef WRITE_CFG_DOTS
    cfg.writeToDotFile(fnName + "_cfg4.dot");
#endif
    delete ddg;    
}


TTAProgram::Terminal*
LLVMTCEIRBuilder::createMBBReference(const MachineOperand& mo) {
    if (mo.isBlockAddress()) {
        TTAProgram::BasicBlock* bb = NULL;
        const MachineBasicBlock* mbb = NULL;

        std::map<const MachineBasicBlock*, BasicBlockNode*>::iterator i =
            bbMapping_.begin();
        for (; i != bbMapping_.end(); ++i) {
            const MachineBasicBlock* mbbt = i->first;
            TTAProgram::BasicBlock& bbt = i->second->basicBlock();
            if (mbbt->getBasicBlock() == mo.getBlockAddress()->getBasicBlock()) {
                if (bb != NULL) {
#if 0
                    Application::logStream() 
                        << "LLVMTCEIRBuilder: found multiple potential BB references."
                        << std::endl;
                    Application::logStream() 
                        << "first: " << bb->toString() << std::endl;
                    Application::logStream()
                        << "another: " << bbt.toString() << std::endl;
#endif
                    // in case the original BB is split to multiple machine BBs,
                    // refer to the first one in the chain because the original
                    // BB reference could not have referred to middle of an BB
                    if (mbbt->isSuccessor(mbb)) {
                        bb = &bbt;
                        mbb = mbbt;
                    }
                } else {
                    bb = &bbt;
                    mbb = mbbt;
                }
            } 
        }

        i = skippedBBs_.begin();
        for (; i != skippedBBs_.end(); ++i) {
            const MachineBasicBlock* mbbt = i->first;
            TTAProgram::BasicBlock& bbt = i->second->basicBlock();
            if (mbbt->getBasicBlock() == mo.getBlockAddress()->getBasicBlock()) {
                assert (bb == NULL);
                bb = &bbt;
            } 
        }

        if (bb == NULL) {
            Application::logStream() 
                << "Could not find referred MBB matching the referred BB:"
                << std::endl;
            assert (bb != NULL);
        }
        return new TTAProgram::TerminalBasicBlockReference(*bb);
    }
    MachineBasicBlock* mbb = mo.getMBB();
    std::map<const MachineBasicBlock*,BasicBlockNode*>::iterator i = 
        bbMapping_.find(mbb);
    
    if (i == bbMapping_.end()) {
        std::map<const MachineBasicBlock*, BasicBlockNode*>::iterator j = 
            skippedBBs_.find(mbb);
        if (j == skippedBBs_.end()) {
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
LLVMTCEIRBuilder::isRealInstruction(const MachineInstr& instr) const {
    const llvm::MCInstrDesc* opDesc = &instr.getDesc();
    if (opDesc->isReturn()) {
        return true;
    }

    // when the -g option turn on, this will come up opc with this, therefore
    // add this to ignore however, it is uncertain whether the debug "-g" will
    // generate more opc, need to verify
    if (opDesc->getOpcode() == TargetOpcode::DBG_VALUE) {
        return false;
    }	

    if (opDesc->getOpcode() == TargetOpcode::KILL) {
        return false;
    }

    std::string opName = operationName(instr);

    // Pseudo instrs or debug labels don't require any actual instructions.
    if (opName == "PSEUDO" || opName == "DEBUG_LABEL") {
        return false;
    }

    if (opName == "MOVE") {
        const MachineOperand& dst = instr.getOperand(0);
        const MachineOperand& src = instr.getOperand(1);
        if (dst.isReg() && src.isReg() && dst.getReg() == src.getReg()) {
            return false;
        }
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

    // Catch the exception here as throwing exceptions
    // through library boundaries is flaky. It crashes 
    // on x86-32 Linux at least. See:
    // https://bugs.launchpad.net/tce/+bug/894816
    EXIT_IF_THROWS(LLVMTCEBuilder::doFinalization(m));
    EXIT_IF_THROWS(prog_->convertSymbolRefsToInsRefs());

    // The Program can now have a bunch of unscheduled Procedures
    // created by the SchedYieldEmitter. Schedule them now.
    for (int p = 0; p < prog_->procedureCount(); ++p) {
        TTAProgram::Procedure& procedure = prog_->procedure(p);
        // assume all functions that were not in the original LLVM
        // module are new ones that need to be scheduled
        
        if (m.getFunction(procedure.name()) != NULL) continue;
        scheduler().handleProcedure(procedure, *mach_);
    }
    return false; 
}

TCEString 
LLVMTCEIRBuilder::operationName(const MachineInstr& mi) const {
    if (dynamic_cast<const TCETargetMachine*>(&targetMachine()) 
        != NULL) {
        return dynamic_cast<const TCETargetMachine&>(targetMachine())
            .operationName(mi.getDesc().getOpcode());
    } else {
        return targetMachine().getSubtargetImpl(
            mi.getParent()->getParent()->getFunction())->getInstrInfo()->
            getName(mi.getOpcode()).str();
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
    ControlFlowGraph&) {

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
            newMBB->setIsEHPad();
        }
    }
}

/**
 * Create MoveNode and attach it to TerminalFUs.
 * The MoveNode will be owned by DDG.
 */
void 
LLVMTCEIRBuilder::createMoveNode(
    ProgramOperationPtr& po,
    std::shared_ptr<TTAProgram::Move> m,
    bool isDestination) {
    MoveNode* mn = new MoveNode(m);

    if (isDestination) {
        po->addInputNode(*mn);
        mn->addDestinationOperationPtr(po);
        TTAProgram::TerminalFUPort& term =
            dynamic_cast<TTAProgram::TerminalFUPort&>(m->destination());
        term.setProgramOperation(po);
    } else {
        po->addOutputNode(*mn);
        mn->setSourceOperationPtr(po);
        TTAProgram::TerminalFUPort& term =
            dynamic_cast<TTAProgram::TerminalFUPort&>(m->source());
        term.setProgramOperation(po);
    }
}

LLVMTCEIRBuilder::~LLVMTCEIRBuilder() {

    LLVMTCECmdLineOptions* options = NULL;
    if (Application::cmdLineOptions() != NULL) {
        options = 
            dynamic_cast<LLVMTCECmdLineOptions*>(
                Application::cmdLineOptions());
        if (options->isVerboseSwitchDefined()) {
//            PostpassOperandSharer::printStats();
//            CycleLookBackSoftwareBypasser::printStats();
        }
    }

    delete scheduler_;
    delete bypasser_;
    delete dsf_;
}
}
