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
 * @file TCEStubSubTarget.cpp
 *
 * Declaration of TCEStubSubTarget class.
 *
 * @author Ville Korhonen 2015
 */

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "tce_config.h"

#include "TCEStubTargetMachine.hh"
#include "TCEStubSubTarget.hh"
#include <llvm/CodeGen/TargetSubtargetInfo.h>

using namespace llvm;

TCEStubSubTarget::TCEStubSubTarget(
    const Triple& TT, const StringRef& CPU, const StringRef& FS,
    const TCEStubTargetMachine& TM)
    :
#if LLVM_MAJOR_VERSION < 21
      TargetSubtargetInfo(
          TT, CPU, FS, FS, ArrayRef<SubtargetFeatureKV>(),
          ArrayRef<SubtargetSubTypeKV>(), NULL, NULL, NULL, NULL, NULL, NULL),
#else
      TargetSubtargetInfo(
          TT, CPU, FS, FS, llvm::ArrayRef<llvm::StringRef>(),
          ArrayRef<SubtargetFeatureKV>(), ArrayRef<SubtargetSubTypeKV>(),
          NULL, NULL, NULL, NULL, NULL, NULL),
#endif
      TLI(TM) {
}
