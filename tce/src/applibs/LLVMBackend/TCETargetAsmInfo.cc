/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file TCETargetAsmInfo.cpp
 *
 * Implementation of TCETargetAsmInfo class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 */

#include "TCETargetAsmInfo.hh"

using namespace llvm;

/**
 * The Constructor.
 *
 * Sets some TCE-specific values in the base class attributes.
 */
TCETargetAsmInfo::TCETargetAsmInfo(const TCETargetMachine& /* tm  */) {
    Data16bitsDirective = "\t.word\t";
    Data64bitsDirective = 0;
    ZeroDirective = "\t.space\t";
    CommentString = " # ";

    ConstantPoolSection = "\t# -- Constant pool --\n\t.data\n";
    PrivateGlobalPrefix = "L";
    GlobalPrefix = "_";
    InlineAsmStart = "# INLINE ASM Start";
    InlineAsmEnd = "# INLINE ASM End";

    // TCEAsmPrinter doesn't print string initializations.
    AsciiDirective = NULL;
    AscizDirective = NULL;

    LCOMMDirective = "\t.comm\t";
}
