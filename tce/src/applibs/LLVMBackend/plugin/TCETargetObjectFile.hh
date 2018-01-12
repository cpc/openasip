
#ifndef LLVM_TARGET_TCE_TARGETOBJECTFILE_H
#define LLVM_TARGET_TCE_TARGETOBJECTFILE_H

#include "tce_config.h"
#ifdef LLVM_OLDER_THAN_6_0
#include <llvm/Target/TargetLoweringObjectFile.h>
#else
#include <llvm/CodeGen/TargetLoweringObjectFile.h>
#endif
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

  class TCETargetObjectFile : public TargetLoweringObjectFileELF {
  public:
    void Initialize(MCContext &Ctx, const TargetMachine &TM);
  };
} // end namespace llvm

#endif
