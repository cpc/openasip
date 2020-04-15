/*
    Copyright (c) 2002-2013 Tampere University.

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
#include "TCEInstrInfo.hh"
#include "TCETargetMachine.hh"
#include "TCEFrameInfo.hh"
#include "TCEISelLowering.hh"
#include "TCESubtarget.hh"
#include "MapTools.hh"

using namespace llvm;

using std::string;

namespace llvm {
class GeneratedTCEPlugin : public TCETargetMachinePlugin {
public:
    GeneratedTCEPlugin();
    virtual ~GeneratedTCEPlugin();
    virtual const TargetInstrInfo* getInstrInfo() const;
    virtual const TargetRegisterInfo* getRegisterInfo() const;
    virtual const TargetFrameLowering* getFrameLowering() const;
    virtual TargetLowering* getTargetLowering() const;
    virtual const TargetSubtargetInfo* getSubtarget() const;

    virtual FunctionPass* createISelPass(TCETargetMachine* tm);

    virtual unsigned spDRegNum() {
        return TCE::SP;
    }

    virtual unsigned fpDRegNum() {
        return TCE::FP;
    }

    virtual unsigned rvDRegNum() {
        return TCE::IRES0;
    }

    virtual unsigned rvHighDRegNum() {
        return TCE::KLUDGE_REGISTER;
    }

    virtual const std::string* adfXML() {
        return &adfXML_;
    }

    virtual std::string rfName(unsigned dwarfRegNum);
    virtual unsigned registerIndex(unsigned dwarfRegNum);

    unsigned int extractElementLane(const llvm::MachineInstr&) const;

    virtual std::string operationName(unsigned opc) const;

    virtual bool hasOperation(TCEString operationName) const {
        return MapTools::containsValue(opNames_, operationName.upper());
    }

    virtual unsigned opcode(TCEString operationName) const {
        return MapTools::keyForValue<unsigned>(opNames_, operationName.upper());
    }

    virtual int getTruePredicateOpcode(unsigned opc) const;
    virtual int getFalsePredicateOpcode(unsigned opc) const;

    unsigned int raPortDRegNum();
    std::string dataASName();

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

    virtual int maxVectorSize() const;

    virtual void registerTargetMachine(TCETargetMachine &tm);

    virtual int getLoad(const TargetRegisterClass *rc) const;
    virtual int getStore(const TargetRegisterClass *rc) const;

    virtual int getMinOpcode(llvm::SDNode* n) const;
    virtual int getMaxOpcode(llvm::SDNode* n) const;
    virtual int getMinuOpcode(llvm::SDNode* n) const;
    virtual int getMaxuOpcode(llvm::SDNode* n) const;

    virtual const llvm::TargetRegisterClass* extrasRegClass(
        const llvm::TargetRegisterClass* current) const;
    virtual const llvm::TargetRegisterClass* nodeRegClass(
        unsigned nodeId, const llvm::TargetRegisterClass* current) const;

    virtual unsigned getMaxMemoryAlignment() const;

    // If machine is little-endian.
    virtual bool isLittleEndian() const override;

private:
    void initialize();
    
    std::map<unsigned, TCEString> opNames_;
    std::map<unsigned, unsigned> truePredOps_;
    std::map<unsigned, unsigned> falsePredOps_;
    std::map<unsigned, TCEString> regNames_;
    std::map<unsigned, unsigned> regIndices_;

    std::string adfXML_;
    std::string dataASName_;
};
}

#include "Backend.inc"

/**
 * The Constructor.
 */
