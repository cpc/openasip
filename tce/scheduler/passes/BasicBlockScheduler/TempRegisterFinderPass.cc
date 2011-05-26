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
 * @file TempRegisterFinderPass.cc
 *
 * Class which finds free regs for temp reg copier.
 *
 * @TODO: remove this class totally.
 *
 * @author Heikki Kultala 2008 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "TempRegisterFinderPass.hh"
#include "RegisterCopyAdder.hh"

void TempRegisterFinderPass::start() throw (Exception) {
    RegisterCopyAdder::findTempRegisters(*target_, interPassData());
}

/**
 * A short description of the module, usually the module name,
 * in this case "TempRegisterFinderPass".
 *
 * @return The description as a string.
 */
std::string
TempRegisterFinderPass::shortDescription() const {
    return "Startable: TempRegisterFinderPass";
}

/**
 * Optional longer description of the Module.
 *
 * This description can include usage instructions, details of choice of
 * helper modules, etc.
 *
 * @return The description as a string.
 */
std::string
TempRegisterFinderPass::longDescription() const {
    std::string answer ="Startable: TempRegisterFinderPass";
    answer += " Finds free regs for temporary register copier.";
    return answer;
}
SCHEDULER_PASS(TempRegisterFinderPass)
