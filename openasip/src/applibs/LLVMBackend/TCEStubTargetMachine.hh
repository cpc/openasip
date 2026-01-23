/*
    Copyright (c) 2002-2025 Tampere University.

    This file is part of OpenASIP.

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

#include <llvm/CodeGen/CodeGenTargetMachineImpl.h>
#include <llvm/CodeGen/TargetLowering.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>

#include "tce_config.h"
// LLVM has function with parameter named as "PIC". Command line
// option -DPIC messes up the compilation.
#define PICCOPY PIC
#undef PIC
#include <llvm/Target/TargetLoweringObjectFile.h>
#include <llvm/CodeGen/TargetLoweringObjectFileImpl.h>
#define PIC PICCOPY
#undef PICCOPY

// TODO: What was the meaning of this. may have been broken with a merge.
//#ifndef PACKAGE
//#include "tce_config.h"
//#endif

#include "TCEStubSubTarget.hh"
#include "TCETargetDataLayout.hh"

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
    extern Target TheTCELE64Target;

    class MCContext;
    class TargetMachine;
    class StringRef;
    class TCEStubTargetMachine;
    class TCEStubSubTarget;
    class TargetSubTargetInfo;
#if LLVM_MAJOR_VERSION >= 21
    typedef llvm::CodeGenTargetMachineImpl LLVMTargetMachine;
#else
    typedef llvm::CodeGenOpt::Level CodeGenOptLevel;
#endif

    /**
     * Base class common to TCEStubTargetMachine(for middle end) and
     * TCETargetMachine(for backend)
     */
    class TCEBaseTargetMachine : public LLVMTargetMachine {
    public:
        TCEBaseTargetMachine(
            const Target& T, const Triple& TT, const llvm::StringRef& CPU,
            const llvm::StringRef& FS, const TargetOptions& Options,
            Reloc::Model RM, CodeModel::Model CM, CodeGenOptLevel OL);
        virtual bool
        isNoopAddrSpaceCast(unsigned SrcAS, unsigned DestAS) const override {
            return true;
        }
        const TTAMachine::Machine* ttaMach_;
        virtual void
        setTTAMach(const TTAMachine::Machine* mach) {
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
            const Target& T, const Triple& TT, const llvm::StringRef& CPU,
            const llvm::StringRef& FS, const TargetOptions& Options,
            std::optional<Reloc::Model> RM,
            std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
            bool isLittle);

        // TODO: this is no longer virtual in llvm 6.0
        // I wonder what this button does..
        virtual ~TCEStubTargetMachine();

        //const TargetSubtargetInfo *getSubtargetImpl() const {
        const TCEStubSubTarget *getSubtargetImpl() const {
            return ST;
        }
        const TCEStubSubTarget *getSubtargetImpl(const Function&)
            const override {
            return ST;
        }
    };
} //end namespace llvm

#endif
