/*
    Copyright (c) 2002-2025 Tampere University.

    This file is part of OpenASIP.

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
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam—tut.fi)
 * @author Pekka Jääskeläinen 2007-2015
 */

#include "CompilerWarnings.hh"

IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "tce_config.h"

#include "llvm/IR/PassManager.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/ADT/APFloat.h"

#include "TCEStubTargetMachine.hh"
#include "TCETargetMachine.hh"
#include "TCEMCAsmInfo.hh"
#include "LLVMPOMBuilder.hh"
#include "PluginTools.hh"
#include "MathTools.hh"
#include "FileSystem.hh"
#include "ADFSerializer.hh"
#include "Conversion.hh"
#include "MachineInfo.hh"
#include "MachineConnectivityCheck.hh"
#include "Machine.hh"
#include "ImmediateAnalyzer.hh"
#include "ImmInfo.hh"
#include <llvm/Transforms/Scalar.h>


#include <iostream>

#include "LLVMTCECmdLineOptions.hh"

POP_COMPILER_DIAGS

using namespace llvm;


Pass* createLowerMissingInstructionsPass(const TTAMachine::Machine& mach);
Pass* createLinkBitcodePass(Module& inputCode);
Pass* createProgramPartitionerPass(std::string partitioningStrategy);
Pass* createInstructionPatternAnalyzer();

class DummyInstPrinter : public MCInstPrinter {
public:
    DummyInstPrinter(
        const llvm::MCAsmInfo& mai, const llvm::MCInstrInfo& mii,
        const llvm::MCRegisterInfo& mri)
        : llvm::MCInstPrinter(mai, mii, mri) {}

    bool
    applyTargetSpecificCLOption(StringRef Opt) override {
        return false;
    }

#if LLVM_MAJOR_VERSION < 21
    std::pair<const char*, uint64_t>
    getMnemonic(const MCInst* MI) override {
        return std::make_pair(nullptr, 0);
    }

    void
    printRegName(raw_ostream& OS, MCRegister Reg) const override {}

#else
    std::pair<const char*, uint64_t>
    getMnemonic(const MCInst&) const override {
        return std::make_pair(nullptr, 0);
    }

    void
    printRegName(raw_ostream& OS, MCRegister Reg) override {}
#endif

    void printInst(
        const MCInst*, uint64_t, StringRef,
        const MCSubtargetInfo&, raw_ostream&) override {}

};

// In TCE target we don't print the MCInsts from LLVM, but
// just want to run the codegen passes so we can get the
// MachineInstrs in to our own finalization phases. LLVM 3.7
// started to require MCInstPrinter to be created, a dummy
// must be created therefore.
MCInstPrinter *dummyInstrPrinterCtor(
    const Triple &,
    unsigned,
    const MCAsmInfo &MAI,
    const MCInstrInfo &MII,
    const MCRegisterInfo &MRI) {
    return new DummyInstPrinter(MAI, MII, MRI);
}

extern "C" void LLVMInitializeTCETarget() { 
    RegisterTargetMachine<TCETargetMachine> Y(TheTCETarget);
    RegisterTargetMachine<TCETargetMachine> X0(TheTCELE64Target);
    RegisterTargetMachine<TCETargetMachine> X(TheTCELETarget);

    RegisterMCAsmInfo<TCEMCAsmInfo> Z(TheTCETarget);
    RegisterMCAsmInfo<TCEMCAsmInfo> V0(TheTCELE64Target);
    RegisterMCAsmInfo<TCEMCAsmInfo> V(TheTCELETarget);
    TargetRegistry::RegisterMCInstPrinter(TheTCETarget, dummyInstrPrinterCtor);
    TargetRegistry::RegisterMCInstPrinter(
        TheTCELETarget, dummyInstrPrinterCtor);
    TargetRegistry::RegisterMCInstPrinter(
        TheTCELE64Target, dummyInstrPrinterCtor);
}

