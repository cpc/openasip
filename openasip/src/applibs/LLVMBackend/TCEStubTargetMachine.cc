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
#include "ADFSerializer.hh"
#include "TCEStubTargetTransformInfo.hh"
#include "TCEStubSubTarget.hh"
#include <llvm/Target/TargetLoweringObjectFile.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/CodeGen/Passes.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/CodeGen/TargetPassConfig.h>

using namespace llvm;

Target llvm::TheTCETarget;
Target llvm::TheTCELETarget;
Target llvm::TheTCELE64Target;

extern "C" void LLVMInitializeTCETargetInfo() {
    RegisterTarget<Triple::tce, /*HasJIT=*/false>
        X(TheTCETarget, "tce-tut-llvm", "TCE custom processor",
            "TODO: wonder what this button does");
    RegisterTarget<Triple::tcele, false>
        Y(TheTCELETarget, "tcele-tut-llvm",
            "TCE custom processor (little endian)",
            "TODO: wonder what this button does");
    RegisterTarget<Triple::tcele64, false>
        Z(TheTCELE64Target, "tcele64-tut-llvm", "64-bit TCE custom processor (little endian) ",
          "TODO: wonder what this button does");
}

extern "C" void LLVMInitializeTCEStubTarget() {
    // Register the targetmachine impl.
    RegisterTargetMachine<TCEStubTargetMachine> X(TheTCETarget);
    RegisterTargetMachine<TCEStubTargetMachine> Y(TheTCELETarget);
    RegisterTargetMachine<TCEStubTargetMachine> Z(TheTCELE64Target);
}

extern "C" void LLVMInitializeTCEStubTargetMC() {}

StringRef DescriptionStringBE = "E-p:32:32:32-i1:8:8-i8:8:32-"
    "i16:16:32-i32:32:32-i64:32:32-"
    "f32:32:32-f64:32:32-v64:32:32-"
    "v128:32:32-v256:32:32-v512:32:32-v1024:32:32-a0:0:32-n32";

#if LLVM_HAS_CUSTOM_VECTOR_EXTENSION == 2
StringRef DescriptionStringLE = "e-p:32:32:32-i1:8:8-i8:8:32-"
    "i16:16:32-i32:32:32-i64:32:32-"
    "f32:32:32-f64:32:32-v64:64:64-"
    "v128:128:128-v256:256:256-v512:512:512-v1024:1024:1024-"
    "v2048:2048:2048-v4096:4096:4096-a0:0:32-n32";
#else
StringRef DescriptionStringLE = "e-p:32:32:32-i1:8:8-i8:8:32-"
    "i16:16:32-i32:32:32-i64:32:32-"
    "f32:32:32-f64:32:32-v64:64:64-"
    "v128:128:128-v256:256:256-v512:512:512-v1024:1024:1024-a0:0:32-n32";
#endif

StringRef DescriptionStringLE64 = "e-p:64:64:64-i1:8:64-i8:8:64-"
    "i16:16:64-i32:32:64-i64:64:64-"
    "f32:32:64-f64:64:64-v64:64:64-"
    "v128:128:128-v256:256:256-v512:512:512-v1024:1024:1024-"
    "v2048:2048:2048-v4096:4096:4096-a0:0:64-n64";

StringRef getTargetDesc(const Triple &TT) {
    if (TT.getArchName().equals("tce"))
        return DescriptionStringBE;
    else {
        if (TT.getArchName().equals("tcele"))
            return DescriptionStringLE;
        else
            return DescriptionStringLE64;
    }
}

/* Base class constructor */
TCEBaseTargetMachine::TCEBaseTargetMachine(
    const Target &T, const Triple& TT, const llvm::StringRef& CPU,
    const llvm::StringRef& FS, const TargetOptions &Options,
    Reloc::Model RM, CodeModel::Model CM, CodeGenOpt::Level OL) :
    LLVMTargetMachine(T, getTargetDesc(TT), TT, CPU, FS, Options, RM, CM, OL),
    ttaMach_(NULL) {
}


TCEStubTargetMachine::TCEStubTargetMachine(
    const Target &T, const Triple &TT, const llvm::StringRef& CPU,
    const llvm::StringRef& FS, const TargetOptions &Options,
    #ifdef LLVM_OLDER_THAN_16
    Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
    #else
    std::optional<Reloc::Model> RM, std::optional<CodeModel::Model> CM,
    #endif
    CodeGenOpt::Level OL, bool) :
    TCEBaseTargetMachine(T, TT, CPU, FS, Options,
                         RM?*RM:Reloc::Model::Static,
                         CM?*CM:CodeModel::Small, OL),
    // Note: Reloc::Model does not have "Default" named member. "Static" is ok?
    // Note: CodeModel does not have "Default" named member. "Small" is ok?
    TLOF(new TargetLoweringObjectFileELF) {
//    TLOF(new TargetLoweringObjectFile) {
    ST = new TCEStubSubTarget(TT, CPU, FS, *this);

    // For autovectorization to work we need to set target machine information:
    // Load ADF from static adfXML string
    ADFSerializer serializer;
    serializer.setSourceString(adfXML_);
    // Create and set TTAMachine
    TTAMachine::Machine* targetTTAMachine = serializer.readMachine();
    setTTAMach(targetTTAMachine);
}

TCEStubTargetMachine::~TCEStubTargetMachine() {}

void
TCEStubTargetMachine::setADFString(std::string adfXML) {
    adfXML_ = adfXML;
}

std::string TCEStubTargetMachine::adfXML_ = "";

namespace {
    /**
     * Minimalistic pass config for middle end passes 
     */
    class TCEStubPassConfig : public TargetPassConfig {
    public:
        TCEStubPassConfig(TCEStubTargetMachine *TM, PassManagerBase &PM)
            : TargetPassConfig(*TM, PM) {}

        TCEStubTargetMachine &getTCEStubTargetMachine() const {
            return getTM<TCEStubTargetMachine>();
        }

    };
} // end namespace
