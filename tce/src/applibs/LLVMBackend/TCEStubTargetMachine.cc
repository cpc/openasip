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
 * @file TCEStubTargeMachine.cc
 *
 * Declaration of TCEStubTargetMachine class.
 *
 * @author Ville Korhonen 2015
 */

/**
 * This Stub target is for middle end optimizations where the actual backend 
 * plugin is not yet generated.
 *
 * Intention is to use ADF information in
 * targetTransformInfo class to offer target spesific details for middle end 
 * optimizations such as loop vectorizer.
 *
 * @todo pass for handing over the ADF to the stub target.
 * @todo add more target spesific features to targetTransformInfo
 * @todo unify TCEStubTarget and TCETarget features by migrating common 
 *       features to base class
 */

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "tce_config.h"
#include "TCEStubTargetMachine.hh"
#ifndef LLVM_OLDER_THAN_3_7
#include "TCEStubTargetTransformInfo.hh"
#endif
#include "TCEStubSubTarget.hh"
#include <llvm/Target/TargetLoweringObjectFile.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/CodeGen/Passes.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetOptions.h>
#ifndef LLVM_OLDER_THAN_3_9
#include <llvm/CodeGen/TargetPassConfig.h>
#endif

using namespace llvm;

Target llvm::TheTCETarget;
Target llvm::TheTCELETarget;

extern "C" void LLVMInitializeTCETargetInfo() {
    RegisterTarget<Triple::tce, /*HasJIT=*/false>
        X(TheTCETarget, "tce", "TCE custom processor");
    RegisterTarget<Triple::tcele, false>
        Y(TheTCELETarget, "tcele", "TCE custom processor (little endian)");
}

#ifndef LLVM_OLDER_THAN_3_7
extern "C" void LLVMInitializeTCEStubTarget() {
    // Register the targetmachine impl.
    RegisterTargetMachine<TCEStubTargetMachine> X(TheTCETarget);
    RegisterTargetMachine<TCEStubTargetMachine> Y(TheTCELETarget);
}
#endif

extern "C" void LLVMInitializeTCEStubTargetMC() {}

StringRef DescriptionStringBE = "E-p:32:32:32-i1:8:8-i8:8:32-"
    "i16:16:32-i32:32:32-i64:32:32-"
    "f32:32:32-f64:32:32-v64:32:32-"
    "v128:32:32-v256:32:32-v512:32:32-v1024:32:32-a0:0:32-n32";

StringRef DescriptionStringLE = "e-p:32:32:32-i1:8:8-i8:8:32-"
    "i16:16:32-i32:32:32-i64:32:32-"
    "f32:32:32-f64:32:32-v64:32:32-"
    "v128:32:32-v256:32:32-v512:32:32-v1024:32:32-a0:0:32-n32";

StringRef getTargetDesc(const Triple &TT) {
    if (TT.getArchName().equals("tce"))
        return DescriptionStringBE;
    else
        return DescriptionStringLE;
}

/* Base class constructor */
#ifdef LLVM_OLDER_THAN_3_7
TCEBaseTargetMachine::TCEBaseTargetMachine(
    const Target &T, const std::string& TT, const std::string& CPU,
    const std::string &FS, const TargetOptions &Options,
    Reloc::Model RM, CodeModel::Model CM, CodeGenOpt::Level OL) :
    LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
    ttaMach_(NULL) {
}
#else
TCEBaseTargetMachine::TCEBaseTargetMachine(
    const Target &T, const Triple& TT, const std::string& CPU, 
    const std::string &FS, const TargetOptions &Options,
    Reloc::Model RM, CodeModel::Model CM, CodeGenOpt::Level OL) :
    LLVMTargetMachine(T, getTargetDesc(TT), TT, CPU, FS, Options, RM, CM, OL),
    ttaMach_(NULL) {
}
#endif

#ifndef LLVM_OLDER_THAN_3_7
#ifdef LLVM_OLDER_THAN_3_9
TCEStubTargetMachine::TCEStubTargetMachine(
    const Target &T, const Triple &TT, const std::string& CPU, 
    const std::string& FS, const TargetOptions &Options,
    Reloc::Model RM, CodeModel::Model CM, CodeGenOpt::Level OL) :
    TCEBaseTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
    TLOF(new TargetLoweringObjectFileELF) {
    ST = new TCEStubSubTarget(TT, CPU, FS, *this);
}
#else
TCEStubTargetMachine::TCEStubTargetMachine(
    const Target &T, const Triple &TT, const std::string& CPU,
    const std::string& FS, const TargetOptions &Options,
    Optional<Reloc::Model> RM, CodeModel::Model CM, CodeGenOpt::Level OL) :
    TCEBaseTargetMachine(T, TT, CPU, FS, Options,
                         RM?*RM:Reloc::Model::Static, CM, OL),
    // Note: Reloc::Model does not have "Default" named member. "Static" is ok?
    TLOF(new TargetLoweringObjectFileELF) {
    ST = new TCEStubSubTarget(TT, CPU, FS, *this);
}
#endif

TargetIRAnalysis TCEStubTargetMachine::getTargetIRAnalysis() {
#ifdef LLVM_OLDER_THAN_3_8
    return TargetIRAnalysis(
        [this](Function &F) {
            return TargetTransformInfo(TCEStubTTIImpl(this, F)); });
#else
    return TargetIRAnalysis(
        [this](const Function &F) {
            return TargetTransformInfo(TCEStubTTIImpl(this, F)); });
#endif
}

TCEStubTargetMachine::~TCEStubTargetMachine() {}

namespace {
    /**
     * Minimalistic pass config for middle end passes 
     */
    class TCEStubPassConfig : public TargetPassConfig {
    public:
        TCEStubPassConfig(TCEStubTargetMachine *TM, PassManagerBase &PM)
            : TargetPassConfig(TM, PM) {}

        TCEStubTargetMachine &getTCEStubTargetMachine() const {
            return getTM<TCEStubTargetMachine>();
        }

    };
} // end namespace
#endif
