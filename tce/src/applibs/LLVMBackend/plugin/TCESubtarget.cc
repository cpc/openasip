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
 * @file TCESubtarget.cpp
 *
 * Implementation of TCESubtargetClass.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Heikki Kultala 2013 (hkultala-no.spam-cs.tut.fi)
 */

#include <iostream>
#include "llvm/Support/CommandLine.h"

#include "TCESubtarget.hh"

#define GET_SUBTARGETINFO_CTOR
#define GET_SUBTARGETINFO_MC_DESC
#define GET_SUBTARGETINFO_TARGET_DESC

#if (!defined(LLVM_3_2) && !defined(LLVM_3_3) && !defined(LLVM_3_4))
#define DEBUG_TYPE ""
#endif

#include "TCEGenSubTargetInfo.inc"

#if (!defined(LLVM_3_2) && !defined(LLVM_3_3) && !defined(LLVM_3_4))
#undef DEBUG_TYPE
#endif

using namespace llvm;

// Add plugin file name option.
cl::opt<std::string>
BackendPluginFile(
    "tce-plugin-file",
    cl::value_desc("plugin file"),
    cl::desc("TCE target machine plugin file."),
    cl::NotHidden);

/**
 * The Constructor.
 */
TCESubtarget::TCESubtarget(): //const std::string &TT, const std::string &FS) :
//    TCEGenSubtargetInfo(TT, FS, std::string("")),
    TCEGenSubtargetInfo(std::string("tut-tce-llvm"), std::string(""), std::string("")),
    pluginFile_(BackendPluginFile) {

}

/**
 * Returns full path of the plugin file name supplied with the
 * -tce-plugin-file parameter.
 *
 * @return plugin file path
 */
std::string
TCESubtarget::pluginFileName() {
    return pluginFile_;
}
