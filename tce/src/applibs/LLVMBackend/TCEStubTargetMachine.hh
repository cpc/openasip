/*
    Copyright (c) 2002-2015 Tampere University of Technology.

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
 * @file TCEStubTargetMachine.hh
 *
 * Declaration of TCEStubTargetMachine class.
 *
 * @author Ville Korhonen 2015
 */

#ifndef TTA_TCE_STUB_TARGET_MACHINE_HH
#define TTA_TCE_STUB_TARGET_MACHINE_HH

#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include "llvm/Target/TargetLowering.h"
#include <llvm/Target/TargetLoweringObjectFile.h>
#include <llvm/CodeGen/TargetLoweringObjectFileImpl.h>

// initializes TCE target to LLVM
extern "C" void LLVMInitializeTCETargetInfo();

// initializes TCEStub as the targetmachine impl. for the TCE Target
extern "C" void LLVMInitializeTCEStubTarget();

namespace TTAMachine {
    class Machine;
}

namespace llvm {
    extern Target TheTCETarget;
    extern Target TheTCELETarget;

    class MCContext;
    class TargetMachine;
    class StringRef;
    class TCEStubTargetMachine;
    class TCEStubSubTarget;

    /**
     * Base class common to TCEStubTargetMachine(for middle end) and
     * TCETargetMachine(for backend)
     */
    class TCEBaseTargetMachine : public LLVMTargetMachine {
    public:
        TCEBaseTargetMachine(
            const Target &T, const Triple& TT,
            const std::string& CPU, const std::string &FS,
            const TargetOptions &Options,
            Reloc::Model RM, CodeModel::Model CM,
            CodeGenOpt::Level OL);

        const TTAMachine::Machine* ttaMach_;
        virtual void setTTAMach(
            const TTAMachine::Machine* mach) {
            ttaMach_ = mach;
        }
    };

    /**
     * TargetStub for middle end optimizations. (for loopvectorizer initially)
     */
    class TCEStubTargetMachine : public TCEBaseTargetMachine {
    protected:
        std::unique_ptr<TargetLoweringObjectFile> TLOF;
        TCEStubSubTarget *ST;

    public:
        TCEStubTargetMachine(
            const Target &T, const Triple& TT,
            const std::string& CPU, const std::string& FS,
            const TargetOptions &Options,
            Reloc::Model RM, CodeModel::Model CM,
            CodeGenOpt::Level OL);

        virtual ~TCEStubTargetMachine();

        const TCEStubSubTarget *getSubtargetImpl() const {
            return ST;
        }

        virtual TargetIRAnalysis getTargetIRAnalysis() override;

    };

} //end namespace llvm
#endif
