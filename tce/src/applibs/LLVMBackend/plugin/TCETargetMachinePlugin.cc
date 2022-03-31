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
 * @file TCETargetMachinePlugin.cc
 *
 * Implementation of TCETargetMachinePlugin class.
 *
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2012
 * @note rating: red
 */

//#include <llvm/Config/config.h>
#include <string>
#include <iostream>
#include <sstream>
#include "TCETargetMachinePlugin.hh"
#include "TCEPlugin.hh"
#include "TCETargetMachine.hh"
#include "TCEFrameInfo.hh"
#include "TCEISelLowering.hh"
#include "TCESubtarget.hh"
#include "MapTools.hh"
#include "CIStringSet.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "TCEStubTargetMachine.hh"
#include "TCETargetTransformInfo.hh"

using namespace llvm;

using std::string;

namespace llvm {
class GeneratedTCEPlugin : public TCETargetMachinePlugin {
public:
    GeneratedTCEPlugin();
    virtual ~GeneratedTCEPlugin();
    virtual const TargetInstrInfo* getInstrInfo() const override;
    virtual const TargetRegisterInfo* getRegisterInfo() const override;
    virtual const TargetFrameLowering* getFrameLowering() const override;
    virtual TargetLowering* getTargetLowering() const override;
    virtual TargetTransformInfo getTargetTransformInfo(
        const Function& F) const override;
    virtual const TargetSubtargetInfo* getSubtarget() const override;

    virtual FunctionPass* createISelPass(TCETargetMachine* tm) override;

    virtual unsigned spDRegNum() override {
        return TCE::SP;
    }

    virtual unsigned fpDRegNum() override {
        return TCE::FP;
    }

    virtual unsigned rvDRegNum() override {
        return TCE::IRES0;
    }

    virtual unsigned rvHighDRegNum() override {
        return TCE::KLUDGE_REGISTER;
    }

    virtual std::vector<unsigned> getParamDRegNums() const override;

    virtual std::vector<unsigned> getVectorRVDRegNums() const override;

    virtual const std::string* adfXML() override {
        return &adfXML_;
    }

    virtual MVT::SimpleValueType getDefaultType() const override {
#ifdef TARGET64BIT
        return MVT::i64;
#else
        return MVT::i32;
#endif
    }
    virtual std::string rfName(unsigned dwarfRegNum) override;
    virtual unsigned registerIndex(unsigned dwarfRegNum) override;

    unsigned llvmRegisterId(const TCEString& ttaRegister) override {
        if (ttallvmRegMap_.count(ttaRegister)) {
            return ttallvmRegMap_[ttaRegister];
        }
        return TCE::NoRegister;
    }

    unsigned int extractElementLane(const llvm::MachineInstr&) const override;

    virtual std::string operationName(unsigned opc) const override;
    virtual bool validStackAccessOperation(
        const std::string& opName) const override;

    virtual bool hasOperation(TCEString operationName) const override {
        return MapTools::containsValue(opNames_, operationName.upper());
    }

    virtual unsigned opcode(TCEString operationName) const override {
        return MapTools::keyForValue<unsigned>(opNames_, operationName.upper());
    }

    virtual int getTruePredicateOpcode(unsigned opc) const override;
    virtual int getFalsePredicateOpcode(unsigned opc) const override;

    unsigned int raPortDRegNum() override;
    std::string dataASName() override;

    virtual bool hasSDIV() const override;
    virtual bool hasUDIV() const override;
    virtual bool hasSREM() const override;
    virtual bool hasUREM() const override;
    virtual bool hasMUL() const override;
    virtual bool hasROTL() const override;
    virtual bool hasROTR() const override;
    virtual bool hasSXHW() const override;
    virtual bool hasSXQW() const override;
    virtual bool hasSQRTF() const override;
    virtual bool hasSHR() const override;
    virtual bool hasSHL() const override;
    virtual bool hasSHRU() const override;
    virtual bool has8bitLoads() const override;
    virtual bool has16bitLoads() const override;

    virtual int maxVectorSize() const override;

