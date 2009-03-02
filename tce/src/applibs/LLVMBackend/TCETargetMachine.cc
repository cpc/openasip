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
 * @file TCETargeMachine.cpp
 *
 * Declaration of TCETargetMachine class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 */

#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Target/TargetMachineRegistry.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"

#include "TCETargetMachine.hh"
#include "TCETargetAsmInfo.hh"
#include "LLVMPOMBuilder.hh"
#include "PluginTools.hh"
#include "FileSystem.hh"
#include "ADFSerializer.hh"

#include <iostream>

using namespace llvm;

//namespace {
//    RegisterTarget<TCETargetMachine>  X("tce", "TCE");
//}

const TargetAsmInfo*
TCETargetMachine::createTargetAsmInfo() const {
    return new TCETargetAsmInfo(*this);
}

//
// Data layout:
//--------------
// E-p:32:32:32-i1:8:8-i8:8:8-i32:32:32-i64:32:64-f32:32:32-f64:32:64"
// E = Big endian data
// -p:32:32:32 = Pointer size 32 bits, api & preferred alignment 32 bits.
// -i8:8:8 = 8bit integer api & preferred alignment 8 bits.
// etc.
// 
//  Frame info:
// -------------
// Grows down, alignment 4 bytes.
//
TCETargetMachine::TCETargetMachine(
    const Module& m, const std::string& fs, TCETargetMachinePlugin& plugin)
    : dataLayout_(
        "E-p:32:32:32"
	"-a0:32:32"
        "-i1:8:8"
        "-i8:8:8"
        "-i32:32:32"
        "-i64:32:64"
        "-f32:32:32"
        "-f64:32:64"),
      subtarget_(m, fs),
      frameInfo_(TargetFrameInfo::StackGrowsDown, 4, -4),
      plugin_(plugin), pluginTool_(NULL) {

    if (!plugin_.hasSDIV()) missingOps_.insert(llvm::ISD::SDIV);
    if (!plugin_.hasUDIV()) missingOps_.insert(llvm::ISD::UDIV);
    if (!plugin_.hasSREM()) missingOps_.insert(llvm::ISD::SREM);
    if (!plugin_.hasUREM()) missingOps_.insert(llvm::ISD::UREM);
    if (!plugin_.hasMUL()) missingOps_.insert(llvm::ISD::MUL);
    if (!plugin_.hasROTL()) missingOps_.insert(llvm::ISD::ROTL);
    if (!plugin_.hasROTR()) missingOps_.insert(llvm::ISD::ROTR);
    
    // register machine to plugin
    plugin_.registerTargetMachine(*this);
}

/**
 * The Destructor.
 */
TCETargetMachine::~TCETargetMachine() {
    if (pluginTool_ != NULL) {
        delete pluginTool_;
        pluginTool_ = NULL;
    }
}

/**
 * Checks how well target triple from llvm-gcc matches with this architecture.
 */
unsigned 
TCETargetMachine::getModuleMatchQuality(const Module &M) {
    std::string TT = M.getTargetTriple();
    if (TT.size() >= 4 && std::string(TT.begin(), TT.begin()+4) == "tce-") {
        return 20;
    }

    return 0;
}

/**
 * Creates an instruction selector instance.
 *
 * @param pm Function pass manager to add isel pass.
 * @param fast Not used.
 */
bool
TCETargetMachine::addInstSelector(FunctionPassManager& pm, bool /* fast */) {
    FunctionPass* isel = plugin_.createISelPass(this);
    pm.add(isel);
    return false;
}


/**
 * Creates a code printer instance.
 *
 * @param pm Pass manager where the code printer pass is added.
 * @param fast Unused.
 * @param out Output stream for the code printer.
 */
bool
TCETargetMachine::addAssemblyEmitter(
    FunctionPassManager& pm, bool /* fast */, llvm::raw_ostream& out) {

    // Output assembly language.
    pm.add(plugin_.createAsmPrinterPass(out, this));
    return false;
}

/**
 * Creates a TTAMachine::Machine object of the target architecture.
 */
TTAMachine::Machine*
TCETargetMachine::createMachine() {
    ADFSerializer serializer;
    serializer.setSourceString(*plugin_.adfXML());
    return serializer.readMachine();
}

/**
 * Returns list of llvm::ISD SelectionDAG opcodes for operations that are not
 * supported in the target architecture.
 *
 * The returned operations have to be expanded to emulation fucntion calls
 * or emulation patterns in TCETargetLowering.
 */
const std::set<unsigned>*
TCETargetMachine::missingOperations() {
    return &missingOps_;
}
