/*
    Copyright (c) 2002-2019 Tampere University.

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
 * @file PLuginCompileWrapper.cc
 *
 * TCE plugin compilation wrapper to make plugin compilation faster:
 *
 * By including these .cc files into one .cc file, all #includes are only
 * included and compiled once. Without this wrapper all files that are
 * included in multiple .cc files are being compiled multiple times, wasting
 * compilation time.
 *
 * @author Sander Ruben 2019 (sander.ruben-no-spam-tuni.fi)
 * @note rating: red
 */

#include "TCEDAGToDAGISel.cc"
#include "TCEISelLowering.cc"
#include "TCEInstrInfo.cc"
#include "TCERegisterInfo.cc"
#include "TCESubtarget.cc"
#include "TCETargetObjectFile.cc"
#include "TCETargetMachinePlugin.cc"
#include "TCEFrameInfo.cc"
