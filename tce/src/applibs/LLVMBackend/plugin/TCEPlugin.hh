/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file TCEPlugin.h
 *
 * TCE Target plugin declaration.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Heikki Kultala 2011 (heikki.kultala-no.spam-tut.fi)
 */

#ifndef TCE_PLUGIN_H
#define TCE_PLUGIN_H

#include <iosfwd>

#include <llvm/CodeGen/TargetInstrInfo.h>

/**
 * Instruction selector and code printer creator declarations.
 */
namespace llvm  {
    class FunctionPass;
    class TCETargetMachine;

    FunctionPass* createTCEISelDag(TCETargetMachine& tm);
    FunctionPass* createTCECodePrinterPass(std::ostream& os, TargetMachine& tm);
}

#define GET_REGINFO_ENUM
#include "TCEGenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "TCEGenInstrInfo.inc"

#endif
