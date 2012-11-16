#include "TCETargetObjectFile.hh"
#include "llvm/DerivedTypes.h"
#include "llvm/GlobalVariable.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

void TCETargetObjectFile::Initialize(MCContext &Ctx, const TargetMachine &TM){
    TargetLoweringObjectFileELF::Initialize(Ctx, TM);
}