    virtual bool analyzeCCBranch(
        llvm::MachineInstr& i,
        llvm::SmallVectorImpl<llvm::MachineOperand>& cond) const override;

    virtual void registerTargetMachine(TCETargetMachine &tm) override;

    virtual int getLoad(const TargetRegisterClass *rc) const override;
    virtual int getStore(const TargetRegisterClass *rc) const override;

    virtual int getMinOpcode(llvm::SDNode* n) const override;
    virtual int getMaxOpcode(llvm::SDNode* n) const override;
    virtual int getMinuOpcode(llvm::SDNode* n) const override;
    virtual int getMaxuOpcode(llvm::SDNode* n) const override;

    virtual const llvm::TargetRegisterClass* extrasRegClass(
        const llvm::TargetRegisterClass* current) const override;
    virtual const llvm::TargetRegisterClass* nodeRegClass(
        unsigned nodeId,
        const llvm::TargetRegisterClass* current) const override;

    virtual int getLoadOpcode(const EVT& vt) const override;
    // -1 or vector add opcode. Implementation generated to Backend.inc
    virtual int getAddOpcode(const EVT& vt) const override;
    // -1 or vector shl opcode. Implementation generated to Backend.inc
    virtual int getShlOpcode(const EVT& vt) const override;
    // -1 or vector or opcode. Implementation generated to Backend.inc
    virtual int getIorOpcode(const EVT& vt) const override;

    // If machine is little-endian.
    virtual bool isLittleEndian() const override;
    virtual bool is64bit() const override;

    // Returns true if the constant value can be materialized. Implementation
    // is generated to Backend.inc
    virtual bool canMaterializeConstant(const ConstantInt& ci) const override;

    virtual std::tuple<int, int> getPointerAdjustment(
        int offset) const override;
    virtual void manualInitialize();

private:
    void initialize();

    std::map<unsigned, TCEString> opNames_;
    std::map<unsigned, unsigned> truePredOps_;
    std::map<unsigned, unsigned> falsePredOps_;
    std::map<unsigned, TCEString> regNames_;
    std::map<unsigned, unsigned> regIndices_;
    /// Map for TTA register to LLVM register id conversion.
    std::map<TCEString, unsigned> ttallvmRegMap_;
    /// Set of valid LLVM opcodes for stack accesses.
    TCETools::CIStringSet validStackAccessOperations_;

    std::string adfXML_;
    std::string dataASName_;
};
}

#include "Backend.inc"


// This global class initialization hack is needed to pass the adf data to the
// middle-end, which provides details for llvm autovectorization.
class TCEPluginInitializer {
public:
    TCEPluginInitializer() {

        GeneratedTCEPlugin* plugin = new GeneratedTCEPlugin();
        plugin->manualInitialize();

        // Pass adf xml string to the middle-end stub targetmachine
        TCEStubTargetMachine::setADFString(*(plugin->adfXML()));

        // Register LLVM Target and TCE Stub TargetMachine
        LLVMInitializeTCETargetInfo();
        LLVMInitializeTCEStubTarget();
    }
} myPlugin;

/**
 * The Constructor.
 */
GeneratedTCEPlugin::GeneratedTCEPlugin() : 
    TCETargetMachinePlugin() {
}


/**
 * The Destructor.
 */
GeneratedTCEPlugin::~GeneratedTCEPlugin() {
   delete instrInfo_;
   if (lowering_ != NULL) {
       delete lowering_;
       lowering_ = NULL;
   }
}

void
GeneratedTCEPlugin::manualInitialize() {
    initialize();
}

void 
GeneratedTCEPlugin::registerTargetMachine(
    TCETargetMachine &tm) {
    tm_ = &tm;
    instrInfo_ = new TCEInstrInfo(this);
    // Initialize register & opcode maps.
    initialize();

    TCERegisterInfo* ri =
        const_cast<TCERegisterInfo*>(
            static_cast<const TCERegisterInfo*>(getRegisterInfo()));
    frameInfo_ = new TCEFrameLowering(ri, instrInfo_, tm.stackAlignment());

    if (isLittleEndian()) {
#ifndef TARGET64BIT
        subTarget_ = new TCELESubtarget(this);
#else
        subTarget_ = new TCELE64Subtarget(this);
#endif
    } else {
        subTarget_ = new TCESubtarget(this);
    }

    if (lowering_ == NULL) {
        lowering_ = new TCETargetLowering(*tm_, *subTarget_);
    }
}

