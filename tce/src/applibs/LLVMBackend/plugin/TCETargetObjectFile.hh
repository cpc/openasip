
#ifndef LLVM_TARGET_TCE_TARGETOBJECTFILE_H
#define LLVM_TARGET_TCE_TARGETOBJECTFILE_H

#include "tce_config.h"
#include <llvm/Target/TargetLoweringObjectFile.h>
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

  class TCETargetObjectFile : public TargetLoweringObjectFileELF {
  public:
    void Initialize(MCContext &Ctx, const TargetMachine &TM);
  };
} // end namespace llvm

#endif
