#include "TCETargetObjectFile.hh"
#include "tce_config.h"
#if (defined(LLVM_3_1) || defined(LLVM_3_2))
#include "llvm/DerivedTypes.h"
#include "llvm/GlobalVariable.h"
#else
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#endif
#include "llvm/MC/MCSectionELF.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

void TCETargetObjectFile::Initialize(MCContext &Ctx, const TargetMachine &TM){
    TargetLoweringObjectFileELF::Initialize(Ctx, TM);
}