GeneratedTCEPlugin::GeneratedTCEPlugin() : 
    TCETargetMachinePlugin() {
     int stackAlignment = static_cast<int>(getMaxMemoryAlignment());

     auto tii = new TCEInstrInfo(this);
     instrInfo_ = tii;
     // Initialize register & opcode maps.
     initialize();

     TCERegisterInfo* ri =
         const_cast<TCERegisterInfo*>(
             static_cast<const TCERegisterInfo*>(getRegisterInfo()));

     frameInfo_ = new TCEFrameLowering(ri, tii, stackAlignment);
     subTarget_ = new TCESubtarget(this);
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
GeneratedTCEPlugin::registerTargetMachine(
    TCETargetMachine &tm) {
    tm_ = &tm;

    if (isLittleEndian()) {
        subTarget_ = new TCELESubtarget(this);
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
 */
std::string
GeneratedTCEPlugin::operationName(unsigned opc) const {

    const std::string MOVE = "MOVE";
    const std::string PSEUDO = "PSEUDO";
    const std::string NOP = "NOP";
    const std::string INLINEASM = "INLINEASM";
    const std::string DEBUG_LABEL = "DEBUG_LABEL";

    if (opc == TCE::IMPLICIT_DEF) return PSEUDO;
    else if (opc == TCE::ADJCALLSTACKDOWN) return PSEUDO;
    else if (opc == TCE::ADJCALLSTACKUP) return PSEUDO;
    else if (opc == TCE::NOP) return NOP;
#ifndef LLVM_OLDER_THAN_7
    else if (opc == TCE::DBG_LABEL) return DEBUG_LABEL;
#endif
    // Moves
    if (opc == TCE::COPY) return MOVE;
    if (opc == TCE::MOVI1rr) return MOVE;
    if (opc == TCE::PRED_TRUE_MOVI1rr) return "?MOVE";
    if (opc == TCE::PRED_FALSE_MOVI1rr) return "!MOVE";
    if (opc == TCE::MOVI1ri) return MOVE;
    if (opc == TCE::PRED_TRUE_MOVI1ri) return "?MOVE";
    if (opc == TCE::PRED_FALSE_MOVI1ri) return "!MOVE";
    if (opc == TCE::MOVI32rr) return MOVE;
    if (opc == TCE::PRED_TRUE_MOVI32rr) return "?MOVE";
    if (opc == TCE::PRED_FALSE_MOVI32rr) return "!MOVE";
    if (opc == TCE::MOVI32ri) return MOVE;
    if (opc == TCE::PRED_TRUE_MOVI32ri) return "?MOVE";
    if (opc == TCE::PRED_FALSE_MOVI32ri) return "!MOVE";
    if (opc == TCE::MOVF32ff) return MOVE;
    if (opc == TCE::MOVF32fi) return MOVE;
    if (opc == TCE::MOVF32fk) return MOVE;
    if (opc == TCE::MOVI32I1rr) return MOVE;
    if (opc == TCE::MOVFI32rf) return MOVE;
    if (opc == TCE::MOVIF32fr) return MOVE;
    if (opc == TCE::MOVF16hh) return MOVE;
    if (opc == TCE::MOVF16hk) return MOVE;
    if (opc == TCE::MOVFI16rh) return MOVE;
    if (opc == TCE::MOVIF16hr) return MOVE;
    if (opc == TCE::MOVF16hi) return MOVE;

    if (opc == TCE::INLINEASM) return INLINEASM;

    if (opc == TCE::ANDext) return "and";
    if (opc == TCE::XORbicmp) return "xor";

    if (opc == TCE::STQBrb) return "stq";
    if (opc == TCE::STQBib) return "stq";
    if (opc == TCE::STQBrj) return "stq";
    if (opc == TCE::STQBij) return "stq";

    if (opc == TCE::ST8Brb) return "st8";
    if (opc == TCE::ST8Bib) return "st8";
    if (opc == TCE::ST8Brj) return "st8";
    if (opc == TCE::ST8Bij) return "st8";

    // temporary RA register store/loads
    if (opc == TCE::STWRArr) return "stw";
    if (opc == TCE::LDWRAr) return "ldw";
    if (opc == TCE::ST32RArr) return "st32";
    if (opc == TCE::LD32RAr) return "ld32";
    
    if (opc == TCE::TCEBRCOND) return "?jump";
    if (opc == TCE::TCEBRICOND) return "!jump";
    if (opc == TCE::TCEBR) return "jump";
    if (opc == TCE::TCEBRIND) return "jump";
    if (opc == TCE::CALL) return "call";
    if (opc == TCE::CALL_MEMrr) return "call";
    if (opc == TCE::CALL_MEMri) return "call";

    if (opc == TCE::BUILDV2vii) return "_BUILD_2";
    if (opc == TCE::BUILDV2mkk) return "_BUILD_2";
    if (opc == TCE::BUILDV4wiiii) return "_BUILD_4";
    if (opc == TCE::BUILDV4nkkkk) return "_BUILD_4";
    if (opc == TCE::BUILDV8xiiiiiiii) return "_BUILD_8";
    if (opc == TCE::BUILDV8okkkkkkkk) return "_BUILD_8";

    if (opc == TCE::MOV2vv) return "_VECTOR_MOV_2";
    if (opc == TCE::MOV2mm) return "_VECTOR_MOV_2";
    if (opc == TCE::MOV4ww) return "_VECTOR_MOV_4";
    if (opc == TCE::MOV4nn) return "_VECTOR_MOV_4";
    if (opc == TCE::MOV8xx) return "_VECTOR_MOV_8";
    if (opc == TCE::MOV8oo) return "_VECTOR_MOV_8";

    std::map<unsigned int, TCEString>::const_iterator i = opNames_.find(opc);
    if (i == opNames_.end()) {
        std::cerr << "ERROR: Couldn't find operation with opc: " << opc
                  << std::endl;
        std::cerr << "Total ops: " << opNames_.size() << std::endl;
        abort();
    } else {
	return i->second;
    }
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

#ifdef LLVM_3_5

    const llvm::TargetRegisterInfo& TRI = *getRegisterInfo();

    TCEString origRCName(current->getName());
    for (unsigned c = 0; c < TRI.getNumRegClasses(); ++c) {
        const llvm::TargetRegisterClass* regClass = TRI.getRegClass(c);
        TCEString RCName(regClass->getName());
        if (!current->hasSubClass(regClass) || !RCName.startsWith("R")) 
            continue;
        std::istringstream parser(origRCName);
        char c2;
        int width;
        TCEString suffix;
        parser >> c2;
        parser >> width;
        parser >> suffix;

        TCEString wantedRegClassName;
        wantedRegClassName << "R" << width << "_L_" << nodeId << suffix;

        if (wantedRegClassName == RCName)
            return regClass;
    }
    return current;
#else
    return NULL;
#endif
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

#ifdef LLVM_3_5
    TCEString origRCName(current->getName());
    for (unsigned c = 0; c < TRI.getNumRegClasses(); ++c) {
        const llvm::TargetRegisterClass* regClass = TRI.getRegClass(c);
        TCEString RCName(regClass->getName());
        if (!current->hasSubClass(regClass) || !RCName.startsWith("R")) 
            continue;
        std::istringstream parser(origRCName);
        char c2;
        int width;
        TCEString suffix;
        parser >> c2;
        parser >> width;
        parser >> suffix;

        TCEString wantedRegClassName;
        wantedRegClassName << "R" << width << "_Ex" << suffix;

        if (wantedRegClassName == RCName)
            return regClass;
    }
    return current;
#else
    return NULL;
#endif
}


/**
 * Maps llvm register numbers to target RF names.
 */
std::string
GeneratedTCEPlugin::rfName(unsigned dwarfRegNum) {
    assert(regNames_.find(dwarfRegNum) != regNames_.end());
    return regNames_[dwarfRegNum];
}

/**
 * Maps llvm register numbers to target RF indices.
 */
unsigned
GeneratedTCEPlugin::registerIndex(unsigned dwarfRegNum) {
    assert(regIndices_.find(dwarfRegNum) != regIndices_.end());
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

