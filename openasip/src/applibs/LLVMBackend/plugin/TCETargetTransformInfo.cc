/*
    Copyright (c) 2002-2022 Kanishkan Vadivel/Eindhoven University.

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
 * @file TCETargetTransformInfo.cc
 *
 * Declaration of TCETargetTransformInfo class.
 *
 * @author Kanishkan Vadivel 2022
 */

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "LLVMTCECmdLineOptions.hh"
#include "TCETargetTransformInfo.hh"

using namespace llvm;

#define DEBUG_TYPE "tcetti"

bool
TCETTIImpl::isHardwareLoopProfitable(
    Loop *L, ScalarEvolution &SE, AssumptionCache &AC,
    TargetLibraryInfo *LibInfo, HardwareLoopInfo &HWLoopInfo) const {
    // Check TCE flag for hwloop
    LLVMTCECmdLineOptions *options =
        dynamic_cast<LLVMTCECmdLineOptions *>(Application::cmdLineOptions());
    if (options == NULL || options->disableHWLoops()) return false;

    // We should do hwloop only for single BB loop
    if (L->getNumBlocks() != 1) {
        LLVM_DEBUG(
            dbgs() << "HWLoops: Loop has " << L->getNumBlocks()
                   << " BB. Not converting to hwloop: " << L->getName()
                   << "\n");
        return false;
    }

    // Set counter type and loop decrement value
    LLVMContext &C = L->getHeader()->getContext();
    HWLoopInfo.CountType = Type::getInt32Ty(C);
    HWLoopInfo.LoopDecrement = ConstantInt::get(HWLoopInfo.CountType, 1);
    HWLoopInfo.IsNestingLegal = false;
    return true;
}
