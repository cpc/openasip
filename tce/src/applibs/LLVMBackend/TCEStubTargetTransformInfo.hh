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
 * @file TCEStubTargeTransformInfo.hh
 *
 * Declaration of TCEStubTargetTransformInfo class.
 *
 * @author Ville Korhonen 2015
 */

#ifndef TTA_TCE_TARGET_TRANSFORM_INFO_HH
#define TTA_TCE_TARGET_TRANSFORM_INFO_HH

#include <llvm/Analysis/TargetTransformInfo.h>
#include <llvm/CodeGen/BasicTTIImpl.h>
#include "TCEStubTargetTransformInfo.hh"
#include "TCEStubTargetMachine.hh"
#include "TCEStubSubTarget.hh"

namespace llvm {

    class TCEStubTTIImpl : public BasicTTIImplBase<TCEStubTTIImpl> {
        typedef BasicTTIImplBase<TCEStubTTIImpl> BaseT;
        typedef TargetTransformInfo TTI;
        friend BaseT;

        const TCEStubTargetMachine* TM;
        const TCEStubSubTarget *ST;
        const TargetLowering *TLI;
        const TCEStubSubTarget *getST() const { return ST; }
        const TargetLowering *getTLI() const {
            return ST->getTargetLowering();
        }

    public:
        explicit TCEStubTTIImpl(const TCEStubTargetMachine *TM, 
				const Function &F)
            : BaseT(TM, F.getParent()->getDataLayout()),
              TM(TM), ST(TM->getSubtargetImpl()) {}

        unsigned getNumberOfRegisters(bool vector);
#ifdef LLVM_OLDER_THAN_5_0
        unsigned getRegisterBitWidth(bool vector);
#else
	unsigned getRegisterBitWidth(bool vector) const;
#endif
        unsigned getMaxInterleaveFactor(unsigned VF);
        unsigned getCastInstrCost(unsigned Opcode, Type *Dst, Type *Src
#ifndef LLVM_OLDER_THAN_5_0
				  , const Instruction* = nullptr
#endif
				  );
    };
}

#endif
