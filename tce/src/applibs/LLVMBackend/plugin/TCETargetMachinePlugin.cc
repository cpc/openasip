/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file TCETargetMachinePlugin.cc
 *
 * Implementation of TCETargetMachinePlugin class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <iostream>
#include "TCETargetMachinePlugin.hh"
#include "TCEPlugin.hh"
#include "TCEInstrInfo.hh"
#include "TCEAsmPrinter.hh"
#include "TCETargetMachine.hh"

using namespace llvm;

using std::string;

namespace llvm {
class GeneratedTCEPlugin : public TCETargetMachinePlugin {
public:
    GeneratedTCEPlugin();
    virtual ~GeneratedTCEPlugin();
    virtual const TargetInstrInfo* getInstrInfo() const;
    virtual const TargetRegisterInfo* getRegisterInfo() const;
    virtual FunctionPass* createISelPass(TCETargetMachine* tm);
    virtual FunctionPass* createAsmPrinterPass(
        std::ostream& o, TCETargetMachine* tm);

    virtual unsigned spDRegNum() {
        return TCE::SP;
    }
   
    virtual const std::string* adfXML() {
        return &adfXML_;
    }

    virtual std::string rfName(unsigned dwarfRegNum);
    virtual unsigned registerIndex(unsigned dwarfRegNum);
    virtual std::string operationName(unsigned opc);

    unsigned int raPortDRegNum();
    std::string dataASName();

    virtual bool hasSDIV() const;
    virtual bool hasUDIV() const;
    virtual bool hasSREM() const;
    virtual bool hasUREM() const;
    virtual bool hasMUL() const;
    virtual bool hasROTL() const;
    virtual bool hasROTR() const;

private:
    void initialize();
    
    std::map<unsigned, std::string> opNames_;
    std::map<unsigned, std::string> regNames_;
    std::map<unsigned, unsigned> regIndices_;

    std::string adfXML_;
    std::string dataASName_;
};
}

#include "Backend.inc"

/**
 * The Constructor.
 */
GeneratedTCEPlugin::GeneratedTCEPlugin(): TCETargetMachinePlugin() {

   instrInfo_ = new TCEInstrInfo();

   // Initialize register & opcode maps.
   initialize();
}


/**
 * The Destructor.
 */
GeneratedTCEPlugin::~GeneratedTCEPlugin() {
   delete instrInfo_;
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
    return &(dynamic_cast<TCEInstrInfo*>(instrInfo_))->getRegisterInfo();
}

/**
 * Creates instruction selector for TCE target machine.
 */
FunctionPass*
GeneratedTCEPlugin::createISelPass(TCETargetMachine* tm) {
    return createTCEISelDag(*tm);
}

/**
 * Creates asm printer pass for TCE target machine.
 */
FunctionPass*
GeneratedTCEPlugin::createAsmPrinterPass(
    std::ostream& o, TCETargetMachine* tm) {

    return new TCEAsmPrinter(o, *tm, tm->getTargetAsmInfo());
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
    if (opc == TCE::SELECT_I1) return SELECT;
    else if (opc == TCE::SELECT_I8) return SELECT;
    else if (opc == TCE::SELECT_I16) return SELECT;
    else if (opc == TCE::SELECT_I32) return SELECT;
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

    if (opc == TCE::MOVI1I32rr) return MOVE;

    if (opc == TCE::INLINEASM) return INLINEASM;

    if (opc == TCE::STQib) return "stq";
    if (opc == TCE::STQrb) return "stq";
    if (opc == TCE::LDQUb) return "ldqu";
    if (opc == TCE::LDQUib) return "ldqu";
    
    if (opc == TCE::TCEBRCOND) return "?jump";
    if (opc == TCE::TCEBR) return "jump";
    if (opc == TCE::CALL) return "call";
    if (opc == TCE::CALL_MEMrr) return "call";
    if (opc == TCE::CALL_MEMri) return "call";

    if (opNames_.find(opc) == opNames_.end()) {
        std::cerr << "ERROR: Couldn't find operation with opc: " << opc
                  << std::endl;
        std::cerr << "Total ops: " << opNames_.size() << std::endl;
        assert(false);
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
        TCETargetMachinePlugin* instance = new GeneratedTCEPlugin();
        return instance;
    }
    void delete_tce_backend_plugin(
        TCETargetMachinePlugin* target) {
        delete target;
    }
}