/**
 * Returns TargetInstrInfo object for TCE target.
 */
const TargetInstrInfo*
GeneratedTCEPlugin::getInstrInfo() const {
    return instrInfo_;
}

/**
 * Returns TargetRegisterInfo object for TCE target.
 */
const TargetRegisterInfo*
GeneratedTCEPlugin::getRegisterInfo() const {
    return &(static_cast<TCEInstrInfo*>(instrInfo_))->getRegisterInfo();
}

/**
 * Returns TargetFrameInfo object for TCE target.
 */
const TargetFrameLowering* 
GeneratedTCEPlugin::getFrameLowering() const {
    return frameInfo_;
}

TargetTransformInfo
GeneratedTCEPlugin::getTargetTransformInfo(const Function& F) const {
    return TargetTransformInfo(TCETTIImpl(tm_, F));
}

/**
 * Returns TargetLowering object for TCE target.
 */
TargetLowering* 
GeneratedTCEPlugin::getTargetLowering() const { 
    assert(lowering_ != NULL && "TCETargetMachine has not registered to plugin.");
    return lowering_;
}

const TargetSubtargetInfo* 
GeneratedTCEPlugin::getSubtarget() const {
    return subTarget_;
}

/**
 * Creates instruction selector for TCE target machine.
 */
FunctionPass*
GeneratedTCEPlugin::createISelPass(TCETargetMachine* tm) {
    return createTCEISelDag(*tm);
}

/**
 * Returns true in case the given opc is for an extract element
 * operation.
 */
unsigned int
GeneratedTCEPlugin::extractElementLane(const llvm::MachineInstr& mi) const {
    // TODO: use subregisters (extract_subreg) instead.
    if (mi.getDesc().getOpcode() == TCE::COPY) {
        assert(mi.getNumOperands() >= 2);
        const MachineOperand& src = mi.getOperand(1);
        int subreg = src.getSubReg();
        if (subreg > 0 && subreg < 9) {
            return subreg -1;
        }
    }
    return UINT_MAX;
}

int GeneratedTCEPlugin::getTruePredicateOpcode(unsigned opc) const {
    std::map<unsigned int, unsigned int>::const_iterator i = truePredOps_.find(opc);
    if (i == truePredOps_.end()) {
        return -1;
    } else {
        return i->second;
    }
}

int GeneratedTCEPlugin::getFalsePredicateOpcode(unsigned opc) const {
    std::map<unsigned int, unsigned int>::const_iterator i = falsePredOps_.find(opc);
    if (i == falsePredOps_.end()) {
        return -1;
    } else {
        return i->second;
    }
}

/**
 * Maps llvm target opcodes to target operation names.
 *
 * Returns an empty string in case the operation code could not be
 * mapped to an OSAL operation name. This is the case with pseudo
 * ops, for example.
 */
