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
  
  class TCEMCAsmInfo : public MCAsmInfo {
  public:    
#if (defined LLVM_3_3 || defined LLVM_3_2 || defined LLVM_3_1)
    explicit TCEMCAsmInfo(const Target &, const StringRef &);
#else
    explicit TCEMCAsmInfo(const StringRef &);
#endif
  };

} // namespace llvm

#endif
