//===-- TCETargetAsmInfo.cpp - TCE asm properties -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the TCETargetAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "TCETargetAsmInfo.hh"

using namespace llvm;

TCETargetAsmInfo::TCETargetAsmInfo(const Target &T, const StringRef &TT) {
    AlignmentIsInBytes          = true;
    COMMDirectiveTakesAlignment = true;
    Data8bitsDirective          = "DA 1\t";
    Data16bitsDirective         = NULL;
    Data32bitsDirective         = NULL;
    Data64bitsDirective         = NULL;
    CommentString               = ";";
}
