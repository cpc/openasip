//===------ TCEMCAsmInfo.cpp - TCE asm properties -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the TCEMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include <cstdlib> // NULL
#include "TCEMCAsmInfo.hh"
#include "tce_config.h"

using namespace llvm;

#ifdef LLVM_OLDER_THAN_3_7
TCEMCAsmInfo::TCEMCAsmInfo(const StringRef&) {
#else
TCEMCAsmInfo::TCEMCAsmInfo(const llvm::Triple&) {
#endif
    AlignmentIsInBytes          = true;
    COMMDirectiveAlignmentIsInBytes = true;

    //COMMDirectiveTakesAlignment = true;
    Data8bitsDirective          = "DA 1\t";
    Data16bitsDirective         = NULL;
    Data32bitsDirective         = NULL;
    Data64bitsDirective         = NULL;
    CommentString               = ";";
}
