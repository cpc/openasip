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
 * @file TCEStubSubTarget.hh
 *
 * Declaration of TCEStubSubTarget class.
 *
 * @author Ville Korhonen 2015
 */

#ifndef TTA_TCE_STUB_SUBTARGET_HH
#define TTA_TCE_STUB_SUBTARGET_HH

#include "TCEStubTargetMachine.hh"
#ifdef LLVM_OLDER_THAN_6_0
#include <llvm/Target/TargetSubtargetInfo.h>
#include <llvm/Target/TargetLowering.h>
#else
#include <llvm/CodeGen/TargetSubtargetInfo.h>
#include <llvm/CodeGen/TargetLowering.h>
#endif

/**
 * Minimalistic subtarget implementation to be used in LLVM middle end 
 * optimization phase. 
 *
 * At the moment the purpose is only to offer 
 * default targetLowering which is required by the LLVM.
 * @todo a Proper targetLowering for more accurate targetTransformInfo
 */

namespace llvm {
    class TCEStubTargetMachine;

    class TCEStubSubTarget : public TargetSubtargetInfo {
        const TargetLowering TLI;

    public:
        TCEStubSubTarget(
            const Triple &TT, const StringRef &CPU, 
            const StringRef &FS, const TCEStubTargetMachine &TM);

        const TargetLowering *getTargetLowering() const {
            return &TLI;
        }

    };
} // end namespace llvm
#endif
