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
 * @file InnerLoopFinder.cc
 *
 * LLVM pass for finding inner loops in the code.
 *
 * @author 2008-2015 Pekka J‰‰skel‰inen
 */

#include <sstream>
#include <fstream>
#include <string>
#include <map>

#include "tce_config.h"

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")
IGNORE_COMPILER_WARNING("-Wcomment")

#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/Support/CommandLine.h>

#include "tce_config.h"

#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/Analysis/LoopInfo.h>
#include "llvm/Transforms/Scalar.h"
#ifndef LLVM_OLDER_THAN_7
#include "llvm/Transforms/Utils.h"
#endif
#ifndef LLVM_OLDER_THAN_11
#include <llvm/Pass.h>
#else
#include <llvm/PassSupport.h>
#endif
#include "llvm/CodeGen/Passes.h"
#include "InnerLoopFinder.hh"

#ifndef LLVM_OLDER_THAN_10
#include "llvm/InitializePasses.h"
#endif

using namespace llvm;

static llvm::cl::opt<bool>
DumpLoopInfo
("dump-loop-info", llvm::cl::init(false), llvm::cl::Hidden,
 llvm::cl::desc("Dump information about loops to files named [modulename].loopinfo.txt."));

static llvm::RegisterPass</*TCE::*/InnerLoopFinder> X(
    "find-innerloops-test", 
  "Finds inner loops test.", false, true);

INITIALIZE_PASS_BEGIN(
    InnerLoopFinder, "find-innerloops", 
    "Finds info of the inner loops in the program.",
    false, true)
INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(LoopSimplify)
#ifdef LLVM_OLDER_THAN_3_9
INITIALIZE_PASS_DEPENDENCY(LCSSA)
#else
INITIALIZE_PASS_DEPENDENCY(LCSSAWrapperPass)
#endif
INITIALIZE_PASS_END(
    InnerLoopFinder, "find-innerloops", 
    "Finds info of the inner loops in the program.",
    false, true)

// Publically exposed interface to pass...
//const llvm::PassInfo* InnerLoopFinderID = X.getPassInfo();

// this causes duplicate TCE namespaces , both global and
// llvm::TCE which break plugin build, so this taken out from
// tce namespace to avoid having it
//namespace TCE {

char InnerLoopFinder::ID = 0;

InnerLoopFinder::InnerLoopFinder() : 
    LoopPass(ID) {
    dump = DumpLoopInfo;
}

void
InnerLoopFinder::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
    AU.setPreservesCFG();
    AU.addRequiredID(llvm::LoopSimplifyID);
    AU.addRequiredID(llvm::LCSSAID);
    AU.addPreservedID(llvm::LCSSAID);
    AU.addRequired<llvm::LoopInfoWrapperPass>();

#ifdef LLVM_OLDER_THAN_3_8
    AU.addRequired<llvm::ScalarEvolution>();
#else
    AU.addRequired<llvm::ScalarEvolutionWrapperPass>();
#endif

    AU.addPreserved<llvm::LoopInfoWrapperPass>();

#ifdef LLVM_OLDER_THAN_3_8
    AU.addPreserved<llvm::ScalarEvolution>();
#else
    AU.addPreserved<llvm::ScalarEvolutionWrapperPass>();
#endif
}

InnerLoopFinder::~InnerLoopFinder() {
    // flush and close all loop info dump files
    if (dump) {
        for (DumpFileIndex::iterator i = dumpFiles.begin(); 
             i != dumpFiles.end(); ++i) {
            std::ostream* stream = (*i).second;
            stream->flush();
            delete stream;
        }
        dumpFiles.clear();
    }
}

/**
 * Returns a stream to dump the loop info to.
 */
std::ostream&
InnerLoopFinder::out(llvm::Loop* l) {

    assert(l != NULL);
    
    std::string moduleName = 
        l->getHeader()->getParent()->getParent()->getModuleIdentifier();
    std::string fName = moduleName + ".loopinfo.txt";
    if (moduleName == "<stdin>") {
        fName = "loopinfo.txt";
    }

    if (dumpFiles.find(moduleName) == dumpFiles.end()) {
        std::fstream* outStream = 
            new std::fstream(
                fName.c_str(),
                std::ios_base::out | std::ios_base::trunc);
        dumpFiles[moduleName] = outStream;
    }
    return *dumpFiles[moduleName];
}

/**
 * Returns a textual description for the given loop.
 */