std::string
GeneratedTCEPlugin::operationName(unsigned opc) const {

    const std::string MOVE = "MOVE";
    const std::string PSEUDO = "PSEUDO";
    const std::string NOP = "NOP";
    const std::string INLINEASM = "INLINEASM";
    const std::string DEBUG_LABEL = "DEBUG_LABEL";
    const std::string DEBUG_VALUE = "DEBUG_VALUE";
    const std::string DEBUG_INSTR_REF = "DEBUG_INSTR_REF";
    const std::string DEBUG_VALUE_LIST = "DEBUG_VALUE_LIST";
    const std::string DEBUG_PHI = "DEBUG_PHI";

    if (opc == TCE::IMPLICIT_DEF) return PSEUDO;
    else if (opc == TCE::ADJCALLSTACKDOWN) return PSEUDO;
    else if (opc == TCE::ADJCALLSTACKUP) return PSEUDO;
    else if (opc == TCE::NOP) return NOP;
    if (opc == TCE::DBG_VALUE) return DEBUG_VALUE;
    if (opc == TCE::DBG_INSTR_REF) return DEBUG_INSTR_REF;
    if (opc == TCE::DBG_LABEL) return DEBUG_LABEL;
    if (opc == TCE::DBG_VALUE_LIST) return DEBUG_VALUE_LIST;
    if (opc == TCE::DBG_PHI) return DEBUG_PHI;
    // Moves
    if (opc == TCE::COPY) return MOVE;
    if (opc == TCE::MOVI1rr) return MOVE;
    if (opc == TCE::PRED_TRUE_MOVI1rr) return "?MOVE";
    if (opc == TCE::PRED_FALSE_MOVI1rr) return "!MOVE";
    if (opc == TCE::MOVI1ri) return MOVE;
    if (opc == TCE::PRED_TRUE_MOVI1ri) return "?MOVE";
    if (opc == TCE::PRED_FALSE_MOVI1ri) return "!MOVE";
    if (opc == TCE::MOVGri) return MOVE;
    if (opc == TCE::MOVI32rr) return MOVE;
    if (opc == TCE::PRED_TRUE_MOVI32rr) return "?MOVE";
    if (opc == TCE::PRED_FALSE_MOVI32rr) return "!MOVE";
    if (opc == TCE::MOVI32ri) return MOVE;
    if (opc == TCE::PRED_TRUE_MOVI32ri) return "?MOVE";
    if (opc == TCE::PRED_FALSE_MOVI32ri) return "!MOVE";
    if (opc == TCE::MOVI64sa) return MOVE;
    if (opc == TCE::PRED_TRUE_MOVI64sa) return "?MOVE";
    if (opc == TCE::PRED_FALSE_MOVI64sa) return "!MOVE";
    if (opc == TCE::MOV64ss) return MOVE;
    if (opc == TCE::PRED_TRUE_MOV64ss) return "?MOVE";
    if (opc == TCE::PRED_FALSE_MOV64ss) return "!MOVE";
    if (opc == TCE::MOVI64I1ss) return MOVE;

    // TODO: why no predicated version of this?
    if (opc == TCE::MOVff) return MOVE;
    if (opc == TCE::MOVfi) return MOVE;
    if (opc == TCE::MOVfk) return MOVE;
    if (opc == TCE::MOVI32I1rr) return MOVE;
    if (opc == TCE::MOVFI32rf) return MOVE;
    if (opc == TCE::MOVIF32fr) return MOVE;
    if (opc == TCE::MOVGrr) return MOVE;
    if (opc == TCE::MOVGI32rr) return MOVE;
    if (opc == TCE::MOVI32Grr) return MOVE;
    if (opc == TCE::MOVGI1rr) return MOVE;
    if (opc == TCE::MOVI1Grr) return MOVE;
    if (opc == TCE::MOVhh) return MOVE;
    if (opc == TCE::MOVhk) return MOVE;
    if (opc == TCE::MOVrh) return MOVE;
    if (opc == TCE::MOVhr) return MOVE;
    if (opc == TCE::MOVhi) return MOVE;
    if (opc == TCE::MOVsd) return MOVE;
    if (opc == TCE::MOVds) return MOVE;

    // TODO: added as a spot fix for simd_operations_On,
    // which selects this particular move operation.
    // Most likely, the compiler can pick other moves not
    // listed here, and subsequently crash.
    if (opc == TCE::MOVI1I32rr) return MOVE;

    if (opc == TCE::INLINEASM) return INLINEASM;

/* TODO: 64-bit support missing here? */

    if (opc == TCE::ADDfri || opc == TCE::ADDhri) return "add";
    
    if (opc == TCE::SUBfir || opc == TCE::SUBfri || opc == TCE::SUBhir ||
        opc == TCE::SUBhri) return "sub";

    if (opc == TCE::SUBfir) return "sub";
    if (opc == TCE::SUBfri) return "sub";

#ifdef TARGET64BIT
    if (opc == TCE::ANDext) return "and64";
    if (opc == TCE::XORbicmp) return "xor64";
#else
    if (opc == TCE::ANDext) return "and";
    if (opc == TCE::XORbicmp) return "xor";
#endif


    if (opc == TCE::STQBrb) return "stq";
    if (opc == TCE::STQBib) return "stq";
    if (opc == TCE::STQBrj) return "stq";
    if (opc == TCE::STQBij) return "stq";

    if (opc == TCE::ST8Brb) return "st8";
    if (opc == TCE::ST8Bib) return "st8";
    if (opc == TCE::ST8Brj) return "st8";
    if (opc == TCE::ST8Bij) return "st8";

    // temporary RA register store/loads
#ifdef TARGET64BIT
    if (opc == TCE::ST64RAss) return "st64";
    if (opc == TCE::LD64RAs) return "ld64";
#else
    if (opc == TCE::STWRArr) return "stw";
    if (opc == TCE::LDWRAr) return "ldw";
    if (opc == TCE::ST32RArr) return "st32";
    if (opc == TCE::LD32RAr) return "ld32";
#endif
    if (opc == TCE::TCEBR) return "jump";
    if (opc == TCE::TCEBRIND) return "jump";
    if (opc == TCE::CALL) return "call";
    if (opc == TCE::CALL_MEMrr) return "call";
    if (opc == TCE::CALL_MEMri) return "call";

    std::map<unsigned int, TCEString>::const_iterator opNameIt;

    std::map<unsigned int, TCEString>::const_iterator i = opNames_.find(opc);
    if (i == opNames_.end()) {
        std::cerr << "ERROR: Couldn't find operation with opc: " << opc
                  << std::endl;
        std::cerr << "Total ops: " << opNames_.size() << std::endl;
/*
        abort();
*/
        return "";
    } else {
        return i->second;
    }
}

