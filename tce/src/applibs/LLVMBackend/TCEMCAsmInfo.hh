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

#include <llvm/MC/MCAsmInfo.h>

namespace llvm {
  class Target;
  class StringRef;
  
  class TCEMCAsmInfo : public MCAsmInfo {
  public:    
    explicit TCEMCAsmInfo(const Target &T, const StringRef &TT);
  };

} // namespace llvm

#endif
