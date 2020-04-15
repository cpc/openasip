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
#include "tce_config.h"
#ifdef LLVM_OLDER_THAN_6_0
#include <llvm/Target/TargetLowering.h>
#else
#include <llvm/CodeGen/TargetLowering.h>
#endif
// LLVM has function with parameter named as "PIC". Command line
// option -DPIC messes up the compilation.
#pragma push_macro("PIC")
#undef PIC
#ifndef LLVM_6_0
// LLVM, COULD YOU PLEASE TRY TO DECIDE WHERE TO PUT THIS FILE?
#include <llvm/Target/TargetLoweringObjectFile.h>
#else
#include <llvm/CodeGen/TargetLoweringObjectFile.h>
#endif
#include <llvm/CodeGen/TargetLoweringObjectFileImpl.h>
#pragma pop_macro("PIC")

#include "TCEStubSubTarget.hh"

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
    class TargetSubTargetInfo;

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
#ifdef LLVM_OLDER_THAN_3_9
        TCEStubTargetMachine(
            const Target &T, const Triple& TT,
            const std::string& CPU, const std::string& FS,
            const TargetOptions &Options,
            Reloc::Model RM, CodeModel::Model CM,
            CodeGenOpt::Level OL);
#elif LLVM_OLDER_THAN_6_0
        TCEStubTargetMachine(
            const Target &T, const Triple& TT,
            const std::string& CPU, const std::string& FS,
            const TargetOptions &Options,
            Optional<Reloc::Model> RM, CodeModel::Model CM,
            CodeGenOpt::Level OL);
#else
        TCEStubTargetMachine(
            const Target &T, const Triple& TT,
            const std::string& CPU, const std::string& FS,
            const TargetOptions &Options,
            Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
            CodeGenOpt::Level OL, bool isLittle);
#endif
        // TODO: this is no longer virtual in llvm 6.0
        // I wonder what this button does..
#ifdef LLVM_OLDER_THAN_6_0
        virtual TargetIRAnalysis getTargetIRAnalysis() override;
#endif
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