bool
GeneratedTCEPlugin::validStackAccessOperation(const std::string& opName) const {
    return validStackAccessOperations_.count(opName);
}

#include "TCEGenRegisterInfo.inc"

/**
 * Returns the specific register class representing registers belonging to 
 * a "computation node" inside a clustered-TTA.
 *
 * This can be used to limit the registers to be allocated from a single
 * node, thus also suggest scheduling the operations there. For data 
 * parallel code.
 *
 * @param nodeId the ID of the node (starting from 0)
 * @param current The "more general" register class to replace. 
 */
const llvm::TargetRegisterClass*
GeneratedTCEPlugin::nodeRegClass(
    unsigned nodeId, const llvm::TargetRegisterClass* current) const {

    return NULL;
}


/**
 * Returns the specific register class representing registers belonging to 
 * the "extras node" inside a clustered-TTA.
 *
 * @param current The "more general" register class to replace. 
 */
const llvm::TargetRegisterClass*
GeneratedTCEPlugin::extrasRegClass(
    const llvm::TargetRegisterClass* current) const {
    const llvm::TargetRegisterInfo& TRI = *getRegisterInfo();

    return NULL;
}


/**
 * Maps llvm register numbers to target RF names.
 */
std::string
GeneratedTCEPlugin::rfName(unsigned dwarfRegNum) {
    if (regNames_.find(dwarfRegNum) == regNames_.end()) {
        return "BYPASS_PSEUDO";
    }
    return regNames_[dwarfRegNum];
}

/**
 * Maps llvm register numbers to target RF indices.
 */
unsigned
GeneratedTCEPlugin::registerIndex(unsigned dwarfRegNum) {
    if (regIndices_.find(dwarfRegNum) == regIndices_.end()) {
        return dwarfRegNum;
    }
    return regIndices_[dwarfRegNum];
}


std::string
GeneratedTCEPlugin::dataASName() {
    return dataASName_;
}
// Returns ID number of the return address register.
unsigned
GeneratedTCEPlugin::raPortDRegNum() {
    return TCE::RA;
}

extern "C" {                                
    TCETargetMachinePlugin*
    create_tce_backend_plugin() {

        TCETargetMachinePlugin* instance =
            new GeneratedTCEPlugin();
        return instance;
    }
    void delete_tce_backend_plugin(
        TCETargetMachinePlugin* target) {
        delete target;
    }
}