std::string
InnerLoopFinder::loopDescription(llvm::Loop* l) {

    if (l == NULL)
        return "[none]";

    std::ostringstream ss;
    assert(l->getHeader() != NULL);
    assert(l->getHeader()->getParent() != NULL);
#ifdef LLVM_OLDER_THAN_11
    std::string curProcName = l->getHeader()->getParent()->getName();
#else
    std::string curProcName = l->getHeader()->getParent()->getName().str();
#endif
    ss << "in " << curProcName << "()";

    return ss.str();
}

/**
 * Saves trip counts of single basic block inner loops.
 */
bool
InnerLoopFinder::runOnLoop(llvm::Loop* l, llvm::LPPassManager&) {

    const std::vector<llvm::Loop*>& subLoops = l->getSubLoops();

    unsigned subLoopCount = subLoops.size();
    unsigned tripCount = getSmallConstantTripCount(l);
    unsigned bbCount = l->getBlocks().size();


    /// early exits?
	llvm::SmallVector<llvm::BasicBlock*, 10> exitingBlocks;

#if 0
    /*
      This caused an LLVM crash in test cases
      llvm-frontend/tests/{FloatEmulationTest,NewLib,softloat_lowering,HelloWorld}:

      /home/visit0r/src/llvm-2.3/include/llvm/Support/Casting.h:199: 
      typename llvm::cast_retty<To, From>::ret_type llvm::cast(const Y&) 
      [with X = llvm::BasicBlock, Y = llvm::Value*]: 
      Assertion `isa<X>(Val) && "cast<Ty>() argument of incompatible type!"' failed.      
     */
    l->getExitingBlocks(exitingBlocks);
    unsigned exitingBlockCount = exitingBlocks.size();
#endif

    unsigned callCount = 0;
    if (subLoopCount == 0) {
	for (llvm::LoopBase<llvm::BasicBlock, llvm::Loop>::
		 block_iterator bb = 
                 l->block_begin(); bb != l->block_end(); ++bb) {
            llvm::BasicBlock& basicBlock = **bb;
            for (llvm::BasicBlock::iterator i = basicBlock.begin();  
                 i != basicBlock.end(); ++i) {
                llvm::Instruction& instruction = *i;
                if (instruction.getOpcode() == llvm::Instruction::Call)
                    ++callCount;
            }
        }
    }

    const bool innerLoop = 
        bbCount == 1 && subLoopCount == 0 && callCount == 0;

    if (innerLoop) {
        if (dump && tripCount > 0) {
            out(l) << "found an inner loop with trip count " 
                   << tripCount << std::endl;
        }
        assert(l->getBlocks().size() > 0);

        InnerLoopInfo loopInfo(tripCount);
#ifdef LLVM_OLDER_THAN_6_0
        loopInfos_[l->getBlocks().at(0)] = loopInfo;
#else
        loopInfos_[l->getBlocks()[0]] = loopInfo;
#endif
    } 

    if (dump) {
        out(l) << "loop: " << loopDescription(l) << std::endl
               << "depth: " << l->getLoopDepth() << std::endl;
        out(l) << "sub loops: " << subLoopCount << std::endl;
        out(l) << "trip count: "; 
        if (tripCount != 0) {
            out(l) << "constant: " << tripCount;
        } else {
            out(l) << "unknown ";
        }
        out(l) << std::endl;
        out(l) << "basic blocks: " << bbCount << std::endl;
//        out(l) << "exiting blocks: " << exitingBlocks.size() << std::endl;
        if (subLoopCount == 0)
            out(l) << "calls: " << callCount << std::endl;
        out(l) << std::endl;
    }
    return false; // loop not modified
}

/**
 * Returns the trip count of the loop as a normal unsigned value, if 
 * possible. 
 *
 * Forwardported from LLVM 2.4. Returns 0 if the trip count is unknown
 * or not constant. Will also return 0 if the trip count is very large 
 * (>= 2^32).
 */
unsigned 
InnerLoopFinder::getSmallConstantTripCount(llvm::Loop* loop) {
#ifdef LLVM_OLDER_THAN_3_8
    ScalarEvolution *SE = &getAnalysis<llvm::ScalarEvolution>();
#else
    ScalarEvolution *SE =
        &getAnalysis<llvm::ScalarEvolutionWrapperPass>().getSE();
#endif
    llvm::BasicBlock* loopLatch = loop->getLoopLatch();
    // In case of the forever loop in the exit() the latch does
    // not jump out of the loop and SE cannot analyze the trip count,
    // which is infinite in that case.
    if (loopLatch == NULL || !loop->isLoopExiting(loopLatch))
        return 0;
    return SE->getSmallConstantTripCount(loop, loopLatch);
}
