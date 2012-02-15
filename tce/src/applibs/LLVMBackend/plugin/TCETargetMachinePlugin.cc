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
 * @file TCETargetMachinePlugin.cc
 *
 * Implementation of TCETargetMachinePlugin class.
 *
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

//#include <llvm/Config/config.h>
#include <string>
#include <iostream>
#include "TCETargetMachinePlugin.hh"
#include "TCEPlugin.hh"
#include "TCEInstrInfo.hh"
#include "TCETargetMachine.hh"
#include "TCEFrameInfo.hh"
#include "TCEISelLowering.hh"
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

    virtual FunctionPass* createISelPass(TCETargetMachine* tm);

    virtual unsigned spDRegNum() {
        return TCE::SP;
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
    virtual std::string operationName(unsigned opc);

    virtual bool hasOperation(TCEString operationName) const {
        return MapTools::containsValue(opNames_, operationName.upper());
    }

    virtual unsigned opcode(TCEString operationName) const {
        return MapTools::keyForValue<unsigned>(opNames_, operationName.upper());
    }

    unsigned int raPortDRegNum();
    std::string dataASName();

    virtual bool hasSDIV() const;
    virtual bool hasUDIV() const;
    virtual bool hasSREM() const;
    virtual bool hasUREM() const;
    virtual bool hasMUL() const;
    virtual bool hasROTL() const;
    virtual bool hasROTR() const;
    virtual bool hasSXHW() const;
    virtual bool hasSXQW() const;
    virtual bool hasSQRTF() const;

    virtual void registerTargetMachine(TCETargetMachine &tm);

private:
    void initialize();
    
    std::map<unsigned, TCEString> opNames_;
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

   instrInfo_ = new TCEInstrInfo();
   // Initialize register & opcode maps.
   initialize();
   frameInfo_ = new TCEFrameInfo(
       static_cast<const TCERegisterInfo*>(getRegisterInfo()));
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
    if (lowering_ == NULL) {
        lowering_ = new TCETargetLowering(*tm_);
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

/**
 * Creates instruction selector for TCE target machine.
 */
FunctionPass*
GeneratedTCEPlugin::createISelPass(TCETargetMachine* tm) {
    return createTCEISelDag(*tm);
}

/**
 * Maps llvm target opcodes to target operation names.
 */
std::string
GeneratedTCEPlugin::operationName(unsigned opc) {

    const std::string MOVE = "MOVE";
    const std::string PSEUDO = "PSEUDO";
    const std::string NOP = "NOP";
    const std::string INLINEASM = "INLINEASM";
    const std::string SELECT = "SELECT";

    // Pseudo operations
    if (opc == TCE::SELECT_I1rr) return SELECT;
    else if (opc == TCE::SELECT_I1ri) return SELECT;
    else if (opc == TCE::SELECT_I1ir) return SELECT;
    else if (opc == TCE::SELECT_I1ii) return SELECT;

    else if (opc == TCE::SELECT_I8rr) return SELECT;

    else if (opc == TCE::SELECT_I32rr) return SELECT;
    else if (opc == TCE::SELECT_I32ri) return SELECT;
    else if (opc == TCE::SELECT_I32ir) return SELECT;
    else if (opc == TCE::SELECT_I32ii) return SELECT;

    else if (opc == TCE::SELECT_I64) return SELECT;
    else if (opc == TCE::SELECT_F32) return SELECT;
    else if (opc == TCE::SELECT_F64) return SELECT;
    else if (opc == TCE::IMPLICIT_DEF) return PSEUDO;
    else if (opc == TCE::ADJCALLSTACKDOWN) return PSEUDO;
    else if (opc == TCE::ADJCALLSTACKUP) return PSEUDO;
    else if (opc == TCE::NOP) return NOP;

    // Moves
    if (opc == TCE::MOVI1rr) return MOVE;
    if (opc == TCE::MOVI1ri) return MOVE;
    if (opc == TCE::MOVI8rr) return MOVE;
    if (opc == TCE::MOVI8ri) return MOVE;
    if (opc == TCE::MOVI16rr) return MOVE;
    if (opc == TCE::MOVI16ri) return MOVE;
    if (opc == TCE::MOVI32rr) return MOVE;
    if (opc == TCE::MOVI32ri) return MOVE;
    if (opc == TCE::MOVI64rr) return MOVE;
    if (opc == TCE::MOVI64ri) return MOVE;
    if (opc == TCE::MOVF32rr) return MOVE;
    if (opc == TCE::MOVF32ri) return MOVE;
    if (opc == TCE::MOVF64rr) return MOVE;
    if (opc == TCE::MOVF64ri) return MOVE;

    if (opc == TCE::MOVI8I1rr) return MOVE;
    if (opc == TCE::MOVI16I1rr) return MOVE;
    if (opc == TCE::MOVI16I8rr) return MOVE;
    if (opc == TCE::MOVI32I1rr) return MOVE;
    if (opc == TCE::MOVI32I8rr) return MOVE;
    if (opc == TCE::MOVI32I16rr) return MOVE;
    if (opc == TCE::MOVI64I1rr) return MOVE;
    if (opc == TCE::MOVI64I8rr) return MOVE;
    if (opc == TCE::MOVI64I16rr) return MOVE;
    if (opc == TCE::MOVI64I32rr) return MOVE;

    if (opc == TCE::MOVFI32rr) return MOVE;
    if (opc == TCE::MOVIF32rr) return MOVE;

    if (opc == TCE::INLINEASM) return INLINEASM;

    if (opc == TCE::ANDext) return "and";
    if (opc == TCE::XORbicmp) return "xor";

    if (opc == TCE::STQBrb) return "stq";
    if (opc == TCE::STQBib) return "stq";
    if (opc == TCE::STQBrj) return "stq";
    if (opc == TCE::STQBij) return "stq";

    if (opc == TCE::LDQBr) return "ldq";
    if (opc == TCE::LDQBi) return "ldq";
    if (opc == TCE::LDQUBr) return "ldqu";
    if (opc == TCE::LDQUBi) return "ldqu";

    // temporary RA register store/loads
    if (opc == TCE::STWRArr) return "stw";
    if (opc == TCE::LDWRAr) return "ldw";
    
    if (opc == TCE::TCEBRCOND) return "?jump";
    if (opc == TCE::TCEBRICOND) return "!jump";
    if (opc == TCE::TCEBR) return "jump";
    if (opc == TCE::CALL) return "call";
    if (opc == TCE::CALL_MEMrr) return "call";
    if (opc == TCE::CALL_MEMri) return "call";

    if (opNames_.find(opc) == opNames_.end()) {
        std::cerr << "ERROR: Couldn't find operation with opc: " << opc
                  << std::endl;
        std::cerr << "Total ops: " << opNames_.size() << std::endl;
        abort();
    }

    return opNames_[opc];
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

