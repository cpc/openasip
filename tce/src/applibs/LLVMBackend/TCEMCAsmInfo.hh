//=====------ TCEMCAsmInfo.h - TCE asm properties -------------*- C++ -*--====//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the TCEMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef TCEMCASMINFO_H
#define TCEMCASMINFO_H

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include <llvm/MC/MCAsmInfo.h>
#include "tce_config.h"

POP_COMPILER_DIAGS

namespace llvm {
  class Target;
  class StringRef;
  class Triple;
  
  class TCEMCAsmInfo : public MCAsmInfo {
  public:    
#ifdef LLVM_OLDER_THAN_10
    explicit TCEMCAsmInfo(const llvm::Triple &);
#else
    explicit TCEMCAsmInfo(
        const llvm::Triple&, const llvm::MCTargetOptions&);
#endif
  };

} // namespace llvm

#endif
