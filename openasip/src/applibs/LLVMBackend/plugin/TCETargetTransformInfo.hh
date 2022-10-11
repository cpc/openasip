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
 * @file TCETargeTransformInfo.hh
 *
 * Declaration of TCETargetTransformInfo class.
 *
 * @author Kanishkan Vadivel 2022
 */

#ifndef TTA_TCE_TARGET_TRANSFORM_INFO_HH
#define TTA_TCE_TARGET_TRANSFORM_INFO_HH

#include <llvm/Analysis/TargetTransformInfo.h>
#include <llvm/CodeGen/BasicTTIImpl.h>

#include "TCETargetMachine.hh"

namespace llvm {

class TCETTIImpl : public BasicTTIImplBase<TCETTIImpl> {
    typedef BasicTTIImplBase<TCETTIImpl> BaseT;
    typedef TargetTransformInfo TTI;
    friend BaseT;

    const TCESubtarget *ST;
    const TCETargetLowering *TLI;

    const TCESubtarget *
    getST() const {
        return ST;
    }
    const TCETargetLowering *
    getTLI() const {
        return TLI;
    }

public:
    explicit TCETTIImpl(const TCETargetMachine *TM, const Function &F)
        : BaseT(TM, F.getParent()->getDataLayout()),
          ST(TM->getSubtargetImpl()),
          TLI(static_cast<const TCETargetLowering *>(
              ST->getTargetLowering())) {}
    bool isHardwareLoopProfitable(
        Loop *L, ScalarEvolution &SE, AssumptionCache &AC,
        TargetLibraryInfo *LibInfo, HardwareLoopInfo &HWLoopInfo);
};
}  // namespace llvm

#endif