//
// Data layout:
//--------------
// E-p:32:32:32-a0:0:64-i1:8:8-i8:8:8-i32:32:32-i64:32:64-f32:32:32-f64:32:64" (old)
// E-p:32:32:32-a0:0:32-i1:8:8-i8:8:8-i32:32:32-i64:32:32-f32:32:32-f64:32:32" (new)
// E = Big endian data
// -p:32:32:32 = Pointer size 32 bits, api & preferred alignment 32 bits.
// -i8:8:8 = 8bit integer api & preferred alignment 8 bits.
// a0:0:32 = struct alignment (abi packed?), preferred 32 bits.
// etc.
// 

TCETargetMachine::TCETargetMachine(
    const Target &T, const Triple& TTriple,
    const llvm::StringRef& CPU, const llvm::StringRef& FS,
    const TargetOptions &Options,
#ifdef LLVM_OLDER_THAN_16
    Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM, CodeGenOpt::Level OL, bool) :
#else
    std::optional<Reloc::Model> RM, std::optional<CodeModel::Model> CM,
    CodeGenOptLevel OL, bool)
    :
#endif
    TCEBaseTargetMachine(T, TTriple, CPU, FS, Options,
                         RM?*RM:Reloc::Model::Static, CM?*CM:CodeModel::Small, OL),
    // Note: Reloc::Model does not have "Default" named member. "Static" is ok?
    // Note: CodeModel does not have "Default" named member. "Small" is ok?
    plugin_(NULL), pluginTool_(NULL) {
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

void 
TCETargetMachine::setTargetMachinePlugin(
    TCETargetMachinePlugin& plugin, TTAMachine::Machine& target) {

    setTTAMach(&target);
    plugin_ = &plugin;
    missingOps_.clear();
    promotedOps_.clear();
    MVT::SimpleValueType defType = plugin_->getDefaultType();
    if (!plugin_->hasSDIV()) missingOps_.insert(std::make_pair(llvm::ISD::SDIV, defType));
    if (!plugin_->hasUDIV()) missingOps_.insert(std::make_pair(llvm::ISD::UDIV, defType));
    if (!plugin_->hasSREM()) missingOps_.insert(std::make_pair(llvm::ISD::SREM, defType));
    if (!plugin_->hasUREM()) missingOps_.insert(std::make_pair(llvm::ISD::UREM, defType));
    if (!plugin_->hasMUL()) missingOps_.insert(std::make_pair(llvm::ISD::MUL, defType));
    if (!plugin_->hasROTL()) missingOps_.insert(std::make_pair(llvm::ISD::ROTL, defType));
    if (!plugin_->hasROTR()) missingOps_.insert(std::make_pair(llvm::ISD::ROTR, defType));

    if (!plugin_->hasSHL()) customLegalizedOps_.insert(std::make_pair(llvm::ISD::SHL, MVT::i32));
    if (!plugin_->hasSHR()) customLegalizedOps_.insert(std::make_pair(llvm::ISD::SRA, MVT::i32));
    if (!plugin_->hasSHRU()) customLegalizedOps_.insert(std::make_pair(llvm::ISD::SRL, MVT::i32));

    if (!plugin_->hasSXHW()) missingOps_.insert(
        std::make_pair(llvm::ISD::SIGN_EXTEND_INREG, MVT::i16));

    if (!plugin_->hasSXQW()) missingOps_.insert(
        std::make_pair(llvm::ISD::SIGN_EXTEND_INREG, MVT::i8));

    if (!plugin_->hasSQRTF()) {
        missingOps_.insert(std::make_pair(llvm::ISD::FSQRT, MVT::f32));
    }

    initAsmInfo();

    if (target.is64bit()) {
        plugin_->setDataLayout(DataLayoutStringLE64);
    } else {
      if (plugin_->isLittleEndian()) {
          plugin_->setDataLayout(DataLayoutStringLE);
      } else {
          plugin_->setDataLayout(DataLayoutStringBE);
      }
    }
    // register machine to plugin
    plugin_->registerTargetMachine(*this);
}

/**
 * Creates an instruction selector instance.
 *
 */
bool
TCEPassConfig::addInstSelector() 
{
    addPass(plugin_->createISelPass(static_cast<TCETargetMachine*>(TM)));
    return false;
}

/**
 * Some extra passes needed by TCE
 *
 * @param pm Function pass manager to add isel pass.
 * @param fast Not used.
 */
void
TCEPassConfig::addPreRegAlloc() {
}


bool
TCEPassConfig::addPreISel() {
    LLVMTCECmdLineOptions* options =
        dynamic_cast<LLVMTCECmdLineOptions*>(Application::cmdLineOptions());
    if (options != NULL && !options->disableHWLoops() &&
        ((static_cast<TCETargetMachine*>(TM))->ttaMach_)
        ->hasOperation("hwloop"))
        addPass(createHardwareLoopsLegacyPass());

    // lower floating point stuff.. maybe could use plugin as param instead machine...
    addPass(createLowerMissingInstructionsPass(
                *((static_cast<TCETargetMachine*>(TM))->ttaMach_)));

    if ((static_cast<TCETargetMachine*>(TM))->emulationModule_ != NULL) {
        addPass(createLinkBitcodePass(
                  *((static_cast<TCETargetMachine*>(TM))->emulationModule_)));
    }

    // NOTE: This must be added before Machine function analysis pass..
    // needed by POMBuilder to prevent writing debug data to data section
    // might be good to disable when printing out machine function code...
    // However, it need to comment out to support debug info
//    PM.add(createStripSymbolsPass(/*bool OnlyDebugInfo=*/true));

    return false;
}

/**
 * Creates a TTAMachine::Machine object of the target architecture.
 */
TTAMachine::Machine*
TCETargetMachine::createMachine() {
    LLVMTCECmdLineOptions* options =
        dynamic_cast<LLVMTCECmdLineOptions*>(Application::cmdLineOptions());

    assert(options->isMachineFileDefined() && "ADF not defined");
    assert(options->machineFile() != "" && "ADF not defined");
    ADFSerializer serializer;
    serializer.setSourceFile(options->machineFile());
    return serializer.readMachine();
}

/**
 * Returns list of llvm::ISD SelectionDAG opcodes for operations that are not
 * supported in the target architecture.
 *
 * The returned operations have to be expanded to emulation function calls
 * or emulation patterns in TCETargetLowering.
 */
const std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> >*
TCETargetMachine::missingOperations() {
    return &missingOps_;
}

/**
 * Returns list of llvm::ISD SelectionDAG opcodes for operations that are not
 * supported in the target architecture but will be promoted.
 *
 * The returned operations have to be promoted to emulation function calls
 * or emulation patterns in TCETargetLowering.
 */
const std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> >*
TCETargetMachine::promotedOperations() {
    return &promotedOps_;
}

/**
 * Returns list of llvm::ISD SelectionDAG opcodes for operations that are not
 * supported in the target architecture but will be custom-selected.
 */
const std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> >*
TCETargetMachine::customLegalizedOperations() {
    return &customLegalizedOps_;
}

TargetPassConfig* 
TCETargetMachine::createPassConfig(
    PassManagerBase &PM) {
    TCEPassConfig *tpc;
    tpc =  new TCEPassConfig(this, PM, plugin_);
    // TODO: is this false by default if not set?
    // false seems to give slightly better performance than true.
    tpc->setEnableTailMerge(false);
    return tpc;
}

/**
 * Returns true if the value can be encoded as immediate to register.
 */
bool
TCETargetMachine::canEncodeAsMOVI(const llvm::MVT& vt, int64_t val) const {
    // setTargetMachinePlugin() calls calculateSupportedImmediateLimits.
    assert(ttaMach_ && "setTargetMachinePlugin() was not called");
    switch (vt.SimpleTy) {
        case MVT::i1:
            return (1 <= largestImm_);
        case MVT::i8:
        case MVT::i16:
        case MVT::i32:
        case MVT::i64:
            return smallestImm_ == INT64_MIN || largestImm_ == UINT64_MAX ||
                ((int64_t)smallestImm_ <= val &&
                (val < 0 || val <= (int64_t)largestImm_));
        default: assert(false && "Not implemented or supported.");
    }
    return false;
}

/**
 * Returns true if the floating point value can be encoded as immediate to
 * register.
 */
bool
TCETargetMachine::canEncodeAsMOVF(const llvm::APFloat& fp) const {
    int fpBitWidth = static_cast<int>(
        llvm::APFloat::getSizeInBits(fp.getSemantics()));
    return fpBitWidth <= SupportedFPImmWidth_;
}

void
TCETargetMachine::calculateSupportedImmediates() {
    using MCC = MachineConnectivityCheck;

    // FIX ME: All 32 bit regs are assumed to be RF-connected.
    std::pair<int64_t, uint64_t> moveImm{
        std::numeric_limits<int64_t>::max(),
        std::numeric_limits<uint64_t>::min() };
    assert(ttaMach_);
    for (auto& rf : ttaMach_->registerFileNavigator()) {
        if (rf->width() != 32) continue;

        for (auto& bus : ttaMach_->busNavigator()) {
            if (!MCC::busConnectedToRF(*bus, *rf)
                || bus->immediateWidth() == 0) {
                continue;
            }

            if (bus->immediateWidth() >= 32) {
                moveImm.first = -(1ll << (32-1));
                moveImm.second = (1ll << 32)-1;
                break;
            } else {
                std::pair<int64_t, uint64_t> imm =
                    MathTools::bitsToIntegerRange<int64_t, uint64_t>(
                        bus->immediateWidth(),
                        bus->signExtends());

                moveImm.first = std::min(moveImm.first, imm.first);
                moveImm.second = std::max(moveImm.second, imm.second);
            }
        }
    }

    for (auto& iu : ttaMach_->immediateUnitNavigator()) {
        for (auto& it : ttaMach_->instructionTemplateNavigator()) {
            int supportedWidth = it->supportedWidth(*iu);
            if (supportedWidth >= 32) {
                moveImm.first = -(1ll << (32-1));
                moveImm.second = (1ll << 32)-1;
                break;
            } else {
                std::pair<int64_t, uint64_t> imm =
                    MathTools::bitsToIntegerRange<int64_t, uint64_t>(
                        supportedWidth, iu->signExtends());

                moveImm.first = std::min(moveImm.first, imm.first);
                moveImm.second = std::max(moveImm.second, imm.second);
            }
        }
    }
    smallestImm_ = moveImm.first;
    largestImm_ = moveImm.second;

    for (auto* rf : ttaMach_->registerFileNavigator()) {
        if (!MCC::rfConnected(*rf)) continue;

        SupportedFPImmWidth_ = std::max(
            SupportedFPImmWidth_,
            ImmInfo::registerImmediateLoadWidth(*rf, false));
    }
}

void
TCEPassConfig::addPreSched2() {
    addPass(&IfConverterID);
}

int TCETargetMachine::getLoadOpcode(int asid, int align, const llvm::EVT& vt) const {
    int laneCount = vt.getVectorElementCount().getKnownMinValue();

    int laneSize = vt.getScalarSizeInBits();
    int vecSize = laneCount * laneSize;
    TCEString relaxedName = "LD"; relaxedName << laneSize << "X" << laneCount;
    TCEString strictName = "LD"; strictName << vecSize;
    bool allowStrict = vecSize <= align;

    auto fuNav = ttaMach_->functionUnitNavigator();
    bool found = false;
    for (int i = 0; i < fuNav.count(); i++) {
        auto fu =  fuNav.item(i);
        if (fu->hasOperation(relaxedName) && fu->hasAddressSpace()) {
            auto as = fu->addressSpace();
            if (as->hasNumericalId(asid)) {
                found = true;
                break;
            }
        }
        if (allowStrict) {
            if (fu->hasOperation(strictName) && fu->hasAddressSpace()) {
                auto as = fu->addressSpace();
                if (as->hasNumericalId(asid)) {
                    found = true;
                    break;
                }
            }
        }
    }
    return found ? plugin_->getLoadOpcode(vt) : -1;
}
