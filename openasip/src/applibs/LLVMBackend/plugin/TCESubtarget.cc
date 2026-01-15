/*
    Copyright (c) 2002-2025 Tampere University.

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
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Heikki Kultala 2013 (hkultala-no.spam-cs.tut.fi)
 */

#include "TCESubtarget.hh"

#include <iostream>

#include "LLVMTCECmdLineOptions.hh"
#include "TCETargetMachine.hh"
#include "TCETargetMachinePlugin.hh"
#include "llvm/Support/CommandLine.h"

#define GET_SUBTARGETINFO_CTOR
#define GET_SUBTARGETINFO_MC_DESC
#define GET_SUBTARGETINFO_TARGET_DESC

#define DEBUG_TYPE ""

#include "TCEGenSubTargetInfo.inc"

#undef DEBUG_TYPE

#undef GET_SUBTARGETINFO_CTOR
#undef GET_SUBTARGETINFO_MC_DESC
#undef GET_SUBTARGETINFO_TARGET_DESC

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
TCESubtarget::TCESubtarget(TCETargetMachinePlugin* plugin)
    : TCEGenSubtargetInfo(
          Triple("tce-tut-llvm"), std::string(""), std::string(""),
          std::string("")),

      pluginFile_(BackendPluginFile),
      plugin_(plugin),
      InstrItins(getInstrItineraryForCPU("generic")) {
    assert(InstrItins.Itineraries != nullptr && "IstrItins not initialized");
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

#if LLVM_MAJOR_VERSION > 21
void
TCESubtarget::initLibcallLoweringInfo(LibcallLoweringInfo& Info) const {
    static const struct {
        const RTLIB::Libcall Op;
        const RTLIB::LibcallImpl Impl;
    } LibraryCalls[] = {
        {RTLIB::MUL_I32, RTLIB::impl___mulsi3},
        {RTLIB::MUL_I64, RTLIB::impl___muldi3},
        {RTLIB::SDIV_I32, RTLIB::impl___divsi3},
        {RTLIB::SDIV_I64, RTLIB::impl___divdi3},
        {RTLIB::SHL_I32, RTLIB::impl___ashlsi3},
        {RTLIB::SHL_I64, RTLIB::impl___ashldi3},
        {RTLIB::SRA_I32, RTLIB::impl___ashrsi3},
        {RTLIB::SRA_I64, RTLIB::impl___ashrdi3},
        {RTLIB::SREM_I32, RTLIB::impl___modsi3},
        {RTLIB::SREM_I64, RTLIB::impl___moddi3},
        {RTLIB::SRL_I32, RTLIB::impl___lshrsi3},
        {RTLIB::SRL_I64, RTLIB::impl___lshrdi3},
        {RTLIB::UDIV_I32, RTLIB::impl___udivsi3},
        {RTLIB::UDIV_I64, RTLIB::impl___udivdi3},
        {RTLIB::UREM_I32, RTLIB::impl___umodsi3},
        {RTLIB::UREM_I64, RTLIB::impl___umoddi3},
    };

    for (const auto& LC : LibraryCalls) Info.setLibcallImpl(LC.Op, LC.Impl);
}
#endif

const TargetInstrInfo* TCESubtarget::getInstrInfo() const {
    return plugin_->getInstrInfo();
}

const TargetRegisterInfo* TCESubtarget::getRegisterInfo() const {
    return plugin_->getRegisterInfo();
}

const TargetFrameLowering* TCESubtarget::getFrameLowering() const {
    return plugin_->getFrameLowering();
}

const TargetLowering* TCESubtarget::getTargetLowering() const { 
    return plugin_->getTargetLowering();
}

const SelectionDAGTargetInfo* TCESubtarget::getSelectionDAGInfo() const {
    return plugin_->getSelectionDAGInfo();
}

bool
TCESubtarget::enableMachinePipeliner() const {
    return false;
}
