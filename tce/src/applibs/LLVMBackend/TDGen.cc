/*
    Copyright (c) 2002-2012 Tampere University.

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
 * Architecture plugin generator for LLVM TCE backend.
 *
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 * @author Heikki Kultala 2012
 *
 * @note rating: red
 */

#include <fstream>
#include <algorithm>

#include "TDGen.hh"
#include "Machine.hh"
#include "ADFSerializer.hh"
#include "ControlUnit.hh"
#include "Operation.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "Conversion.hh"
#include "MachineConnectivityCheck.hh"
#include "Bus.hh"
#include "Guard.hh"
#include "StringTools.hh"
#include "OperationPool.hh"
#include "OperationNode.hh"
#include "TerminalNode.hh"
#include "ConstantNode.hh"
#include "OperationDAG.hh"
#include "OperationDAGEdge.hh"
#include "OperationDAGSelector.hh"
#include "TCEString.hh"
#include "Operand.hh"
#include "Application.hh"
#include "LLVMBackend.hh" // llvmRequiredOps..
#include "MathTools.hh"
#include "tce_config.h"
// SP, RES, KLUDGE, 2 GPRs?
unsigned const TDGen::REQUIRED_FULL_WIDTH_REGS = 5;


/**
 * Constructor.
 *
 * @param mach Machine to generate plugin for.
 */
TDGen::TDGen(const TTAMachine::Machine& mach) :
    mach_(mach), dregNum_(0),
    highestLaneInt_(-1), highestLaneBool_(-1),
    hasExBoolRegs_(false), hasExIntRegs_(false), hasSelect_(false),
    littleEndian_(mach.isLittleEndian()), use64bitForFP_(false) {
    tempRegFiles_ = MachineConnectivityCheck::tempRegisterFiles(mach);
    hasConditionalMoves_ =  
        MachineConnectivityCheck::hasConditionalMoves(mach_);
}

/**
 * Generates all files required to build a tce backend plugin
 * (excluding static plugin code included from include/llvm/TCE/).
 */
void
TDGen::generateBackend(std::string& path) {
    std::ofstream regTD;
    regTD.open((path + "/GenRegisterInfo.td").c_str());
    writeRegisterInfo(regTD);
    regTD.close();

    std::ofstream instrTD0;
    instrTD0.open((path + "/GenInstrInfo0.td").c_str());
    writeAddressingModeDefs(instrTD0);
    instrTD0.close();

    std::ofstream instrTD;
    instrTD.open((path + "/GenInstrInfo.td").c_str());
    writeInstrInfo(instrTD);
#ifdef DEBUG_TDGEN
    writeInstrInfo(std::cerr);
#endif
    instrTD.close();

    std::ofstream ccTD;
    ccTD.open((path + "/GenCallingConv.td").c_str());
    writeCallingConv(ccTD);
    ccTD.close();

    std::ofstream pluginInc;
    pluginInc.open((path + "/Backend.inc").c_str());
    writeBackendCode(pluginInc);
    pluginInc.close();

    std::ofstream topLevelTD;
    topLevelTD.open((path + "/TCE.td").c_str());
    writeTopLevelTD(topLevelTD);
    topLevelTD.close();
}

/**
 * Writes .td definition of a single register to the output stream.
 *
 * @param o Output stream to write the definition to.
 * @param reg Information about the physical register.
 * @param regName Name for the register in the llvm framework.
 * @param regTemplate Base class for the register.
 * @param aliases Comma-separated list of aliases for this register.
 */
void
TDGen::writeRegisterDef(
    std::ostream& o,
    const RegInfo& reg,
    const std::string regName,
    const std::string regTemplate,
    const std::string aliases,
    RegType type) {

    std::string templ = regTemplate;

    o << "def " << regName << " : " << templ
      << "<\"" << reg.rf << "." << reg.idx
      << "\", [" << aliases << "]>, DwarfRegNum<"
      << "[" << dregNum_ << "]>;"
      << std::endl;


    if (type == GPR) {
        gprRegNames_.push_back(regName);
    } else if (type == ARGUMENT) {
        argRegNames_.push_back(regName);
    } else if (type == RESULT) {
        argRegNames_.push_back(regName);
        resRegNames_.push_back(regName);
    } else {
        assert(type == RESERVED);
    }

    regs_[regName] = reg;

    regsInClasses_[regTemplate].push_back(regName);

    dregNum_++;
}


/**
 * Writes .td definitions of all registers in the machine to an output stream.
 *
 * @param o Output stream for the .td definitions.
 * @return True, if the definitions were succesfully generated.
 */
bool
TDGen::writeRegisterInfo(std::ostream& o) {
    analyzeRegisters();

    if (!checkRequiredRegisters()) {
        return false;
    }

    if (!mach_.is64bit()) {
        assert(regs32bit_.size() >= REQUIRED_FULL_WIDTH_REGS);
    } else {
        assert(regs64bit_.size() >= REQUIRED_FULL_WIDTH_REGS);
    }

    o << "//" << std::endl;
    o << "// This is an automatically generated file!" << std::endl;
    o << "// Do not edit manually." << std::endl;
    o << "//" << std::endl;
    o << std::endl;

    o << "class TCEReg<string n, list<Register> aliases> : "
      << "Register<n> {"
      << std::endl;

    o << "   let Namespace = \"TCE\";" << std::endl;
    o << "   let Aliases = aliases;" << std::endl;
    o << "}" << std::endl;

    writeRegisterClasses(o);
   
    writeRARegisterInfo(o);
    write1bitRegisterInfo(o);
    //write16bitRegisterInfo(o);
    write32bitRegisterInfo(o);
    write64bitRegisterInfo(o);

    return true;
}

void
TDGen::writeRegisterClasses(std::ostream& o) {

    o << "class R1<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << "}" << std::endl;

    o << "class R32<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << "}" << std::endl;

    o << "class R64<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << "}" << std::endl;


    o << "class R16<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << "}" << std::endl;
}


/**
 * Iterates through all registers in the machine and adds register information
 * to the register sets.
 */
void
TDGen::analyzeRegisters() {

    const TTAMachine::Machine::BusNavigator busNav =
        mach_.busNavigator();

    // Check which registers have guards and put their info in
    // guardedRegs_ set.
    for (int i = 0; i < busNav.count(); i++) {
        const TTAMachine::Bus& bus = *busNav.item(i);
        for (int g = 0; g < bus.guardCount(); g++) {
            const TTAMachine::RegisterGuard* rg =
                dynamic_cast<const TTAMachine::RegisterGuard*>(bus.guard(g));

            if (rg != NULL) {
                RegInfo reg = {
                    rg->registerFile()->name(),
                    (unsigned)rg->registerIndex() };

                guardedRegs_.insert(reg);
            }
        }
    }

    const TTAMachine::Machine::RegisterFileNavigator nav =
        mach_.registerFileNavigator();

    bool regsFound = true;
    int currentIdx = 0;
    while (regsFound) {
        regsFound = false;
        for (int i = 0; i < nav.count(); i++) {
            const TTAMachine::RegisterFile* rf = nav.item(i);

            // Check that the registerfile has both input and output ports.
            bool hasInput = false;
            bool hasOutput = false;
            for (int p = 0; p < rf->portCount(); p++) {
                if (rf->port(p)->isInput()) hasInput = true;
                if (rf->port(p)->isOutput()) hasOutput = true;
            }

            if (!hasInput) {
                if (Application::verboseLevel() >
                    Application::VERBOSE_LEVEL_DEFAULT) {

                    Application::logStream()
                        << "Warning: Skipping register file"
                        << rf->name()
                        << ": no input ports."
                        << std::endl;
                }
                continue;
            }

            if (!hasOutput) {
                if (Application::verboseLevel() >
                    Application::VERBOSE_LEVEL_DEFAULT) {
                    
                    Application::logStream()
                        << "Warning Skipping register file"
                        << rf->name()
                        << ": no output ports."
                        << std::endl;
                }
                continue;
            }

            unsigned width = rf->width();
            std::vector<RegInfo>* ri = NULL;
            if (width == 64) ri = &regs64bit_;
            else if (width == 32) ri = &regs32bit_;
            //else if (width == 16) ri = &regs16bit_;
            else if (width == 8) ri = &regs8bit_;
            else if (width == 1) ri = &regs1bit_;
            else {
                if (Application::verboseLevel() >
                    Application::VERBOSE_LEVEL_DEFAULT) {
                    
                    Application::logStream()
                        << "Warning: Skipping " << Conversion::toString(width)
                        << " bit rf '" << rf->name() << "'." << std::endl;
                }
                continue;
            }

            int lastIdx = rf->size();
            // todo: find a good solution to use just one big rf for this.
            
            bool isTempRegRf = false;
            for (unsigned int j = 0; j < tempRegFiles_.size(); j++) {
                if (tempRegFiles_[j] == rf) {
                    isTempRegRf = true;
                    break;
                }
            }

            if (isTempRegRf) {
                // If the machine is not enough connected,
                // preserve last register
                // of all register files for routing values.
                lastIdx--;
            }

            if (currentIdx < lastIdx) {
                RegInfo reg = {rf->name(), (unsigned)currentIdx};
                if (width != 1) {
                    // if has guard, set as 1-bit reg
                    if (guardedRegs_.find(reg) == guardedRegs_.end()) {
                        ri->push_back(reg);
                    } else {
                        // n-bit guarded registers not used by the compiler.
                    }
                } else {
                    // if it is 1-bit, it has to have guard.
                    if (!(guardedRegs_.find(reg) == guardedRegs_.end())) {
                        ri->push_back(reg);
                    } else {
                        // 1-bit non-guarded regs not used by the compiler.
                    }
                }
                regsFound = true;
            }
        }
        currentIdx++;
    }
}


/**
 * Writes 1-bit register definitions to the output stream.
 */
void
TDGen::write1bitRegisterInfo(std::ostream& o) {

    if (regs1bit_.size() < 1) {
        RegInfo reg = {"dummy1", 0};
        std::string name = "I1DUMMY";
        writeRegisterDef(o, reg, name, "R1", "", RESERVED);
    } else {
        for (unsigned i = 0; i < regs1bit_.size(); i++) {
            std::string regName = "B" + Conversion::toString(i);
            writeRegisterDef(o, regs1bit_[i], regName, "R1", "", GPR);
        }
    }

    int stackSize = mach_.is64bit() ? 64 : 32;
    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 1-bit RF classes
        if (ri->first.find("R1") == 0) {

            o << std::endl
              << "def " << ri->first << "Regs : RegisterClass<\"TCE\", [i1]"
              << ", " << stackSize << ", (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")> {" << std::endl
              << " let Size=" << stackSize << ";" << std::endl
              << "}" << std::endl;
        }
    }
}

/**
 * Writes 32-bit register definitions to the output stream.
 */
void
TDGen::write32bitRegisterInfo(std::ostream& o) {

    std::string i32regs;
    int firstFree32bitReg = 0;

    if (!mach_.is64bit()) {
        // --- Hardcoded reserved registers. ---
        writeRegisterDef(o, regs32bit_[0], "SP", "R32", "", RESERVED);
        writeRegisterDef(o, regs32bit_[1], "IRES0", "R32", "", RESULT);
        writeRegisterDef(o, regs32bit_[2], "FP", "R32", "", RESERVED);
        writeRegisterDef(
            o, regs32bit_[3], "KLUDGE_REGISTER", "R32", "", RESERVED);

        i32regs = "SP, IRES0, FP, KLUDGE_REGISTER";
        firstFree32bitReg = 4;

    }

    if (regs32bit_.size() < 1) {
        RegInfo reg = { "dummy32", 0 };
        writeRegisterDef(o, reg, "dummy32", "R64", "", RESERVED);
        o << "def R32Regs : RegisterClass<\"TCE\", [i32,f32,f16,i1], 32, (add dummy32)>;"
          << std::endl;
        o << "def R32IRegs : RegisterClass<\"TCE\", [i32], 32, (add dummy32)>;"
          << std::endl;
        o << "def R32FPRegs : RegisterClass<\"TCE\", [f32], 32, (add dummy32)>;"
          << std::endl;
        o << "def R32HFPRegs : RegisterClass<\"TCE\", [f16], 32, (add dummy32)>;"
          << std::endl;

    }

    // -------------------------------------
    
    for (unsigned i = firstFree32bitReg; i < regs32bit_.size(); i++) {
        std::string regName = "I" + Conversion::toString(i);
        writeRegisterDef(o, regs32bit_[i], regName, "R32", "", GPR);
        
        if (!regsInRFClasses_.count(regs32bit_[i].rf)) {
            regsInRFClasses_[regs32bit_[i].rf] = std::vector<std::string>();
        }
        i32regs += ", ";
        i32regs += regName;
        regsInRFClasses_[regs32bit_[i].rf].push_back(regName);
    }

    o << std::endl;

    for (RegClassMap::iterator 
        it = regsInRFClasses_.begin(); it != regsInRFClasses_.end(); ++it) {
        o << "def R32_" 
          << it->first 
          << "_Regs : RegisterClass<\"TCE\", [i32,f32,f16], 32, (add ";
        
        for (std::vector<std::string>::iterator r = it->second.begin();
             r != it->second.end(); ++r) {
            if (r != it->second.begin()) {
                o << ", ";
            }
            
            o << *r;
        }
        
        o << ")>;" << std::endl;
    }
    
    o << std::endl;
    
    // Register classes for all 32-bit registers.
    // TODO: why are these needed? same as integer classes below?
    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 1-bit RF classes
        if (ri->first.find("R32") == 0) {
            
            o << "def " << ri->first << "Regs : RegisterClass<\"TCE\", [i32,f32,f16,i1], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")>;" << std::endl;
        }
    }
    o << std::endl;
    
    // Integer register classes for 32-bit registers
    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 32-bit RF classes
        if (ri->first.find("R32") == 0) {
            o << "def " << ri->first << "IRegs : RegisterClass<\"TCE\", [i32], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")>;" << std::endl;
        }
    }
    o << std::endl;

    // Floating point register classes for 32-bit registers
    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 32-bit RF classes
        if (ri->first.find("R32") == 0) {
            
            o << "def " << ri->first << "FPRegs : RegisterClass<\"TCE\", [f32], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")>;" << std::endl;
        }
    }

    // Half-float register classes for 32-bit registers (TODO: 16-bit registers also?)
    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 32-bit RF classes
        if (ri->first.find("R32") == 0) {
            
            o << "def " << ri->first << "HFPRegs : RegisterClass<\"TCE\", [f16], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")>;" << std::endl;
        }
    }

//        if (regs32bit_.size() <3) {
    if (mach_.is64bit()) { // for now. later try to use 32-b it regs for float.
        use64bitForFP_ = true;
    }

    if (!use64bitForFP_) {

        o  << "def FPRegs : RegisterClass<\"TCE\", [f32], 32, (add "
           << i32regs << ")>;" << std::endl;

        o  << "def HFPRegs : RegisterClass<\"TCE\", [f16], 32, (add "
           << i32regs << ")>;" << std::endl;
    }
    o << std::endl;
}


/**
 * Writes 16-bit register definitions to the output stream.
 */
void
TDGen::write16bitRegisterInfo(std::ostream& o) {

    // --- Hardcoded reserved registers. ---
    writeRegisterDef(o, regs16bit_[0], "HIRES0", "R16", "", RESULT);

    // -------------------------------------
    
    for (unsigned i = 1; i < regs16bit_.size(); i++) {
        std::string regName = "H" + Conversion::toString(i);
        writeRegisterDef(o, regs16bit_[i], regName, "R16", "", GPR);
    }

    o << std::endl;

    // All 16-bit regs.
    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 1-bit RF classes
        if (ri->first.find("R16") == 0) {
            
            o << "def " << ri->first << "Regs : RegisterClass<\"TCE\", [i16], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")>;" << std::endl;
        }
    }
    o << std::endl;
    
    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 1-bit RF classes
        if (ri->first.find("R16") == 0) {
            o << "def " << ri->first << "IRegs : RegisterClass<\"TCE\", [i16], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")>;" << std::endl;
        }
    }
    o << std::endl;

    // floating-point-versions of these

    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 1-bit RF classes
        if (ri->first.find("R16") == 0) {
            
            o << "def " << ri->first << "FPRegs : RegisterClass<\"TCE\", [f16], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")>;" << std::endl;
        }
    }

    o << std::endl;
}

/**
 * Writes 64-bit register definitions to the output stream.
 */
void
TDGen::write64bitRegisterInfo(std::ostream& o) {

    // --- Hardcoded reserved registers. ---
    std::string i64regs;
    std::string f64regs;
    int firstFreeReg = 1;

    if (mach_.is64bit()) {
        // --- Hardcoded reserved registers. ---
        writeRegisterDef(o, regs64bit_[0], "SP", "R64", "", RESERVED);
        writeRegisterDef(o, regs64bit_[1], "IRES0", "R64", "", RESULT);
        writeRegisterDef(o, regs64bit_[2], "FP", "R64", "", RESERVED);
        writeRegisterDef(
            o, regs64bit_[3], "KLUDGE_REGISTER", "R64", "", RESERVED);
        firstFreeReg += 4;
        i64regs = "SP, IRES0, FP, KLUDGE_REGISTER";
    } else {

        if (regs64bit_.size() < 1) {
            RegInfo reg = { "dummy64", 0 };
            writeRegisterDef(o, reg, "LRES0", "R64", "", RESERVED);
        } else {
            writeRegisterDef(o, regs64bit_[0], "LRES0", "R64", "",
                             RESERVED);
        }
        i64regs = "LRES0";
    }

    for (unsigned i = firstFreeReg; i < regs64bit_.size(); i++) {
        std::string intRegName = "L" + Conversion::toString(i);
        i64regs += ", ";
        i64regs += intRegName;
        writeRegisterDef(o, regs64bit_[i], intRegName, "R64", "", GPR);
    }

    o << std::endl
      << "def R64Regs : RegisterClass<\"TCE\", [i64,f64], 64, (add "
      << i64regs << ")> ;"
      << std::endl;

    o << std::endl
      << "def R64IRegs : RegisterClass<\"TCE\", [i64], 64, (add "
      << i64regs << ")> ;"
      << std::endl;

    o << std::endl
      << "def R64FPRegs : RegisterClass<\"TCE\", [f32], 64, (add "
      << i64regs << ")>;" << std::endl;

    o << std::endl
      << "def R64HFPRegs : RegisterClass<\"TCE\", [f16], 64, (add "
      << i64regs << ")>;" << std::endl;

    o << std::endl
      << "def R64DFPRegs : RegisterClass<\"TCE\", [f64], 64, (add "
      << i64regs << ")>;" << std::endl << std::endl;

    if (use64bitForFP_) {
        o  << "def FPRegs : RegisterClass<\"TCE\", [f32], 64, (add "
           << i64regs << ")>;" << std::endl << std::endl;

        o  << "def HFPRegs : RegisterClass<\"TCE\", [f16], 64, (add "
           << i64regs << ")>;" << std::endl << std::endl;
    }
}

/**
 * Writes return address register definition to the output stream.
 */
void
TDGen::writeRARegisterInfo(std::ostream& o) {
    o << "class Rra<string n> : TCEReg<n, []>;" << std::endl;
    o << "def RA : Rra<\"return-address\">, ";
    o << "DwarfRegNum<[" << dregNum_++ << "]>;";
    o << std::endl;
    if (mach_.is64bit()) {
        o << "def RAReg : RegisterClass<\"TCE\", [i64], 64, (add RA)>;" <<
            std::endl;
    } else {
        o << "def RAReg : RegisterClass<\"TCE\", [i32], 32, (add RA)>;" <<
            std::endl;
    }

}


/**
 * Checks that the target machine has required registers to build a usable
 * plugin.
 *
 * @return True if required registers were found, false if not.
 */
bool
TDGen::checkRequiredRegisters() {
    if (!mach_.is64bit() && regs32bit_.size() < REQUIRED_FULL_WIDTH_REGS) {
        std::string msg =
            (boost::format(
                "Architecture doesn't meet the minimal requirements.\n"
                "Only %d 32-bit general purpose registers found. At least %d\n"
                "needed. ")
             % regs32bit_.size() % REQUIRED_FULL_WIDTH_REGS)
            .str();

        if (tempRegFiles_.size() > 0) {
            msg += "Your machine is not fully connected, thus one register\n"
                "from each register file are reserved for temp moves and\n"
                "not used as general purpose registers.";
        }

        throw InvalidData(__FILE__, __LINE__, __func__, msg);

        return false;
    }

    if (mach_.is64bit() && regs64bit_.size() < REQUIRED_FULL_WIDTH_REGS) {
        std::string msg =
            (boost::format(
                "Architecture doesn't meet the minimal requirements.\n"
                "Only %d 64-bit general purpose registers found. At least %d\n"
                "needed. ")
             % regs64bit_.size() % REQUIRED_FULL_WIDTH_REGS)
            .str();

        if (tempRegFiles_.size() > 0) {
            msg += "Your machine is not fully connected, thus one register\n"
                "from each register file are reserved for temp moves and\n"
                "not used as general purpose registers.";
        }

        throw InvalidData(__FILE__, __LINE__, __func__, msg);

        return false;
    }

    return true;
}

/**
 * Writes instruction .td definitions to the outputstream.
 */
void
TDGen::writeInstrInfo(std::ostream& os) {

    // llvm-version-dependent
    writeCallSeqStart(os);

    OperationDAGSelector::OperationSet opNames;
    OperationDAGSelector::OperationSet requiredOps =
        LLVMBackend::llvmRequiredOpset(true, littleEndian_, mach_.is64bit());

    const TTAMachine::Machine::FunctionUnitNavigator fuNav =
        mach_.functionUnitNavigator();

    OperationPool opPool;

    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit* fu = fuNav.item(i);
        for (int o = 0; o < fu->operationCount(); o++) {
            const std::string opName = fu->operation(o)->name();
            opNames.insert(StringTools::stringToUpper(opName));
        }
    }

    if (littleEndian_) {
        if (mach_.is64bit()) {
            opNames_["ST64fa"] = "ST64";
            opNames_["ST64fs"] = "ST64";
            opNames_["ST64ha"] = "ST64";
            opNames_["ST64hs"] = "ST64";

            opNames_["LD64fa"] = "LD64";
            opNames_["LD64fs"] = "LD64";
            opNames_["LD64ha"] = "LD64";
            opNames_["LD64hs"] = "LD64";
        }

        if (mach_.hasOperation("LD32")) {
            opNames_["LD32fr"] = "LD32";
            opNames_["LD32fi"] = "LD32";

            opNames_["LD32hr"] = "LD32";
            opNames_["LD32hi"] = "LD32";

        } else if (mach_.hasOperation("LDU32")) {
            opNames_["LD32fr"] = "LDU32";
            opNames_["LD32fi"] = "LDU32";

            opNames_["LD32hr"] = "LDU32";
            opNames_["LD32hi"] = "LDU32";
        }
        opNames_["ST32fr"] = "ST32";
        opNames_["ST32fi"] = "ST32";
        
        opNames_["ST16hr"] = "ST16";
        opNames_["ST16hi"] = "ST16";
    } else {
        opNames_["LDWfr"] = "LDW";
        opNames_["LDWfi"] = "LDW";
        opNames_["LDWhr"] = "LDW";
        opNames_["LDWhi"] = "LDW";

        opNames_["STWfr"] = "STW";
        opNames_["STWfi"] = "STW";
        
        opNames_["STWhr"] = "STW";
        opNames_["STWhi"] = "STW";
        opNames_["STHhr"] = "STH";
        opNames_["STHhi"] = "STH";
    }

    // some global/address immediate if conversion fails
    // so commented out
    if (hasConditionalMoves_) {

        if (mach_.is64bit()) {
            truePredOps_["MOV64ss"] = "PRED_TRUE_MOV64ss";
            falsePredOps_["MOV64ss"] = "PRED_FALSE_MOV64ss";
        }

        truePredOps_["MOVI32rr"] = "PRED_TRUE_MOVI32rr";
        falsePredOps_["MOVI32rr"] = "PRED_FALSE_MOVI32rr";
        truePredOps_["MOVI1rr"] = "PRED_TRUE_MOVI1rr";
        falsePredOps_["MOVI1rr"] = "PRED_FALSE_MOVI1rr";
    }

    OperationDAGSelector::OperationSet::const_iterator iter = opNames.begin();
    for (; iter != opNames.end(); iter++) {
        OperationDAGSelector::OperationSet::iterator r = 
            requiredOps.find(*iter);
        if (r != requiredOps.end()) {
            requiredOps.erase(r);
        }
        Operation& op = opPool.operation((*iter).c_str());
        bool skipPattern = false;

        if (&op == &NullOperation::instance()) {
            continue;
        }
        
        // TODO: Allow multioutput (remove last or)
        if (!operationCanBeMatched(op)) {
            
            // TODO: write opeation def without graphs
            if (&op != &NullOperation::instance()) {
                skipPattern = true;
                if (Application::verboseLevel() > 0) {
                    Application::logStream() 
                        << "Skipped writing operation pattern for " 
                        << op.name() << std::endl;
                }
            } else {
                // NULL op - ignore
                continue;
            }
        }
        
        // TODO: remove this. For now MIMO operation patterns are not 
        // supported by tablegen.
        if (op.numberOfOutputs() > 1) {
            skipPattern = true;
            continue;
        }

        writeOperationDefs(os, op, skipPattern);
    }
    

    writeCallDef(os);

    os << std::endl;

    // Emulated operations,
    iter = requiredOps.begin();
    for (; iter != requiredOps.end(); iter++) {

        const Operation& op = opPool.operation((*iter).c_str());       

        if (&op == &NullOperation::instance()) {
            std::string msg = "Required OP '" + *iter + "' not found.";
            throw InvalidData(__FILE__, __LINE__, __func__, msg);
        }

        const OperationDAGSelector::OperationDAGList emulationDAGs =
            OperationDAGSelector::findDags(op.name(), opNames);

        if (emulationDAGs.empty()) {
            if (Application::verboseLevel() >
                Application::VERBOSE_LEVEL_DEFAULT) {

                Application::logStream()
                    << "Warning: Operation '" << *iter 
                    <<"' not supported." << std::endl;
            }
        } else {
            // TODO: write all dags of operation (first as normal, the rest 
            // as pattern)
            writeEmulationPattern(os, op, emulationDAGs.smallestNodeCount());
        }
    }
    createByteExtLoadPatterns(os);
    createShortExtLoadPatterns(os);
    if (mach_.is64bit()) {
        create32BitExtLoadPatterns(os);
    }
    createSelectPatterns(os);
    writeMiscPatterns(os);
    createConstShiftPatterns(os);
    createBoolAndHalfLoadPatterns(os);
}

/**
 * Writes .td pattern for the call instruction(s) to the output stream.
 */
void
TDGen::writeCallDef(std::ostream& o) {
    o << "let ";
    if (!argRegNames_.empty()) {
        o << "Uses = [";
        for (unsigned i = 0; i < argRegNames_.size(); i++) {
            if (i > 0) o << ", ";
            o << argRegNames_[i];
        }    
        o << "],";
    }
    o << "hasDelaySlot = 1, isCall = 1,";
    o << "Defs = [";
    for (unsigned i = 0; i < resRegNames_.size(); i++) {
        if (i > 0) o << ", ";
        o << resRegNames_[i];
    }    
    for (unsigned i = 0; i < gprRegNames_.size(); i++) {
        o << ", " << gprRegNames_[i];
    }
    o << "] in {" << std::endl;
    o << "def CALL : InstTCE<(outs), (ins calltarget:$dst),";
    o << "\"$dst -> call.1;\", []>;" << std::endl;

    o << "def CALL_MEMrr : InstTCE<(outs), (ins MEMrr:$ptr),";
    o << "\"$ptr -> call.1;\", [(call ADDRrr:$ptr)]>;" << std::endl;
    
    o << "def CALL_MEMri : InstTCE<(outs), (ins MEMri:$ptr),";
    o << "\"$ptr -> call.1;\", [(call ADDRri:$ptr)]>;" << std::endl;
    o << "}" << std::endl;

    o << "def : Pat<(call tglobaladdr:$dst), (CALL tglobaladdr:$dst)>;"
      << std::endl;

    o << "def : Pat<(call texternalsym:$dst), (CALL texternalsym:$dst)>;"
      << std::endl;

}

/**
 * Generates required function definitions for the backend plugin.
 *
 * @param o Output stream to write the c++ code to.
 */
void
TDGen::writeBackendCode(std::ostream& o) {


    // Register & operation info table initialization

    o << "void" << std::endl
      << "GeneratedTCEPlugin::initialize() {" << std::endl;

    // operation names
    std::map<std::string, std::string>::const_iterator iter =
        opNames_.begin();

    for (; iter != opNames_.end(); iter++) {
        o << "    opNames_[TCE::" << (*iter).first
          << "] = \"" << (*iter).second
          << "\";" << std::endl;
    }

    for (iter = truePredOps_.begin(); iter != truePredOps_.end(); iter++) {
	o << "    truePredOps_[TCE::" << (*iter).first
	  << "] = TCE::" << (*iter).second
	  << ";" << std::endl;
    }

    for (iter = falsePredOps_.begin(); iter != falsePredOps_.end(); iter++) {
	o << "    falsePredOps_[TCE::" << (*iter).first
	  << "] = TCE::" << (*iter).second
	  << ";" << std::endl;
    }

    // Register names & indices
    std::map<std::string, RegInfo>::const_iterator rIter = regs_.begin();
    rIter = regs_.begin();
    for (; rIter != regs_.end(); rIter++) {
        o << "    regNames_[TCE::" << (*rIter).first
          << "] = \"" << (*rIter).second.rf
          << "\";" << std::endl;

        o << "    regIndices_[TCE::" << (*rIter).first
          << "] = " << (*rIter).second.idx
          << ";" << std::endl;
    }


    // Target machine .adf XML string.
    std::string adfXML;
    ADFSerializer serializer;
    serializer.setDestinationString(adfXML);
    serializer.writeMachine(mach_);
    o << "    adfXML_ = \"";
    for (unsigned int i = 0; i < adfXML.length(); i++) {
        if (adfXML[i] == '"') o << "\\\"";
        else if (adfXML[i] == '\n') o << "\"\n\"";
        else o << adfXML[i];
    }
    o << "\";" << std::endl;


    // data address space
    const TTAMachine::Machine::FunctionUnitNavigator& nav =
        mach_.functionUnitNavigator();
    std::string asName = "";
    for (int i = 0; i < nav.count(); i++) {
        if (nav.item(i) != mach_.controlUnit() &&
            nav.item(i)->addressSpace() != NULL) {
            asName = nav.item(i)->addressSpace()->name();
        }
    }

    if (asName == "") {
        std::string msg = "Couldn't determine data address space.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    o << "    dataASName_ = \"" << asName << "\";" << std::endl;

    o << "}" << std::endl;


    bool hasSDIV = false;
    bool hasUDIV = false;
    bool hasSREM = false;
    bool hasUREM = false;
    bool hasMUL = false;
    bool hasROTL = false;
    bool hasROTR = false;
    bool hasSXHW = false;
    bool hasSXQW = false;
    bool hasSQRTF = false;
    bool hasSHR = false;
    bool hasSHRU = false;
    bool hasSHL = false;
    bool has8bitLoads = false;
    bool has16bitLoads = false;
//    bool has32bitLoads = false; // used only for 64-bit system

    const TTAMachine::Machine::FunctionUnitNavigator fuNav =
        mach_.functionUnitNavigator();

    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit* fu = fuNav.item(i);
        for (int o = 0; o < fu->operationCount(); o++) {
            const std::string opName =
                StringTools::stringToLower(fu->operation(o)->name());

            if (mach_.is64bit()) {
                if (opName == "div64") hasSDIV = true;
                if (opName == "divu64") hasUDIV = true;
                if (opName == "mod64") hasSREM = true;
                if (opName == "modu64") hasUREM = true;
                if (opName == "mul64") hasMUL = true;
                if (opName == "rotl64") hasROTL = true;
                if (opName == "rotr64") hasROTR = true;
                if (opName == "sxh64") hasSXHW = true;
                if (opName == "sxq64") hasSXQW = true;
                if (opName == "shr64") hasSHR = true;
                if (opName == "shru64") hasSHRU = true;
                if (opName == "shl64") hasSHL = true;
            } else {
                if (opName == "div") hasSDIV = true;
                if (opName == "divu") hasUDIV = true;
                if (opName == "mod") hasSREM = true;
                if (opName == "modu") hasUREM = true;
                if (opName == "mul") hasMUL = true;
                if (opName == "rotl") hasROTL = true;
                if (opName == "rotr") hasROTR = true;
                if (opName == "sxhw") hasSXHW = true;
                if (opName == "sxqw") hasSXQW = true;
                if (opName == "shr") hasSHR = true;
                if (opName == "shru") hasSHRU = true;
                if (opName == "shl") hasSHL = true;
            }
            if (opName == "sqrtf") hasSQRTF = true;

            if (littleEndian_) {
                if (opName == "ld16" || opName == "ldu16") {
                    has16bitLoads = true;
                } else if(opName == "ld8" || opName == "ldu8") {
                    has8bitLoads = true;
//                } else if (opName == "ld32" || opName == "ldu32") {
//                    has32bitLoads = true;
                }
            } else {
                if (opName == "ldh" || opName == "ldhu") {
                    has16bitLoads = true;
                } else if (opName == "ldq" || opName == "ldqu") {
                    has8bitLoads = true;
//                } else if (opName == "ldw") {
//                    has32bitLoads = true;
                }
           }
        }
    }

    o << "bool GeneratedTCEPlugin::hasSDIV() const { return "
      << hasSDIV << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasUDIV() const { return "
      << hasUDIV << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasSREM() const { return "
      << hasSREM << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasUREM() const { return "
      << hasUREM << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasMUL() const { return "
      << hasMUL << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasROTL() const { return "
      << hasROTL << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasROTR() const { return "
      << hasROTR << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasSXHW() const { return "
      << hasSXHW << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasSXQW() const { return "
      << hasSXQW << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasSQRTF() const { return "
      << hasSQRTF << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasSHR() const { return "
      << hasSHR << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasSHL() const { return "
      << hasSHL << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasSHRU() const { return "
      << hasSHRU << ";}" << std::endl
      << "bool GeneratedTCEPlugin::has8bitLoads() const { return "
      << has8bitLoads << ";}" << std::endl
      << "bool GeneratedTCEPlugin::has16bitLoads() const { return "
      << has16bitLoads << ";}" << std::endl;
//      << "bool GeneratedTCEPlugin::has32bitLoads() const { return "
//      << has32bitLoads << ";}" << std::endl

    generateLoadStoreCopyGenerator(o);
    createMinMaxGenerator(o);
    createGetMaxMemoryAlignment(o);
    createEndiannesQuery(o);
}

/**
 * Writes a top-level .td file which includes generated .td definitions
 * to single file.
 *
 * @param o Output stream to write the .td to.
 */
void
TDGen::writeTopLevelTD(std::ostream& o) {
   o << "include \"Target.td\"" << std::endl;
   o << "include \"GenRegisterInfo.td\"" << std::endl;
   o << "include \"TCEInstrInfo.td\"" << std::endl;
   o << "include \"GenCallingConv.td\"" << std::endl;
   o << "def TCEInstrInfo : InstrInfo { }" << std::endl;
   o << "def TCE : Target { let InstructionSet = TCEInstrInfo; }" 
     << std::endl;
}


/*
 * Operand type characters:
 * a
 * b = Boolean/predicate register
 * c
 * d
 * e
 * f = Float32 register
 * g
 * h = Float16 register
 * i = Immediate integer
 * j = immediate boolean
 * k = immediate float?
 * l = immediate float16?
 * p
 * q
 * r = integer Register
 * s
 * t
 * u
 * y
 * z
 */


/**
 * Writes operation definition in .td format to an output stream.
 *
 * @param o Output stream to write the definition to.
 * @param op Operation to write definition for.
 */
void
TDGen::writeOperationDefs(
    std::ostream& o,
    Operation& op,
    bool skipPattern) {

    std::string attrs;

    // These white listed operations have mayLoad/mayStore flag
    // inferred from the llvm pattern and declaring it
    // explicitly will display a warning in tablegen.
    if (op.name() != "LDQ" && op.name() != "LDQU" &&
        op.name() != "LDH" && op.name() != "LDHU" &&
        op.name() != "LDW" && op.name() != "LDD" &&
        op.name() != "STQ" && op.name() != "STH" &&
        op.name() != "STW" && op.name() != "STD" &&
        op.name() != "ALDQ" && op.name() != "ALDQU" &&
        op.name() != "ALDH" && op.name() != "ALDHU" &&
        op.name() != "ALDW" && op.name() != "ALDD" &&
        op.name() != "ASTQ" && op.name() != "ASTH" &&
        op.name() != "ASTW" && op.name() != "ASTD" &&

        op.name() != "LD8" && op.name() != "LDU8" &&
        op.name() != "LD16" && op.name() != "LDU16" &&
        op.name() != "LD32" && op.name() != "LDU32" &&
        op.name() != "LD64" &&
        op.name() != "ST8" && op.name() != "ST16" &&
        op.name() != "ST32" && op.name() != "ST64" &&
        op.name() != "ALD8" && op.name() != "ALDU8" &&
        op.name() != "ALD16" && op.name() != "ALDU16" &&
        op.name() != "ALD32" && op.name() != "ALDU32" &&
        op.name() != "ALD64" &&
        op.name() != "AST8" && op.name() != "AST16" &&
        op.name() != "AST32" && op.name() != "AST64") {

        if (op.readsMemory()) attrs += " mayLoad = 1";
        if (op.readsMemory() && op.writesMemory()) attrs += ", ";
        if (op.writesMemory()) attrs += " mayStore = 1";
    }

    // no bool outs for some operatios
    if (op.name() == "CFI" || op.name() == "CFIU") {
        writeOperationDef(o, op, "rf", attrs, skipPattern);
        return;
    }
        
    // rotations are allways n x n -> n bits.
    if (op.name() == "ROTL" || op.name() == "ROTR" ||
        op.name() == "SHL" || op.name() == "SHR" || op.name() == "SHRU") {
        writeOperationDefs(o, op, "rrr", attrs, skipPattern);
        return;
    }

    if (op.name() == "SXHW" || op.name() == "SXQW") {
        writeOperationDef(o, op, "rr", attrs, skipPattern);
        return;
    }

    // these can have 1-bit inputs
    if (op.name() == "XOR" || op.name() == "IOR" || op.name() == "AND" ||
        op.name() == "ANDN" || op.name() == "ADD" || op.name() == "SUB" ||
        op.name() == "XOR64" || op.name() == "IOR64" || op.name() == "AND64") {
        
        writeOperationDefs(o, op, "bbb", attrs, skipPattern);
    }

    if (op.name() == "SELECT") {
        writeOperationDef(o, op, "rrrb", attrs, skipPattern);
        writeOperationDef(o, op, "riib", attrs, skipPattern);
        writeOperationDef(o, op, "rrib", attrs, skipPattern);
        writeOperationDef(o, op, "rirb", attrs, skipPattern);
        writeOperationDef(o, op, "bbbb", attrs, skipPattern);
        writeOperationDef(o, op, "bjjb", attrs, skipPattern);
        writeOperationDef(o, op, "bjbb", attrs, skipPattern);
        writeOperationDef(o, op, "bbjb", attrs, skipPattern);
        // TODO: what about floating-point values?
        writeOperationDef(o, op, "fffb", attrs, skipPattern);
        writeOperationDef(o, op, "hhhb", attrs, skipPattern);

        hasSelect_ = true;
        return;
    }

    // store likes this. store immediate to immediate address
    if (op.numberOfInputs() == 2 && op.numberOfOutputs() == 0) {
        Operand& operand1 = op.operand(1);
        Operand& operand2 = op.operand(2);
        // TODO: add an else branch here for float immediates
        if ((operand1.type() == Operand::UINT_WORD || 
             operand1.type() == Operand::SINT_WORD ||
             operand1.type() == Operand::RAW_DATA) &&
           (operand2.type() == Operand::UINT_WORD || 
            operand2.type() == Operand::SINT_WORD ||
            operand2.type() == Operand::RAW_DATA)) {

            if (mach_.is64bit()) {
                writeOperationDef(o, op, "aa", attrs, skipPattern);
            } else {
                writeOperationDef(o, op, "ii", attrs, skipPattern);
            }
        }
    }

    std::string operandTypes = createDefaultOperandTypeString(op);
    // this the ordinary def

    // then try with immediates.
    // TODO: this should be 2^n loop instead of n loop, to get
    // all permutations.

    writeOperationDefs(o, op, operandTypes, attrs, skipPattern);

    // then with boolean outs, and vector versions.
    if (op.numberOfOutputs() == 1 && !op.readsMemory()) {
        Operand& outOperand = op.operand(op.numberOfInputs()+1);
        if (outOperand.type() == Operand::UINT_WORD || 
            outOperand.type() == Operand::SINT_WORD ||
            outOperand.type() == Operand::ULONG_WORD ||
            outOperand.type() == Operand::SLONG_WORD) {

            // 32/64 to 1-bit operations
            operandTypes[0] = 'b';
            writeOperationDefs(o, op, operandTypes, attrs, skipPattern);
        } 

        // create vector versions.
    }
}

char regOperandCharToImmOperandChar(char c) {
    switch(c) {
    case 's':
        return 'a';
    case 'r':
        return 'i';
    case 'b':
        return 'j';
    case 'f':
        return 'k';
    case 'h':
        return 'l';
    dafault:
        return 0;
    }
}

/**
 * Writes operation defs for single operation, with different immediate params.
 *
 * @param o Output stream to write the definition to.
 * @param op Operation to write definition for.
 * @param operandTypes value types of operands.
 */
void
TDGen::writeOperationDefs(
    std::ostream& o, Operation& op, const std::string& operandTypes,
    const std::string& attrs, bool skipPattern, std::string backendPrefix) {

    // first without imms.
    writeOperationDef(o, op, operandTypes, attrs, skipPattern, backendPrefix);

    for (int i = 0; i < op.numberOfInputs(); i++) {
        bool canSwap = false;
        for (int j = i+1 ; j < op.numberOfInputs(); j++) {
            if (op.canSwap(i+1, j+1)) {
                canSwap = true;
                break;
            }
        }
        // setcc of LLVM is not commutative and get confused if we don't generate
        // a pattern with immediate elsewhere than the last operand        
        canSwap = canSwap && !(op.name().upper() == "EQ" || op.name().upper() == "NE");

        if (!canSwap) {
            std::string opTypes = operandTypes;
            char& c = opTypes[i + op.numberOfOutputs()];
            c = regOperandCharToImmOperandChar(c);
            if (c) {
                writeOperationDef(o, op, opTypes, attrs, skipPattern, backendPrefix);
            }
        }
    }
}


/**
 * Writes a single operation def for single operation.
 *
 * @param o Output stream to write the definition to.
 * @param op Operation to write definition for.
 * @param operandTypes value types of operands.
 */
void 
TDGen::writeOperationDef(
    std::ostream& o,
    Operation& op, const std::string& operandTypes, const std::string& attrs,
    bool skipPattern, std::string backendPrefix) {
    assert (operandTypes.size() > 0);

    char operandChar = mach_.is64bit() ? 's': 'r';

    std::string outputs, inputs, asmstr, pattern;
    outputs = "(outs" + patOutputs(op, operandTypes) + ")";
    inputs = "(ins " + patInputs(op, operandTypes) + ")";
    std::string predicatedInputs = 
	"(ins R1Regs:$pred, " +patInputs(op, operandTypes)+ ")";

    asmstr = "\"\"";
    
    if (!skipPattern) {
        if (llvmOperationPattern(op, operandChar) != "" ||
            op.dagCount() == 0) {
            OperationDAG* trivial = createTrivialDAG(op);
            pattern = operationPattern(op, *trivial, operandTypes);
            delete trivial;
        } else {
            pattern = operationPattern(op, op.dag(0), operandTypes);
        }
    }
    
    if (attrs != "") {
        o << "let" << attrs << " in { " << std::endl;
    }
    
    std::string opcEnum = 
        StringTools::stringToUpper(op.name()) + operandTypes;
    
    o << "def " << opcEnum << " : " 
      << "InstTCE<"
      << outputs << ", "
      << inputs << ", "
      << asmstr << ", "
      << "[" << pattern << "]>;"
      << std::endl;

    // Predicating operands with immediates can currently lead to
    // unschedulable code in case there's no bus that has both the
    // predicate and the immediate transfer capability. Disable
    // generating the predicated versions for immediate operand
    // patterns for now.
    bool canBePredicated = operandTypes.find('i') == std::string::npos;

    if (canBePredicated) {
        // write predicated versions
        o << "def PRED_TRUE_" << opcEnum << " : "
          << "InstTCE<"
          << outputs << ", "
          << predicatedInputs << ", "
          << asmstr << ", "
          << "[]>;"
          << std::endl;

        // write predicated versions
        o << "def PRED_FALSE_" << opcEnum << " : "
          << "InstTCE<"
          << outputs << ", "
          << predicatedInputs << ", "
          << asmstr << ", "
          << "[]>;"
          << std::endl;
    }

    if (attrs != "") {
        o << "}" << std::endl;
    }        
    opNames_[opcEnum] = backendPrefix + op.name();

    if (canBePredicated) {
        opNames_["PRED_TRUE_" + opcEnum] = "?" + backendPrefix + op.name();
        opNames_["PRED_FALSE_" + opcEnum] = "!" + backendPrefix + op.name();
        truePredOps_[opcEnum] = "PRED_TRUE_" + opcEnum;
        falsePredOps_[opcEnum] = "PRED_FALSE_" + opcEnum;
    }
}

/**
 * Checks whether operand is integer or float type.
 *
 * @return 'r' for integer, 'f' for float
 */
char 
TDGen::operandChar(Operand& operand) {
    if (operand.type() == Operand::BOOL) {
        return 'b';
    } else if (operand.type() == Operand::HALF_FLOAT_WORD) {
        return 'h';
    } else if (operand.type() == Operand::ULONG_WORD ||
               operand.type() == Operand::SLONG_WORD ||
               operand.type() == Operand::RAW_DATA) {
        return mach_.is64bit() ? 's' : 'r';
    } else if (operand.type() != Operand::UINT_WORD &&
               operand.type() != Operand::SINT_WORD) {
        return 'f';
    } else {
        return 'r';
    }
}

/**
 * Writes operation emulation pattern in .td format to an output stream.
 *
 * @param o Output stream to write the definition to.
 * @param op Emulated operation.
 * @param dag Emulation pattern.
 */
void
TDGen::writeEmulationPattern(
    std::ostream& o,
    const Operation& op,
    const OperationDAG& dag) {

    char operandCh = mach_.is64bit() ? 's': 'r';

    const OperationDAGNode* res = *(dag.endNodes().begin());
    if (dag.endNodes().empty()) {
        std::cerr << "end nodes of dag for operation: " << op.name()
                  << " is empty!" << std::endl;
        assert(false);
    }

    const OperationNode* opNode = dynamic_cast<const OperationNode*>(res);
    if (opNode == NULL) {
        assert(dag.inDegree(*res) ==  1);
        const OperationDAGEdge& edge = dag.inEdge(*res, 0);
        res = dynamic_cast<OperationNode*>(&dag.tailNode(edge));
        assert(res != NULL);
    }

    int inputCount = op.numberOfInputs();
    for (int immInput = 0; immInput <= inputCount; immInput++) {
        // generate commutative operation immediate patterns only once
        bool canSwap = false;
        if (immInput > 0) {
            for (int j = immInput + 1; j <= op.numberOfInputs(); j++) {
                if (op.canSwap(immInput, j)) {
                    canSwap = true;
                    break;
                }
            }
        }
        // setcc of LLVM is not commutative and get confused if we don't generate
        // a pattern with an immediate elsewhere than the last operand
        canSwap = canSwap && !(op.name().upper() == "EQ" || op.name().upper() == "NE");
        if (canSwap) {
            continue;
        }

        bool ok = true;
        std::string llvmPat = llvmOperationPattern(op, operandCh);
        if (llvmPat == "") {
            std::cerr << "unknown op: " << op.name() << std::endl;
        }
        assert(llvmPat != "" && "Unknown operation to emulate.");
        
        boost::format match1(llvmPat);

        int outputs = op.numberOfOutputs();
        
        std::string operandTypes;
        for (int i = 0; i < outputs; i++) {
            operandTypes += operandChar(op.operand(i+inputCount + 1));
        }

        for (int i = 0; i < op.numberOfInputs(); i++) {
            char inputType = operandChar(op.operand(i+1));
            if (immInput == i+1) {
                // float imm operands not allowed
                if (inputType == 'f' || inputType == 'h') {
                    ok = false;
                    break;
                } else {
                    inputType = regOperandCharToImmOperandChar(inputType);
                    if (!inputType) {
                        ok = false;
                        break;
                    }
                }
            }
            operandTypes += inputType;
            match1 % operandToString(op.operand(i + 1), false, inputType);
        }
        if (ok) {
            o << "def : Pat<(" << match1.str() << "), "
              << dagNodeToString(op, dag, *res, true, operandTypes)
              << ">;" << std::endl;
            
            // need to generate emulation patterns for boolean out
            // for these comparison operations.
            if (op.name() == "LTF" || op.name() == "LTUF" ||
                op.name() == "EQF" || op.name() == "EQUF" ||
                op.name() == "GEF" || op.name() == "GEUF" || 
                op.name() == "LEF" || op.name() == "LEUF" ||
                op.name() == "GTF" || op.name() == "GTUF" ||
                op.name() == "NEF" || op.name() == "NEUF" ||
                op.name() == "EQ" || op.name() == "NE" ||
                op.name() == "EQ64" || op.name() == "NE64" ||
                op.name() == "GE" ||op.name() == "GEU" ||
                op.name() == "GE64" ||op.name() == "GEU64" ||
                op.name() == "GT" || op.name() == "GTU" ||
                op.name() == "GT64" || op.name() == "GTU64" ||
                op.name() == "LE" || op.name() == "LEU" ||
                op.name() == "LE64" || op.name() == "LEU64" ||
                op.name() == "LT" || op.name() == "LTU" ||
                op.name() == "LT64" || op.name() == "LTU64" ||
                op.name() == "ORDF" || op.name() == "UORDF") {
                std::string boolOperandTypes = operandTypes;
                boolOperandTypes[0] = 'b';
                o << "def : Pat<(" << match1.str() << "), "
                  << dagNodeToString(op, dag, *res, true, boolOperandTypes)
                  << ">;" << std::endl;
            }
        }
    }
}


/**
 * Returns llvm operation node .td format string as a boost::format string.
 *
 * Boost format parameters correspond to the operand strings.
 *
 * @param op Operation for which the LLVM pattern should be returned.
 * @return Boost::format string of the operation node in llvm.
 */
TCEString
TDGen::llvmOperationPattern(const Operation& op, char operandType) {

    TCEString opName = StringTools::stringToLower(op.name());

    if (opName == "add") return "add %1%, %2%";
    if (opName == "add64") return "add %1%, %2%";
    if (opName == "sub") return "sub %1%, %2%";
    if (opName == "sub64") return "sub %1%, %2%";
    if (opName == "mul") return "mul %1%, %2%";
    if (opName == "mul64") return "mul %1%, %2%";
    if (opName == "div") return "sdiv %1%, %2%";
    if (opName == "divu") return "udiv %1%, %2%";
    if (opName == "div64") return "sdiv %1%, %2%";
    if (opName == "divu64") return "udiv %1%, %2%";
    if (opName == "mod") return "srem %1%, %2%";
    if (opName == "modu") return "urem %1%, %2%";
    if (opName == "mod64") return "srem %1%, %2%";
    if (opName == "modu64") return "urem %1%, %2%";

    if (opName == "shl") return "shl %1%, %2%";
    if (opName == "shr") return "sra %1%, %2%";
    if (opName == "shru") return "srl %1%, %2%";
    if (opName == "rotl") return "rotl %1%, %2%";
    if (opName == "rotr") return "rotr %1%, %2%";


    if (opName == "shl64") return "shl %1%, %2%";
    if (opName == "shr64") return "sra %1%, %2%";
    if (opName == "shru64") return "srl %1%, %2%";
    if (opName == "rotl64") return "rotl %1%, %2%";
    if (opName == "rotr64") return "rotr %1%, %2%";

    if (opName == "and") return "and %1%, %2%";
    if (opName == "ior") return "or %1%, %2%";
    if (opName == "xor") return "xor %1%, %2%";

    if (opName == "and64") return "and %1%, %2%";
    if (opName == "ior64") return "or %1%, %2%";
    if (opName == "xor64") return "xor %1%, %2%";

    if (opName == "eq") return "seteq %1%, %2%";
    if (opName == "eq64") return "seteq %1%, %2%";
    if (opName == "ne") return "setne %1%, %2%";
    if (opName == "ne64") return "setne %1%, %2%";
    if (opName == "lt") return "setlt %1%, %2%";
    if (opName == "lt64") return "setlt %1%, %2%";
    if (opName == "le") return "setle %1%, %2%";
    if (opName == "le64") return "setle %1%, %2%";
    if (opName == "gt") return "setgt %1%, %2%";
    if (opName == "gt64") return "setgt %1%, %2%";
    if (opName == "ge") return "setge %1%, %2%";
    if (opName == "ge64") return "setge %1%, %2%";
    if (opName == "ltu") return "setult %1%, %2%";
    if (opName == "ltu64") return "setult %1%, %2%";
    if (opName == "leu") return "setule %1%, %2%";
    if (opName == "leu64") return "setule %1%, %2%";
    if (opName == "gtu") return "setugt %1%, %2%";
    if (opName == "gtu64") return "setugt %1%, %2%";
    if (opName == "geu") return "setuge %1%, %2%";
    if (opName == "geu64") return "setuge %1%, %2%";

    if (opName == "eqf") return "setoeq %1%, %2%";
    if (opName == "nef") return "setone %1%, %2%";
    if (opName == "ltf") return "setolt %1%, %2%";
    if (opName == "lef") return "setole %1%, %2%";
    if (opName == "gtf") return "setogt %1%, %2%";
    if (opName == "gef") return "setoge %1%, %2%";

    if (opName == "equf") return "setueq %1%, %2%";
    if (opName == "neuf") return "setune %1%, %2%";
    if (opName == "ltuf") return "setult %1%, %2%";
    if (opName == "leuf") return "setule %1%, %2%";
    if (opName == "gtuf") return "setugt %1%, %2%";
    if (opName == "geuf") return "setuge %1%, %2%";

    if (opName == "ordf") return "seto %1%, %2%";
    if (opName == "uordf") return "setuo %1%, %2%";

    if (opName == "addf") return "fadd %1%, %2%";
    if (opName == "subf") return "fsub %1%, %2%";
    if (opName == "mulf") return "fmul %1%, %2%";
    if (opName == "divf") return "fdiv %1%, %2%";
    if (opName == "absf") return "fabs %1%";
    if (opName == "negf") return "fneg %1%";
    if (opName == "sqrtf") return "fsqrt %1%";

    if (opName == "cif") return "sint_to_fp %1%";
    if (opName == "cfi") return "fp_to_sint %1%";
    if (opName == "cifu") return "uint_to_fp %1%";
    if (opName == "cfiu") return "fp_to_uint %1%";

#if LLVM_OLDER_THAN_4_0
    if (opName == "cfh") return "fround %1%";
    if (opName == "chf") return "f32 (fextend %1%)";
#else
    if (opName == "cfh") return "fpround %1%";
    if (opName == "chf") return "f32 (fpextend %1%)";
#endif

    if (opName == "cih") return "sint_to_fp %1%";
    if (opName == "chi") return "i32 (fp_to_sint %1%)";
    if (opName == "cihu") return "uint_to_fp %1%";
    if (opName == "chiu") return "i32 (fp_to_uint %1%)";

    if (opName == "neuh") return "setune %1%, %2%";
    if (opName == "eqh") return "setoeq %1%, %2%";
    if (opName == "neh") return "setone %1%, %2%";
    if (opName == "lth") return "setolt %1%, %2%";
    if (opName == "leh") return "setole %1%, %2%";
    if (opName == "gth") return "setogt %1%, %2%";
    if (opName == "geh") return "setoge %1%, %2%";

    if (opName == "ordh") return "seto %1%, %2%";
    if (opName == "uordh") return "setuo %1%, %2%";

    if (opName == "addh") return "fadd %1%, %2%";
    if (opName == "subh") return "fsub %1%, %2%";
    if (opName == "mulh") return "fmul %1%, %2%";
    if (opName == "divh") return "fdiv %1%, %2%";
    if (opName == "absh") return "fabs %1%";
    if (opName == "negh") return "fneg %1%";
    if (opName == "sqrth") return "fsqrt %1%";

    if (opName == "cih") return "sint_to_fp %1%";
    if (opName == "chi") return "fp_to_sint %1%";
    if (opName == "cihu") return "uint_to_fp %1%";
    if (opName == "chiu") return "fp_to_uint %1%";

    if (opName == "csh") return "sint_to_fp %1%";
    if (opName == "cshu") return "uint_to_fp %1%";
    if (opName == "chs") return "fp_to_sint %1%";
    if (opName == "chsu") return "fp_to_uint %1%";

    if (littleEndian_) {
        if (opName == "ld8") return "sextloadi8 %1%";
        if (opName == "ldu8") return "zextloadi8 %1%";
        if (opName == "ld16") return "sextloadi16 %1%";
        if (opName == "ldu16") return "zextloadi16 %1%";
        if (mach_.is64bit()) {
            if (opName == "ld32") return "sextloadi32 %1%";
            if (opName == "ldu32") return "zextloadi32 %1%";
        } else {
            if (opName == "ld32" || opName == "ldu32") return "load %1%";
        }
        if (opName == "ld64") return "load %1%";
        //if (opName == "ldd") return "load";
        
        if (opName == "st8") return "truncstorei8 %2%, %1%";
        if (opName == "st16") return "truncstorei16 %2%, %1%";
        if (mach_.is64bit()) {
            if (opName == "st32") return "truncstorei32 %2%, %1%";
        } else {
            if (opName == "st32") return "store %2%, %1%";
        }
        if (opName == "st64") return "store %2%, %1%";
    } else {
        if (opName == "ldq") return "sextloadi8 %1%";
        if (opName == "ldqu") return "zextloadi8 %1%";
        if (opName == "ldh") return "sextloadi16 %1%";
        if (opName == "ldhu") return "zextloadi16 %1%";
        if (opName == "ldw") return "load %1%";
        //if (opName == "ldd") return "load";
        
        if (opName == "stq") return "truncstorei8 %2%, %1%";
        if (opName == "sth") return "truncstorei16 %2%, %1%";
        if (opName == "stw") return "store %2%, %1%";
        //if (opName == "std") return "load";
    } 

    if (opName == "sxw64") {
        return "sext_inreg %1%, i32";
    }

    if (opName == "sxb64") {
        return "sext_inreg %1%, i1";
    }

    if (opName == "sxq64") {
        return "sext_inreg %1%, i8";
    }
    if (opName == "sxh64") {
        return "sext_inreg %1%, i16";
    }

    if (opName == "sxhw") {
      return "sext_inreg %1%, i16";
    }
    if (opName == "sxqw") {
      return "sext_inreg %1%, i8";
    }

    if (opName == "sxw")
        return mach_.is64bit() ? "sext_inreg %1%, i64": "sext_inreg %1%, i32";

    if (opName == "sxbw") return "sext_inreg %1%, i1"; 

    if (opName == "truncwh") return "trunc %1%";

    if (opName == "neg") return "ineg %1%";
    if (opName == "not") return "not %1%";

    if (opName == "select") return "select %3%, %1%, %2%";

    // Unknown operation name.
    return "";
}

/**
 * Returns llvm operation name for the given OSAL operation name, if any.
 */
TCEString
TDGen::llvmOperationName(const Operation& op) {
    
    TCEString opName = StringTools::stringToLower(op.name());

    if (opName == "add") return "add";
    if (opName == "sub") return "sub";
    if (opName == "mul") return "mul";
    if (opName == "div") return "sdiv";
    if (opName == "divu") return "udiv";
    if (opName == "mod") return "srem";
    if (opName == "modu") return "urem";

    if (opName == "add64") return "add";
    if (opName == "sub64") return "sub";
    if (opName == "mul64") return "mul";
    if (opName == "div64") return "sdiv";
    if (opName == "divu64") return "udiv";
    if (opName == "mod64") return "srem";
    if (opName == "modu64") return "urem";

    if (opName == "shl") return "shl";
    if (opName == "shr") return "sra";
    if (opName == "shru") return "srl";
    if (opName == "rotl") return "rotl";
    if (opName == "rotr") return "rotr";

    if (opName == "shl64") return "shl";
    if (opName == "shr64") return "sra";
    if (opName == "shru64") return "srl";
    if (opName == "rotl64") return "rotl";
    if (opName == "rotr64") return "rotr";

    if (opName == "and") return "and";
    if (opName == "ior") return "or";
    if (opName == "xor") return "xor";

    if (opName == "and64") return "and";
    if (opName == "ior64") return "or";
    if (opName == "xor64") return "xor";

    if (opName == "eq") return "seteq";
    if (opName == "eq64") return "seteq";
    if (opName == "ne") return "setne";
    if (opName == "ne64") return "setne";
    if (opName == "lt") return "setlt";
    if (opName == "lt64") return "setlt";
    if (opName == "le") return "setle";
    if (opName == "le64") return "setle";
    if (opName == "gt") return "setgt";
    if (opName == "gt64") return "setgt";
    if (opName == "ge") return "setge";
    if (opName == "ltu") return "setult";
    if (opName == "ltu64") return "setult";
    if (opName == "leu") return "setule";
    if (opName == "leu64") return "setule";
    if (opName == "gtu") return "setugt";
    if (opName == "gtu64") return "setugt";
    if (opName == "geu") return "setuge";
    if (opName == "geu64") return "setuge";

    if (opName == "eqf") return "setoeq";
    if (opName == "nef") return "setone";
    if (opName == "ltf") return "setolt";
    if (opName == "lef") return "setole";
    if (opName == "gtf") return "setogt";
    if (opName == "gef") return "setoge";

    if (opName == "equf") return "setueq";
    if (opName == "neuf") return "setune";
    if (opName == "ltuf") return "setult";
    if (opName == "leuf") return "setule";
    if (opName == "gtuf") return "setugt";
    if (opName == "geuf") return "setuge";

    if (opName == "ordf") return "seto";
    if (opName == "uordf") return "setuo";

    if (opName == "addf") return "fadd";
    if (opName == "subf") return "fsub";
    if (opName == "mulf") return "fmul";
    if (opName == "divf") return "fdiv";
    if (opName == "absf") return "fabs";
    if (opName == "negf") return "fneg";
    if (opName == "sqrtf") return "fsqrt";

    if (opName == "cif") return "sint_to_fp";
    if (opName == "cfi") return "fp_to_sint";
    if (opName == "cifu") return "uint_to_fp";
    if (opName == "cfiu") return "fp_to_uint";

#if LLVM_OLDER_THAN_4_0
    if (opName == "cfh") return "fround";
    if (opName == "chf") return "fextend";
#else
    if (opName == "cfh") return "fpround";
    if (opName == "chf") return "fpextend";
#endif

    if (littleEndian_) {
        if (opName == "ld8") return "sextloadi8";
        if (opName == "ldu8") return "zextloadi8";
        if (opName == "ld16") return "sextloadi16";
        if (opName == "ldu16") return "zextloadi16";
        if (mach_.is64bit()) {
            if (opName == "ld32") return "sextloadi32";
            if (opName == "ldu32") return "zextloadi32";
        } else {
            if (opName == "ld32" || opName =="ldu32") return "load";
        }
        if (opName == "ld64") return "load";

        //if (opName == "ldd") return "load";
        
        if (opName == "st8") return "truncstorei8";
        if (opName == "st16") return "truncstorei16";
        if (opName == "st32") {
            if (mach_.is64bit()) {
                return "truncstorei32";
            } else {
                return "store";
            }
        }

        if (opName == "st32") return "store";
        if (opName == "st64") return "store";
        //if (opName == "std") return "load";
    } else { // big-endian
        if (opName == "ldq") return "sextloadi8";
        if (opName == "ldqu") return "zextloadi8";
        if (opName == "ldh") return "sextloadi16";
        if (opName == "ldhu") return "zextloadi16";
        if (opName == "ldw") return "load";
        //if (opName == "ldd") return "load";

        if (opName == "stq") return "truncstorei8";
        if (opName == "sth") return "truncstorei16";
        if (opName == "stw") return "store";
        //if (opName == "std") return "load";
    }

    if (opName.length() >2 && opName.substr(0,2) == "sx") {
        return "sext_inreg";
    }

    if (opName == "truncwh") return "trunc"; 

    if (opName == "neg") return "ineg";
    if (opName == "not") return "not";

    if (opName == "select") return "select";

    // Unknown operation name.
    return "";
}


/**
 * Pattern for tce generated custom op patterns.
 */
std::string
TDGen::tceOperationPattern(const Operation& op) {
    std::string opList = "";
    for (int i = 0; i < op.numberOfInputs(); i++) {
        opList += " %" + Conversion::toString(i+1) + "%";
    }
    return op.name() + opList;
}

/**
 * Check if operation can be matched with llvm pattern.
 *
 * Check if operation has llvmOperationPatters or 
 * one of it's DAGs contain only operations, which can be matched.
 */
bool
TDGen::operationCanBeMatched(
    const Operation& op, std::set<std::string>* recursionCycleCheck,
    bool recursionHasStore) {
    
    char operandChar = mach_.is64bit() ? 's': 'r';
    // if operation has llvm pattern
    if (llvmOperationPattern(op,operandChar) != "") {
        return true;
    }

    std::set<std::string> useSet;
    if (recursionCycleCheck != NULL) {
        useSet = *recursionCycleCheck;
    }    
    useSet.insert(op.name());

    // check if one of dags of operation is ok
    for (int i = 0; i < op.dagCount(); i++) {
        OperationDAG& dag = op.dag(i);
        if (op.dagError(i) != "") {
            std::cerr << "Broken dag in operation " << op.name() <<
                op.dagCode(i) << std::endl;
            assert(0);
        }

        bool dagIsGood = true;
        bool hasStore = false;
      
        for (int j = 0; j < dag.nodeCount(); j++) {
            OperationNode* opNode = dynamic_cast<OperationNode*>(&dag.node(j));
            if (opNode != NULL) {
                Operation& refOp = opNode->referencedOperation();
                if (refOp.writesMemory()) {
                    if (recursionHasStore || hasStore) {
                        dagIsGood = false;
                        break;
                    } else {
                        hasStore = true;
                    }
                }

                // check that the same operation is not used recursively
                if (useSet.count(refOp.name()) != 0) {
                    dagIsGood = false;
                    break;
                }

                // check if referenced op can be matched
                if (!operationCanBeMatched(refOp, &useSet, hasStore)) {
                    dagIsGood =  false;
                    break;
                }
            }
        }
              
        if (dagIsGood) {
            return true;
        }

        // TODO: remove this if we write also others than the
        // first dag of operation pattern.
        break;
    }

    // did not find good dag and operation does not match exact llvm pattern
    return false;
}

/**
 * Returns operation pattern in llvm .td format.
 *
 * @param op Operation to return pattern for.
 * @param dag Operation pattern's DAG.
 * @param  immOp Index of an operand to define as an immediate operand,
 *                or 0, if all operands should be in registers.
 *
 * @return Pattern string.
 */
std::string
TDGen::operationPattern(
    const Operation& op,
    const OperationDAG& dag,
    const std::string& operandTypes) {

    std::string retVal;
    for (OperationDAG::NodeSet::iterator i = dag.endNodes().begin(); 
         i != dag.endNodes().end(); ++i) {
        if (i != dag.endNodes().begin()) {
            retVal += ",";
        }
        const OperationDAGNode& res = **(i);
        retVal += dagNodeToString(op, dag, res, false, operandTypes);
    }
    return retVal;
}

std::string 
TDGen::createDefaultOperandTypeString(const Operation& op) {
    std::string operandTypes;

    int inputs = op.numberOfInputs();
    int outputs = op.numberOfOutputs();

    for (int i = 0; i < outputs; i++) {
        Operand& operand = op.operand(i + inputs +1);
        operandTypes += operandChar(operand);
    }

    for (int i = 0; i < inputs ; i++) {
        Operand& operand = op.operand(i +1);
        operandTypes += operandChar(operand);
    }
    return operandTypes;
}

/**
 * Return operation pattern is llvm .td format without outputs.
 *
 * This pattern can be used as sub-pattern of bigger pattern.
 * The operation must have only one output.
 *
 * @param op Operation to return pattern for.
 * @param dag Operation pattern's DAG.
 * @return Pattern string.
 */
std::string
TDGen::subPattern(
    const Operation& op,
    const OperationDAG& dag) {

    if (dag.endNodes().size() != 1) {
	throw InvalidData(
	    __FILE__,__LINE__,__func__,
	    "Cannot create subpattern: not exactly 1 end node!");
    }

    OperationDAG::NodeSet::iterator i = dag.endNodes().begin(); 
    const OperationDAGNode& res = **(i);
    OperationDAG::NodeSet preds = dag.predecessors(res);
    if (preds.size() != 1) {
	throw InvalidData(
	    __FILE__,__LINE__,__func__,
	    "Cannot create subpattern: not single data source for end node.");
    }
    
    std::string operandTypes = createDefaultOperandTypeString(op);
    // TODO: what about immediate operands?

    // TODO: size of the operand string?
    return dagNodeToString(
        op, dag, **(preds.begin()), false, operandTypes);
}
/**
 * Converts single OperationDAG node to llvm pattern fragment string.
 *
 * @param op Operation that the whole DAG is for.
 * @param dag Whole operation DAG.
 * @param node DAG node to return string for.
 * @param immOp Index of an operand to define as an immediate or 0 if none.
 * @param emulationPattern True, if the returned string should be in
 *                         emulation pattern format.
 * @return DAG node as a llvm .td string.
 */
std::string
TDGen::dagNodeToString(
    const Operation& op, const OperationDAG& dag, const OperationDAGNode& node,
    bool emulationPattern, const std::string& operandTypes) {
    const OperationNode* oNode = dynamic_cast<const OperationNode*>(&node);
    if (oNode != NULL) {
        assert(
            dag.inDegree(*oNode) ==
            oNode->referencedOperation().numberOfInputs());

        return operationNodeToString(
            op, dag, *oNode, emulationPattern, operandTypes);
    }

    const TerminalNode* tNode = dynamic_cast<const TerminalNode*>(&node);
    if (tNode != NULL) {
        const Operand& operand = op.operand(tNode->operandIndex());
        if (dag.inDegree(*tNode) == 0) {
            // Input operand for the whole operation.
            assert(operand.isInput());
            
            char operandType = 
                operandTypes[operand.index()-1 + op.numberOfOutputs()];
            bool imm =  (operandType == 'i' || operandType == 'j');

            if (imm && !canBeImmediate(dag, *tNode)) {
                std::string msg = 
                    "Invalid immediate operand for " + op.name() +
                    " operand #" + Conversion::toString(tNode->operandIndex());
                throw InvalidData(__FILE__, __LINE__, __func__, msg);
            }

            return operandToString(operand, false, operandType);
        } else {
            // Output operand for the whole operation.
            assert(dag.inDegree(*tNode) == 1);
            assert(op.operand(tNode->operandIndex()).isOutput());
            assert(operand.isOutput());
            int globalOperandIndex = 
                tNode->operandIndex() - op.numberOfInputs();
            assert(globalOperandIndex == 1);
            
            const OperationDAGEdge& edge = dag.inEdge(node, 0);
            const OperationDAGNode& srcNode = dag.tailNode(edge);

            // Multiple-output operation nodes not supported in the middle
            // of dag:
            assert(dag.outDegree(srcNode) == 1);
            
            std::string dnString = 
                dagNodeToString(
                    op, dag, srcNode, emulationPattern, operandTypes);

            bool needTrunc = (operandTypes[globalOperandIndex-1] == 'b' &&
                              operandTypes[1] != 'b' &&
                              operandTypes[1] != 'j');

            // handle setcc's without trunc
            // also loads and extends.
            if (needTrunc) {
                if (dnString.substr(0,4) == "(set" ||
                    dnString.substr(0,5) == "(zext" ||
                    dnString.substr(0,5) == "(load") {
                    needTrunc = false;
                }
            }

            if (needTrunc) {
                std::string pattern =
                    "(set " + operandToString(
                        operand, emulationPattern, operandTypes[0])
                    + ", (trunc " + dnString + "))";
                return pattern;
            } else {
                std::string pattern =
                    "(set " + operandToString(
                        operand, emulationPattern, operandTypes[0]) 
                    + ", " + dnString + ")";
                return pattern;
            }
        }
    }

    // Constant values.
    const ConstantNode* cNode = dynamic_cast<const ConstantNode*>(&node);
    if (cNode != NULL) {
        return constantNodeString(op, dag, *cNode, operandTypes);
    }

    abortWithError("Unknown OperationDAG node type.");
    return "";
}

std::string 
TDGen::constantNodeString(
    const Operation& op, 
    const OperationDAG& dag,
    const ConstantNode& node,
    const std::string& operandTypes) {

    assert(dag.inDegree(node) == 0);
    assert(dag.outDegree(node) == 1);
    OperationDAGNode& succ = dag.headNode(dag.outEdge(node,0));
    OperationNode* opn = dynamic_cast<OperationNode*>(&succ);
    OperationDAG::NodeSet siblings = dag.predecessors(*opn);
    for (OperationDAG::NodeSet::iterator i = siblings.begin();
         i!= siblings.end(); i++) {
        const TerminalNode* tNode = dynamic_cast<const TerminalNode*>(*i);
        if (tNode != NULL) {
            const Operand& operand = op.operand(tNode->operandIndex());
            assert(operand.isInput());
            char operandType = 
                operandTypes[operand.index()-1 + op.numberOfOutputs()];
            switch (operandType) {
            case 'j':
            case 'b':
                return "(i1 " + Conversion::toString(node.value()) + ")";
            case 'd':
                return "(f64 " + Conversion::toString(node.value()) +")";
            case 'h':
                return "(f16 " + Conversion::toString(node.value()) + ")";
                // TODO: f16 vectors not yet implemented
            case 'f':
            case 'k':
                return "(f32 " + Conversion::toString(node.value()) + ")";
            case 's':
            case 'a':
                return "(i64 " + Conversion::toString(node.value()) + ")";
            case 'r':
            case 'i':
                return mach_.is64bit() ?
                    "(i64 " + Conversion::toString(node.value()) + ")":
                    "(i32 " + Conversion::toString(node.value()) + ")";
            default:
                break;
            }
        }
    }
    return Conversion::toString(node.value());
}

/**
 * Returns an llvm name for an operation node in an emulation dag.
 *
 * @param op the operation being emulated.
 * @param dag dag of the emulated operation
 * @param node node whose name is being asked
 * @param operandTypes string containing oeprand types for the emulated op.
 */
std::string
TDGen::emulatingOpNodeLLVMName(
    const Operation& op, const OperationDAG& dag, const OperationNode& node,
    const std::string& operandTypes) {
    const Operation& operation = node.referencedOperation();
    std::string operationName = StringTools::stringToUpper(operation.name());


    int inputs = operation.numberOfInputs();

    // Look at outgoing nodes. If operand goes to another op,
    // the value is in register.
    // if it's terminal, get the type from the paramete string.
    for (int i = 1 ; i < operation.numberOfOutputs() + 1; i++) {
        char c = 0;
        for (int e = 0; e < dag.outDegree(node); e++) {
            const OperationDAGEdge& edge = dag.outEdge(node, e);
            int dst = edge.srcOperand();
            if (dst == i + operation.numberOfInputs()) {
                TerminalNode* t = 
                    dynamic_cast<TerminalNode*>(
                        &(dag.headNode(edge)));
                if (t != NULL) {
                    int strIndex = t->operandIndex() -1 -
                        op.numberOfInputs();
                    assert((int)operandTypes.length() > strIndex &&
                           strIndex >= 0);
                    if (c != 0 && c != operandTypes[strIndex]) {
                        throw InvalidData(__FILE__,__LINE__,__func__,
                                          "conflicting output types!");
                    }
                    c = operandTypes[strIndex];
                } else {
                    Operand &operand = 
                        operation.operand(i+operation.numberOfInputs());
                    char type = operandChar(operand);
                    if (c != 0 && c!= type) {
                        throw InvalidData(__FILE__,__LINE__,__func__,
                                              "conflicting output types!");
                    }
                    c = type;
                }
            }
        }
        if (c == 0) {
            throw InvalidData(__FILE__,__LINE__,__func__,"output not found.");
        }
        operationName += c;
    }

    // Look at incoming nodes. If operand comes from another op,
    // the value is in register. If operand comes from constant,
    // it's immediate.
    // if it's terminal, get the type from the parm string.
    for (int i = 1; i < inputs + 1; i++) {
        Operand &operand = operation.operand(i);
        for (int e = 0; e < dag.inDegree(node); e++) {
            const OperationDAGEdge& edge = dag.inEdge(node, e);
            int dst = edge.dstOperand();
            if (dst == i) {
                if (dynamic_cast<OperationNode*>(&(dag.tailNode(edge)))) {
                    operationName += operandChar(operand);
                } else {
                    if (dynamic_cast<ConstantNode*>(
                            &(dag.tailNode(edge)))) {
                        if (operand.type() == Operand::SINT_WORD ||
                            operand.type() == Operand::UINT_WORD) {
                            operationName += 'i';
                        } else if (operand.type() == Operand::RAW_DATA) {
                            operationName += mach_.is64bit() ? 'a' : 'i';
                        } else if (operand.type() == Operand::SLONG_WORD ||
                                   operand.type() == Operand::ULONG_WORD) {
                            operationName += 'a';
                        }
                    } else {
                        TerminalNode* t = 
                            dynamic_cast<TerminalNode*>(
                                &(dag.tailNode(edge)));
                        assert (t != NULL);
                        int strIndex = t->operandIndex() -1 + 
                            op.numberOfOutputs();
                        if (!((int)operandTypes.length() > strIndex &&
                              strIndex > 0)) {
                            std::cerr << "problem wiht emulation dag: "
                                      << "strindex: " << strIndex
                                      << " op name: " << op.name() << std::endl;
                        }
                        assert((int)operandTypes.length() > strIndex &&
                               strIndex > 0);
                        operationName += operandTypes[strIndex];
                    }
                }
            }
        }
    }
    return operationName;
}

/**
 * Converts OSAL dag operation node to llvm .td pattern fragment string.
 *
 * @param op Operation which this operation node is part of.
 * @param dag Parent DAG of the operation node.
 * @param node Node to convert to string.
 * @param immOp Index of an operand to define as immediate or 0 if none.
 * @param emulationPattern True, if the string should be in emulation pattern
 *                         format.
 */
std::string
TDGen::operationNodeToString(
    const Operation& op, const OperationDAG& dag, const OperationNode& node,
    bool emulationPattern, const std::string& operandTypes) {
    const Operation& operation = node.referencedOperation();

    std::string operationPat;

    if (emulationPattern) {
        operationPat = emulatingOpNodeLLVMName(
            op, dag, node, operandTypes);
        
        for (int i = 0; i < operation.numberOfInputs(); i++) {
            if (i > 0) {
                operationPat += ", ";
            } else {
                operationPat += " ";
            }
            operationPat =
                operationPat + "%" + Conversion::toString(i + 1) + "%";
        }
    } else {
        operationPat = llvmOperationPattern(operation, operandTypes[0]);
        
        // generate pattern for operation if not llvmOperation (can match 
        // custom op patterns)
        if (operationPat == "") {
            if (operationCanBeMatched(operation)) {
                return subPattern(operation, operation.dag(0));
            } else {
                operationPat = tceOperationPattern(operation);
            }
        }
    }

    if (operationPat == "") {
        std::string msg("Unknown operation node in dag: " + 
                        std::string(operation.name()));
        
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }

    boost::format pattern(operationPat);

    int inputs = operation.numberOfInputs();
#ifndef NDEBUG
    int outputs = 
#endif
    operation.numberOfOutputs();

    assert(outputs == 0 || outputs == 1);
    
    for (int i = 1; i < inputs + 1; i++) {
        for (int e = 0; e < dag.inDegree(node); e++) {
            const OperationDAGEdge& edge = dag.inEdge(node, e);
            int dst = edge.dstOperand();
            if (dst == i) {
                const OperationDAGNode& in = dag.tailNode(edge);
                std::string dagNodeString =  dagNodeToString(
                    op, dag, in, emulationPattern, operandTypes);
                try {
                    pattern % dagNodeString;
                } catch(...) {
                    TCEString msg = "Boost format threw exception! ";
                    msg << "Input format: " << operationPat;
                    throw InvalidData(__FILE__, __LINE__, __func__, msg);
                }
            }
        }
    }

    return std::string("(") + pattern.str() + ")";
}

/**
 * Returns llvm .td format string for an OSAL operand.
 *
 * @param operand Operand to return string for.
 * @param match True, if the string should be in the matching pattern format.
 * @param immediate True, if the operand should be defined as an immediate.
 * @return Operand string to be used in a llvm .td pattern.
 */
std::string
TDGen::operandToString(
    const Operand& operand,
    bool match,
    char operandType) {
    int idx = operand.index();

    if (operand.isAddress()) {
        switch (operandType) {
        case 'i':
        case 'a':
            if (match) {
                return "MEMri:$op" + Conversion::toString(idx);
            } else {
                return "ADDRri:$op" + Conversion::toString(idx);
            }
        case 'r':
        case 's':
            if (match) {
                return  "MEMrr:$op" + Conversion::toString(idx);
            } else {
                return  "ADDRrr:$op" + Conversion::toString(idx);
            }
        default:
            std::string msg = 
                "invalid operation type for mem operand:";
            msg += operandType;
            throw InvalidData(__FILE__, __LINE__, __func__, msg);
        }
    } else if (operand.type() == Operand::SINT_WORD ||
               operand.type() == Operand::UINT_WORD ||
               operand.type() == Operand::RAW_DATA ||
               operand.type() == Operand::BOOL) {

        // imm
        switch (operandType) {
        case 'a':
            if (match) {
                return "i64imm:$op" + Conversion::toString(idx);
            } else {
                return "(i64 imm:$op" + Conversion::toString(idx) + ")";
            }
        case 'i':
            if (match) {
                return mach_.is64bit() ?
                    "i64imm:$op" + Conversion::toString(idx):
                    "i32imm:$op" + Conversion::toString(idx);
            } else {
                return mach_.is64bit() ?
                    "(i64 imm:$op" + Conversion::toString(idx) + ")":
                    "(i32 imm:$op" + Conversion::toString(idx) + ")";
            }
        case 'j':
            if (match) {
                return "i1imm:$op" + Conversion::toString(idx);
            } else {
                return "(i1 imm:$op" + Conversion::toString(idx) + ")";
            }
        case 'r':
            return mach_.is64bit() ?
                "R64IRegs:$op" + Conversion::toString(idx):
                "R32IRegs:$op" + Conversion::toString(idx);
        case 's':
            return "R64IRegs:$op" + Conversion::toString(idx);
        case 'b':
            return "R1Regs:$op" + Conversion::toString(idx);
        case 'f':
            if (operand.type() == Operand::RAW_DATA) {
                return "FPRegs:$op" + Conversion::toString(idx);
            }
            /* fall through */
        case 'h':
            if (operand.type() == Operand::RAW_DATA) {
                return "HFPRegs:$op" + Conversion::toString(idx);
            }
            /* fall through */
        default:
            std::string msg = 
                "invalid operation type for integer operand:";
            msg += operandType;
            throw InvalidData(__FILE__, __LINE__, __func__, msg);
        }
    } else if (operand.type() == Operand::FLOAT_WORD ) {

        switch (operandType) {
        case 'i':
        case 'k':
            if (match) {
                return "f32imm:$op" + Conversion::toString(idx);
            } else {
                return "(f32 fpimm:$op" + Conversion::toString(idx) + ")";
            }
        case 'r':
        case 'f':
            return "FPRegs:$op" + Conversion::toString(idx);

        default:
            std::string msg = 
                "invalid operation type for float operand:";
            msg += operandType;
            throw InvalidData(__FILE__, __LINE__, __func__, msg);
        }
    } else if (operand.type() == Operand::HALF_FLOAT_WORD) {

        switch (operandType) {
        case 'i':
        case 'k':
        case 'l':
            if (match) {
                return "f16imm:$op" + Conversion::toString(idx);
            } else {
                return "(f16 fpimm:$op" + Conversion::toString(idx) + ")";
            }
        case 'r':
        case 'h':
            return "HFPRegs:$op" + Conversion::toString(idx);
        default:
            std::string msg = 
                "invalid operation type for half operand:";
            msg += operandType;
            throw InvalidData(__FILE__, __LINE__, __func__, msg);
        }
    } else if (operand.type() == Operand::DOUBLE_WORD) {
        // TODO: immediate check??
        return "I64Regs:$op" + Conversion::toString(idx);
    } else if (operand.type() == Operand::SLONG_WORD || operand.type() == Operand::ULONG_WORD) {
        if (operandType == 'b') {
            return "R1Regs:$op" + Conversion::toString(idx);
        }
        if (operandType == 'j') {
            if (match) {
                return "i1imm:$op" + Conversion::toString(idx);
            } else {
                return "(i1 imm:$op" + Conversion::toString(idx) + ")";
            }
        }
        if (operandType == 'a') {
            return match ?
                "i64imm:$op" + Conversion::toString(idx) :
                "(i64 imm:$op" + Conversion::toString(idx) + ")";
        }
        return "R64IRegs:$op" + Conversion::toString(idx);
    } else {
        std::cerr << "Unknown operand type: " << operandType << std::endl;
        assert(false && "Unknown operand type.");
    }
    abortWithError("Unknown operand type on osal? Should not get here.");
    return "";
}

/**
 * Returns llvm input definition list for an operation.
 *
 * @param op Operation to define inputs for.
 * @return String defining operation inputs in llvm .td format.
 */
std::string
TDGen::patInputs(const Operation& op, const std::string& inputTypes) {
    std::string ins;
    for (int i = 0; i < op.numberOfInputs(); i++) {
        if (i > 0) {
            ins += ",";
        }
        ins += operandToString(
            op.operand(i + 1), true, inputTypes[i+op.numberOfOutputs()]);
    }
    return ins;
}


/**
 * Returns llvm output definition list for an operation.
 *
 * @param op Operation to define outputs for.
 * @return String defining operation outputs in llvm .td format.
 */
std::string
TDGen::patOutputs(const Operation& op, const std::string& operandTypes) {
    std::string outs;
    for (int i = 0; i < op.numberOfOutputs(); i++) {
        assert(op.operand(op.numberOfInputs() + 1 + i).isOutput());
        outs += (i > 0) ? (",") : (" ");
        outs += operandToString(
            op.operand(op.numberOfInputs() + 1 + i), true, operandTypes[i]);
    }
    return outs;
}

/**
 * Creates a dummy dag for an OSAL operation.
 *
 * @param op Operation to create OperationDAG for.
 * @return OperationDAG consisting of only one operation node referencing
 *         the given operation.
 */
OperationDAG*
TDGen::createTrivialDAG(Operation& op) {

    OperationDAG* dag = new OperationDAG(op.impl());
    OperationNode* opNode = new OperationNode(op);
    dag->addNode(*opNode);

    for (int i = 0; i < op.numberOfInputs() + op.numberOfOutputs(); i++) {
        const Operand& operand = op.operand(i + 1);
        TerminalNode* t = new TerminalNode(operand.index());
        dag->addNode(*t);
        if (operand.isInput()) {
            OperationDAGEdge* e = new OperationDAGEdge(1, operand.index());
            dag->connectNodes(*t, *opNode, *e);
        } else {
            OperationDAGEdge* e = new OperationDAGEdge(operand.index(), 1);
            dag->connectNodes(*opNode, *t, *e);
        }
    }
    return dag;
}


/**
 * Returns true if the operand corresponding to the given TerminalNode in
 * an OperationDAG can be immediate in the llvm pattern.
 *
 * @param dag DAG of the whole operation.
 * @param node TerminalNode corresponding to the operand queried.
 */
bool
TDGen::canBeImmediate(
    const OperationDAG& dag, const TerminalNode& node) {

    if (dag.inDegree(node) != 0) {
        return false;
    }

    for (int i = 0; i < dag.outDegree(node); i++) {

        const OperationDAGEdge& edge = dag.outEdge(node, i);
        const OperationDAGNode& dstNode = dag.headNode(edge);
        const OperationNode* opNode =
            dynamic_cast<const OperationNode*>(&dstNode);

        if (opNode == NULL) {
            return false;
        }

    }
    return true;
}

void
TDGen::generateLoadStoreCopyGenerator(std::ostream& os) {
    // vector store/load generation code

    TCEString prefix = "&"; // address of -operator
    TCEString rcpf = "RegsRegClass";
    TCEString rapf = "TCE::RARegRegClass";
    TCEString boolStore = littleEndian_ ? "ST8Brb;" : "STQBrb;";
    TCEString intStore =  littleEndian_ ? "ST32"   : "STW";
    TCEString halfStore = littleEndian_ ? "ST16"   : "STH";
    TCEString longStore = "ST64";

    os << "#include <stdio.h>" << std::endl 
       << "int GeneratedTCEPlugin::getStore(const TargetRegisterClass *rc)"
       << " const {" << std::endl;
    
    os << "\tif (rc == " << prefix << rapf
       << ") return TCE::" << intStore << "RArr;"
       << std::endl;

    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        if (ri->first.find("R1") == 0) {
            os << "\tif (rc == " << prefix << "TCE::" << ri->first
               << rcpf << ") return TCE::" << boolStore << std::endl;
        }
        if (ri->first.find("R32") == 0) {
            os << "\tif (rc == " << prefix << "TCE::" << ri->first
               << rcpf << ") return TCE::" << intStore << "rr;" << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::"  << ri->first
               << "I" << rcpf << ") return TCE::" << intStore << "rr;" << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::"  << ri->first
               << "FP" << rcpf << ") return TCE::" << intStore << "fr;" << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::"  << ri->first
               << "HFP" << rcpf << ") return TCE::" << halfStore << "hr;" << std::endl;
        }
        if (mach_.is64bit()) {
            if (ri->first.find("R64") == 0) {
                os << "\tif (rc == " << prefix << "TCE::" << ri->first
                   << rcpf << ") return TCE::" << longStore << "ss;" << std::endl;
            }

            if (ri->first.find("R64") == 0) {
                os << "\tif (rc == " << prefix << "TCE::" << ri->first
                   << "I" << rcpf << ") return TCE::" << longStore <<  "ss;" << std::endl;
            }

            if (ri->first.find("R64") == 0) {
                os << "\tif (rc == &TCE::" << ri->first << "FP" << rcpf
                   << ") return TCE::" << longStore <<  "fs;" << std::endl;
            }

            if (ri->first.find("R64") == 0) {
                os << "\tif (rc == &TCE::" << ri->first << "HFP" << rcpf
                   << ") return TCE::" << longStore <<  "hs;" << std::endl;
            }
        }
    }

    if (use64bitForFP_) {
        os << "\tif (rc == &TCE::FPRegsRegClass) return TCE::ST64fs;"
           << std::endl;

        os << "\tif (rc == &TCE::HFPRegsRegClass) return TCE::ST64hs;"
           << std::endl;
    } else {
        os << "\tif (rc == &TCE::FPRegsRegClass) return TCE::"
           << intStore << "fr;" << std::endl;

        os << "\tif (rc == &TCE::HFPRegsRegClass) return TCE::"
           << intStore << "hr;" << std::endl;
    }

    for (RegClassMap::iterator ri = regsInRFClasses_.begin(); 
         ri != regsInRFClasses_.end(); ri++) {
            os << "\tif (rc == " 
               << prefix << "TCE::R32_"  << ri->first << "_"
               << rcpf << ") return TCE::" <<intStore << "rr;" << std::endl;    
    }
    
    TCEString boolLoad = littleEndian_ ? "LD8" : "LDQ";
    if (!mach_.hasOperation(boolLoad)) {
        boolLoad = littleEndian_ ? "LDU8" : "LDQU";
        if (!mach_.hasOperation(boolLoad)) {
            boolLoad="";
        }
    }

    TCEString intLoad = littleEndian_ ? "LD32" : "LDW";
    TCEString halfLoad = littleEndian_ ? "LD16" : "LDH";
    TCEString longLoad = "LD64";
    if (!mach_.hasOperation(halfLoad)) {
        halfLoad = littleEndian_ ? "LDU16;" : "LDHU";
        if (!mach_.hasOperation(halfLoad)) {
            halfLoad="";
        }
    }

#ifdef LLVM_3_5
    os  << "\tprintf(\"regclass: %s\\n\", rc->getName());" << std::endl
#else
#ifdef LLVM_OLDER_THAN_5_0
    os  << "\tprintf(\"regclass: of size %d \\n\", rc->getSize());" << std::endl
#elif LLVM_OLDER_THAN_6_0
    os  << "\tprintf(\"regclass of size %d \\n\",rc->SpillSize);" << std::endl
#elif LLVM_OLDER_THAN_8
    os  << "\tprintf(\"regclass of size %d \\n\",rc->MC->getSize());"
        << std::endl
#else
    os  << "\tprintf(\"regclass of size %d \\n\",rc->MC->RegsSize);"
        << std::endl
#endif
#endif
        << "\tassert(0&&\"Storing given regclass to stack not supported. "
        << "Bug in backend?\");"
        << std::endl
        << "} " << std::endl
        << std::endl
              
        << "int GeneratedTCEPlugin::getLoad(const TargetRegisterClass *rc)"
        << " const {" << std::endl;

    os << "\tif (rc == " << prefix << rapf << ") return TCE::" << intLoad << "RAr;"
       << std::endl;

    if (!mach_.hasOperation(intLoad) && mach_.hasOperation("LDU32")) {
        intLoad = "LDU32";
    }

    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        if (ri->first.find("R1") == 0) {
            if (boolLoad != "") {
                os << "\tif (rc == " << prefix << "TCE::" << ri->first
                   << rcpf <<") return TCE::" << boolLoad << "Br;" << std::endl;
            } else {
                os << "\tif (rc == " << prefix << "TCE::" << ri->first
                   << rcpf <<") return TCE::" << intLoad << "Br;" << std::endl;
            }
        }
        if (ri->first.find("R32") == 0) {
            os << "\tif (rc == " << prefix << "TCE::" << ri->first
               << rcpf << ") return TCE::" << intLoad << "rr;" << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::" << ri->first
               << "I" << rcpf << ") return TCE::" << intLoad << "rr;" << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::" << ri->first
               << "FP" << rcpf << ") return TCE::" << intLoad << "fr;" << std::endl;
            
            if (halfLoad != "") {
                os << "\tif (rc == " << prefix << "TCE::" << ri->first
                   << "HFP" << rcpf << ") return TCE::" << halfLoad << "hr;" << std::endl;
            }
        }

        if (mach_.is64bit()) {
            if (ri->first.find("R64") == 0) {
                os << "\tif (rc == " << prefix << "TCE::" << ri->first
                   << rcpf << ") return TCE::" << longLoad << "ss;" << std::endl;

                os << "\tif (rc == " << prefix << "TCE::" << ri->first
                   << "I" << rcpf << ") return TCE::" << longLoad << "ss;" << std::endl;

                os << "\tif (rc == &TCE::" << ri->first << "FP" << rcpf
                   << ") return TCE::" << longLoad << "fs;" << std::endl;

                os << "\tif (rc == &TCE::" << ri->first << "HFP" << rcpf
                   << ") return TCE::" << longLoad << "hs;" << std::endl;
            }
        }
        // TODO: double load!

    }

    for (RegClassMap::iterator ri = regsInRFClasses_.begin(); 
         ri != regsInRFClasses_.end(); ri++) {
 
            os << "\tif (rc == " << prefix 
               << "TCE::R32_"  << ri->first << "_"
               << rcpf << ") return TCE::" << intLoad << "rr;" << std::endl;
            
    }

    if (use64bitForFP_) {
        os << "\tif (rc == &TCE::FPRegsRegClass) return TCE::LD64fs;"
           << std::endl;

        os << "\tif (rc == &TCE::HFPRegsRegClass) return TCE::LD64hs;"
           << std::endl;
    } else {
        os << "\tif (rc == &TCE::FPRegsRegClass) return TCE::" << intLoad << "fr;"
           << std::endl;

        os << "\tif (rc == &TCE::HFPRegsRegClass) return TCE::" << intLoad << "hr;"
           << std::endl;
    }

#ifdef LLVM_3_5
    os  << "\tprintf(\"regclass: %s\\n\", rc->getName());" << std::endl
#else
#ifdef LLVM_OLDER_THAN_5_0
    os  << "\tprintf(\"regclass: of size %d \\n\", rc->getSize());" << std::endl
#elif LLVM_OLDER_THAN_6_0
    os  << "\tprintf(\"regclass of size %d \\n\",rc->SpillSize);" << std::endl
#elif LLVM_OLDER_THAN_8
    os  << "\tprintf(\"regclass of size %d \\n\",rc->MC->getSize());"
        << std::endl
#else
    os  << "\tprintf(\"regclass of size %d \\n\",rc->MC->RegsSize);"
        << std::endl
#endif
#endif
        << "\tassert(0&&\"loading from stack to given regclass not supported."
        << " Bug in backend?\");"
        << std::endl
        << "} " << std::endl
        << std::endl;
}

void
TDGen::createMinMaxGenerator(std::ostream& os) {

    bool is64bit = mach_.is64bit();
    // MIN
    os  << "int GeneratedTCEPlugin::getMinOpcode(SDNode* n) const {" << std::endl
        << "\tEVT vt = n->getOperand(1).getValueType();" << std::endl;
    if (!is64bit) {
        if (opNames_.find("MINrrr") != opNames_.end()) {
            os << "if (vt == MVT::i32) return TCE::MINrrr;" << std::endl;
        }
    } else {
        if (opNames_.find("MIN64sss") != opNames_.end()) {
            os << "if (vt == MVT::i64) return TCE::MIN64sss;" << std::endl;
        }
    }
    if (opNames_.find("MINFfff") != opNames_.end()) {
        os << "if (vt == MVT::f32) return TCE::MINFfff;" << std::endl;
    }
    os << "\treturn -1; " << std::endl << "}" << std::endl;

    // MAX
    os  << "int GeneratedTCEPlugin::getMaxOpcode(SDNode* n) const {" << std::endl
        << "\tEVT vt = n->getOperand(1).getValueType();" << std::endl;
    if (!is64bit) {
        if (opNames_.find("MAXrrr") != opNames_.end()) {
            os << "if (vt == MVT::i32) return TCE::MAXrrr;" << std::endl;
        }
    } else {
        if (opNames_.find("MAX64sss") != opNames_.end()) {
            os << "if (vt == MVT::i64) return TCE::MAX64sss;" << std::endl;
        }

    }
    if (opNames_.find("MAXFfff") != opNames_.end()) {
        os << "if (vt == MVT::f32) return TCE::MAXFfff;" << std::endl;
    }
    os << "\treturn -1; " << std::endl << "}" << std::endl;
    
    // MINU
    os  << "int GeneratedTCEPlugin::getMinuOpcode(SDNode* n) const {" << std::endl;
    os << "\tEVT vt = n->getOperand(1).getValueType();" << std::endl;
    if (!is64bit) {
        if (opNames_.find("MINUrrr") != opNames_.end()) {
            os << "if (vt == MVT::i32) return TCE::MINUrrr;" << std::endl;
        }
    } else {
        if (opNames_.find("MINU64sss") != opNames_.end()) {
            os << "if (vt == MVT::i64) return TCE::MINU64sss;" << std::endl;
        }
    }
    os << "\treturn -1; " << std::endl << "}" << std::endl;

    // MAXU
    os  << "int GeneratedTCEPlugin::getMaxuOpcode(SDNode* n) const {" << std::endl;
    os << "\tEVT vt = n->getOperand(1).getValueType();" << std::endl;

    if (!is64bit) {
        if (opNames_.find("MAXUrrr") != opNames_.end()) {
            os << "if (vt == MVT::i32) return TCE::MAXUrrr;" << std::endl;
        }
    } else {
        if (opNames_.find("MAXU64sss") != opNames_.end()) {
            os << "if (vt == MVT::i64) return TCE::MAXU64sss;" << std::endl;
        }
    }
    os << "\treturn -1; " << std::endl << "}" << std::endl;
}

void TDGen::createEndiannesQuery(std::ostream& os) {
    os << "bool GeneratedTCEPlugin::isLittleEndian() const {" << std::endl;
    os << "return " << littleEndian_ << "; }" << std::endl;
    os << "bool GeneratedTCEPlugin::is64bit() const {" << std::endl;
    os << "return " << mach_.is64bit() << "; }" << std::endl;
}

void TDGen::createByteExtLoadPatterns(std::ostream& os) {
    TCEString load = littleEndian_ ? "LD8" : "LDQ";
    TCEString uload = littleEndian_ ? "LDU8" : "LDQU";
    TCEString destSize = mach_.is64bit() ? "64" : "32";
    TCEString ANDOP = mach_.is64bit() ? "AND64ssa" : "ANDrri";
    TCEString SUBIMM = mach_.is64bit() ? "SUB64sas" : "SUBrir";
    TCEString EXTOP = mach_.is64bit() ? "SXQ64" : "SXQW";
    TCEString EXTOPC = mach_.is64bit() ? "SXQ64sr" : "SXQWrr";
    TCEString dstTypeChar = mach_.is64bit() ? 's' : 'r';
    TCEString regSrcChar = mach_.is64bit() ? 's' : 'r';
    TCEString immSrcChar = mach_.is64bit() ? 'a' : 'i';
    TCEString loadOpcReg = load + dstTypeChar + regSrcChar;
    TCEString loadOpcImm = load + dstTypeChar + immSrcChar;

    if (mach_.hasOperation(load)) {
        if (!mach_.hasOperation(uload)) {

            // emulate zero ext with sing-ext and and
            os << "def : Pat<(i" << destSize << " (zextloadi8 ADDRrr:$addr)), "
               << "(" << ANDOP << " ("
               << loadOpcReg << " ADDRrr:$addr), 255)>;"
               << std::endl;
            os << "def : Pat<(i" << destSize << " (zextloadi8 ADDRri:$addr)), "
               << "(" << ANDOP << " ("
               << loadOpcImm << " ADDRri:$addr), 255)>;"
               << std::endl;
        }
    } else {
        // if no sign ext load, try zero ext load
        if (!mach_.hasOperation(uload)) {
            std::cerr << "Warning: The architecture is missing any 8-bit loads."
                      << " All code may not compile!"
                      << std::endl;
            return;
        }
        loadOpcReg = uload + dstTypeChar + regSrcChar;
        loadOpcImm = uload + dstTypeChar + immSrcChar;

        if (mach_.hasOperation(EXTOP)) {
            // use zextload + sext for sextload
            os << "def : Pat<(i" << destSize
               << " (sextloadi8 ADDRrr:$addr)), "
               << "(" << EXTOPC << " ("
               << loadOpcReg << " ADDRrr:$addr))>;" << std::endl;
            os << "def : Pat<(i" << destSize
               << " (sextloadi8 ADDRri:$addr)), "
               << "(" << EXTOPC << " ("
               << loadOpcImm << " ADDRri:$addr))>;" << std::endl;

        } else {
            std::cerr << "Warning: no sign-extending 8-bit loads or"
                      << " 8-bit sign extension instruction!"
                      << " in the processor. All code may not compile!"
                      << std::endl;
        }
    }

    os << "def : Pat<(i" << destSize << " (zextloadi1 ADDRrr:$addr)), ("
       << loadOpcReg << " ADDRrr:$addr)>;"
       << std::endl
       << "def : Pat<(i" << destSize << " (zextloadi1 ADDRri:$addr)), ("
       << loadOpcImm << " ADDRri:$addr)>;"
       << std::endl;

    os << "def : Pat<(i" << destSize << " (sextloadi1 ADDRrr:$addr)), "
       << "(" << SUBIMM << " 0, "
       << "(" << ANDOP << " ("
       << loadOpcReg << " ADDRrr:$addr), 1))>;"
       << std::endl
       << "def : Pat<(i" << destSize << " (sextloadi1 ADDRri:$addr)), "
       << "(" << SUBIMM << " 0, "
       <<  "(" << ANDOP << " ("
       << loadOpcImm << " ADDRri:$addr), 1))>;"
       << std::endl
       << "// anyextloads" << std::endl;

    os << "def : Pat<(i" << destSize << " (extloadi1 ADDRrr:$src)), ("
       << loadOpcReg << " ADDRrr:$src)>;" << std::endl
       << "def : Pat<(i" << destSize << " (extloadi1 ADDRri:$src)), ("
       << loadOpcImm << " ADDRri:$src)>;" << std::endl
       << "def : Pat<(i" << destSize << " (extloadi8 ADDRrr:$src)), ("
       << loadOpcReg << " ADDRrr:$src)>;" << std::endl
       << "def : Pat<(i" << destSize << " (extloadi8 ADDRri:$src)), ("
       << loadOpcImm << " ADDRri:$src)>;" << std::endl
       << std::endl;
}

void TDGen::createShortExtLoadPatterns(std::ostream& os) {
    TCEString load = littleEndian_ ? "LD16" : "LDH";
    TCEString destSize = mach_.is64bit() ? "64" : "32";
    TCEString ANDOPC = mach_.is64bit() ? "AND64ssa" : "ANDrri";
    TCEString uload = littleEndian_ ? "LDU16" : "LDHU";
    TCEString EXTOP = mach_.is64bit() ? "SXH64" : "SXHW";
    TCEString EXTOPC = mach_.is64bit() ? "SXH64sr" : "SXHWrr";
    TCEString dstTypeChar = mach_.is64bit() ? 's' : 'r';
    TCEString regSrcChar = mach_.is64bit() ? 's' : 'r';
    TCEString immSrcChar = mach_.is64bit() ? 'a' : 'i';
    TCEString loadOpcReg = load + dstTypeChar + regSrcChar;
    TCEString loadOpcImm = load + dstTypeChar + immSrcChar;

    if (mach_.hasOperation(load)) {
        if (!mach_.hasOperation(uload)) {
            // emulate zero ext with sing-ext and and
            os << "def : Pat<(i" << destSize
               << " (zextloadi16 ADDRrr:$addr)), "
               << "(" << ANDOPC << " (" << loadOpcReg
               << " ADDRrr:$addr), 65535)>;" << std::endl;
            os << "def : Pat<(i" << destSize
               << " (zextloadi16 ADDRri:$addr)), "
               << "(" << ANDOPC << " ("
               << loadOpcImm << " ADDRri:$addr), 65535)>;" << std::endl;
        }
    } else {
        if (!mach_.hasOperation(uload)) {
            std::cerr << "Warning: The architecture is missing any 16-bit loads."
                      << std::endl;
            return;
        }
        loadOpcReg = uload + dstTypeChar + regSrcChar;
        loadOpcImm = uload + dstTypeChar + immSrcChar;

        if (mach_.hasOperation(EXTOP)) {
            // use zextload + sext for sextload
            os << "def : Pat<(i" << destSize
               << " (sextloadi16 ADDRrr:$addr)), "
               << "(" << EXTOPC
               << " (" << loadOpcReg << " ADDRrr:$addr))>;" << std::endl;
            os << "def : Pat<(i" << destSize
               << " (sextloadi16 ADDRri:$addr)), "
               << "(" << EXTOPC
               << " (" << loadOpcImm << " ADDRri:$addr))>;" << std::endl;
        } else {
            std::cerr << "Warning: no sign-extending 16-bit loads or"
                      << " 16-bit sign extension instruction!"
                      << " in the processor. All code may not compile!"
                      << std::endl;
        }
    }
    // anyext
    os << "def : Pat<(i" << destSize << " (extloadi16 ADDRrr:$src)), ("
       << loadOpcReg << " ADDRrr:$src)>;" << std::endl
       << "def : Pat<(i" << destSize << " (extloadi16 ADDRri:$src)), ("
       << loadOpcImm << " ADDRri:$src)>;" << std::endl;
}

void TDGen::create32BitExtLoadPatterns(std::ostream& os) {
    TCEString load = "LD32";
    const TCEString uload = "LDU32";

    if (mach_.hasOperation(load)) {
        if (!mach_.hasOperation(uload)) {
            // emulate zero ext with sing-ext and and
            os << "def : Pat<(i64 (zextloadi32 ADDRrr:$addr)), "
               << "(AND64ssa (LD32sr ADDRrr:$addr),"
               << "0xffffffff)>;" << std::endl;

            os << "def : Pat<(i64 (zextloadi32 ADDRri:$addr)), "
               << "(AND64ssa (LD32si ADDRri:$addr),"
               << "0xffffffff)>;" << std::endl;
        }
    } else {
        if (!mach_.hasOperation(uload)) {
            std::cerr << "Warning: The architecture is missing any 16-bit loads."
                      << std::endl;
            return;
        }
        load = uload;

        if (mach_.hasOperation("SXW64")) {
            // use zextload + sext for sextload
            os << "def : Pat<(i64 (sextloadi32 ADDRrr:$addr)), "
               << "(SXW64sr (LDU32ss ADDRrr:$addr))>;" << std::endl;

            os << "def : Pat<(i64 (sextloadi32 ADDRri:$addr)), "
               << "(SXW64sr (LDU32sa ADDRri:$addr))>;" << std::endl;
        } else {
            std::cerr << "Warning: no sign-extending 32-bit loads or"
                      << " 32-bit sign extension instruction!"
                      << " in the processor. All code may not compile!"
                      << std::endl;
        }
    }
    // anyext.
    os << "def : Pat<(i64 (extloadi32 ADDRrr:$src)), "
       << "(" << load << "ss ADDRrr:$src)>;" << std::endl

       << "def : Pat<(i64 (extloadi32 ADDRri:$src)), "
       << "(" << load << "sa ADDRrr:$src)>;" << std::endl;
}

void
TDGen::writeCallingConv(std::ostream& os) {
    writeCallingConvLicenceText(os);

    os << "// Function return value types." << std::endl;
    os << "def RetCC_TCE : CallingConv<[" << std::endl;
    if (mach_.is64bit()) {
        os << "  CCIfType<[i1,i8,i16,i32], CCPromoteToType<i64>>," << std::endl
           << "  CCIfType<[i64], CCAssignToReg<[IRES0]>>," << std::endl
           << "  CCIfType<[f64], CCAssignToReg<[IRES0]>>," << std::endl;
        os << "  CCIfType<[f32], CCAssignToReg<[IRES0]>>," << std::endl
           << "  CCAssignToStack<8, 8>" << std::endl;
    } else {
        os << "  CCIfType<[i1,i8,i16], CCPromoteToType<i32>>," << std::endl
           << "  CCIfType<[i32], CCAssignToReg<[IRES0]>>," << std::endl;
        os << "  CCIfType<[f32], CCAssignToReg<[IRES0]>>," << std::endl
           << "  CCIfType<[f16], CCAssignToReg<[IRES0]>>," << std::endl
           << "  CCAssignToStack<4, 4>" << std::endl;
    }

    os  << "]>;" << std::endl << std::endl;

    os << "// Function argument value types." << std::endl;
    os << 
        "def CC_TCE : CallingConv<[" << std::endl;
    if (mach_.is64bit()) {
        os << "  CCIfType<[i1, i8, i16, i32], CCPromoteToType<i64>>," << std::endl
           << "  CCIfType<[i64, f16, f32], CCAssignToReg<[IRES0]>>," << std::endl;
    } else {
        os << "  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>," << std::endl <<
            "  CCIfType<[i32,f32,f16], CCAssignToReg<[IRES0]>>," << std::endl;
    }

    if (!mach_.is64bit()) {
        os <<
            "  // Integer values get stored in stack slots that are 4 bytes in "
           << std::endl <<
            "  // size and 4-byte aligned." << std::endl <<
            "  CCIfType<[i32, f32, f16], CCAssignToStack<4, 4>>," << std::endl <<
            "  // Integer values get stored in stack slots that are 8 bytes in"
           << std::endl <<
            "  // size and 8-byte aligned." << std::endl <<
            "  CCIfType<[f64], CCAssignToStack<8, 8>>" << std::endl;
    } else {
        os << "  // All values get stored in stack slots that are 8 bytes in "
           << std::endl <<
            "  // size and 8-byte aligned." << std::endl <<
            "  CCIfType<[i64, f64, f32, f16], CCAssignToStack<8, 8>>" << std::endl;
    }
    os << "]>;" << std::endl;
}

void
TDGen::writeCallingConvLicenceText(std::ostream& os) {
    os << "//===- GenCallingConv.td - Calling Conventions TCE ---------*- "
       << "tablegen -*-===//" << std::endl
       << "// " << std::endl
       << "//                     The LLVM Compiler Infrastructure" << std::endl
       << "//" << std::endl
       << "// This file is distributed under the University of "
       << "Illinois Open Source" << std::endl
       << "// License. See LICENSE.TXT for details." << std::endl
       << "// " << std::endl
       << "//===--------------------------------------------------------"
       << "--------------===//" << std::endl
       << "//" << std::endl
       << "// This describes the calling conventions for the TCE "
       << "architectures." << std::endl
       << "//" << std::endl
       << "//===--------------------------------------------------------"
       << "--------------===//" << std::endl << std::endl;
}

void
TDGen::createGetMaxMemoryAlignment(std::ostream& os) const {
    if (mach_.is64bit()) {
        os << std::endl
           << "unsigned GeneratedTCEPlugin::getMaxMemoryAlignment() const {"
           << std::endl
           << "\treturn 8;"
           << std::endl << "}" << std::endl;
    } else {
        os << std::endl
           << "unsigned GeneratedTCEPlugin::getMaxMemoryAlignment() const {"
           << std::endl
           << "\treturn 4;"
           << std::endl << "}" << std::endl;
    }
}

void TDGen::createSelectPatterns(std::ostream& os) {
    os << "// Creating select patterns. " << std::endl;
    if (!hasSelect_) {
        os << "// Does not have select instr. " << std::endl;
        if (!hasConditionalMoves_) {
            if (mach_.is64bit()) {
            os << std::endl
               << "def : Pat<(i64 (select R1Regs:$c, R64Regs:$t, R64Regs:$f)), "
               << "(IOR64sss (AND64sss R32Regs:$t, (SUB64sas 0, (ANDext R1Regs:$c, 1))),"
               << "(AND64sss R32Regs:$f, (SUB64ssa (ANDext R1Regs:$c, 1), 1)))>;"
               << std::endl << std::endl

               << "def : Pat<(i64 (select R1Regs:$c, (i64 imm:$t),(i64 imm:$f))),"
               << "(IOR64sss (AND64ssa (SUB64sas 0, (ANDext R1Regs:$c, 1)), imm:$t),"
               << "(AND64ssa (SUB64ssa (ANDext R1Regs:$c, 1), 1), imm:$f))>;"
               << std::endl << std::endl

               << "def : Pat<(i64 (select R1Regs:$c, R64Regs:$t, (i64 imm:$f))),"
               << "(IOR64sss (AND64sss (SUB64sas 0, (ANDext R1Regs:$c, 1)), R32Regs:$t),"
               << "(AND64ssa (SUB64ssa (ANDext R1Regs:$c, 1), 1), imm:$f))>;"
               << std::endl << std::endl

               << "def : Pat<(i64 (select R1Regs:$c, (i64 imm:$t), R64Regs:$f)),"
               << "(IOR64sss (AND64ssa (SUB64sas 0, (ANDext R1Regs:$c, 1)), imm:$t),"
               << "(AND64sss (SUB64ssa (ANDext R1Regs:$c, 1), 1), R32Regs:$f))>;"
               << std::endl << std::endl;

             os << std::endl
               << "def : Pat<(i64 (select R1Regs:$c, R64Regs:$t, R64Regs:$f)), "
               << "(IOR64sss (AND64sss R64Regs:$t, (SUB64sas 0, (ANDext R1Regs:$c, 1))),"
               << "(AND64sss R64Regs:$f, (SUB64ssa (ANDext R1Regs:$c, 1), 1)))>;"
               << std::endl << std::endl

               << "def : Pat<(i64 (select R1Regs:$c, (i64 imm:$t),(i64 imm:$f))),"
               << "(IOR64sss (AND64ssa (SUB64sas 0, (ANDext R1Regs:$c, 1)), imm:$t),"
               << "(AND64ssa (SUB64ssa (ANDext R1Regs:$c, 1), 1), imm:$f))>;"
               << std::endl << std::endl

               << "def : Pat<(i64 (select R1Regs:$c, R64Regs:$t, (i64 imm:$f))),"
               << "(IOR64sss (AND64sss (SUB64sas 0, (ANDext R1Regs:$c, 1)), R64Regs:$t),"
               << "(AND64ssa (SUB64ssa (ANDext R1Regs:$c, 1), 1), imm:$f))>;"
               << std::endl << std::endl

               << "def : Pat<(i64 (select R1Regs:$c, (i64 imm:$t), R64Regs:$f)),"
               << "(IOR64sss (ANDssa (SUB64sas 0, (ANDext R1Regs:$c, 1)), imm:$t),"
               << "(ANDsss (SUBssa (ANDext R1Regs:$c, 1), 1), R64Regs:$f))>;"
               << std::endl << std::endl

               << "def : Pat<(i1 (select R1Regs:$c, R1Regs:$t, R1Regs:$f)),"
               << "(IOR64bbb (AND64bbb R1Regs:$c, R1Regs:$t), "
               << "(AND64bbb (XOR64bbj R1Regs:$c, 1), R1Regs:$f))>;"
               << std::endl << std::endl

               << "def : Pat<(i1 (select R1Regs:$c, (i1 0), R1Regs:$f)),"
               << "(AND64bbb (XOR64bbj R1Regs:$c, 1), R1Regs:$f)>;"
               << std::endl << std::endl

               << "def : Pat<(i1 (select R1Regs:$c, R1Regs:$t, (i1 -1))),"
               << "(IOR64bbb (AND64bbb R1Regs:$c, R1Regs:$t),"
               << "(XOR64bbj R1Regs:$c, 1))>;"
               << std::endl << std::endl;
            } else {
            os << std::endl
               << "def : Pat<(i32 (select R1Regs:$c, R32Regs:$t, R32Regs:$f)), " 
               << "(IORrrr (ANDrrr R32Regs:$t, (SUBrir 0, (ANDext R1Regs:$c, 1))),"
               << "(ANDrrr R32Regs:$f, (SUBrri (ANDext R1Regs:$c, 1), 1)))>;" 
               << std::endl << std::endl
                
               << "def : Pat<(i32 (select R1Regs:$c, (i32 imm:$t),(i32 imm:$f))),"
               << "(IORrrr (ANDrri (SUBrir 0, (ANDext R1Regs:$c, 1)), imm:$t),"
               << "(ANDrri (SUBrri (ANDext R1Regs:$c, 1), 1), imm:$f))>;" 
               << std::endl << std::endl
                
               << "def : Pat<(i32 (select R1Regs:$c, R32Regs:$t, (i32 imm:$f))),"
               << "(IORrrr (ANDrrr (SUBrir 0, (ANDext R1Regs:$c, 1)), R32Regs:$t),"
               << "(ANDrri (SUBrri (ANDext R1Regs:$c, 1), 1), imm:$f))>;"
               << std::endl << std::endl
                
               << "def : Pat<(i32 (select R1Regs:$c, (i32 imm:$t), R32Regs:$f)),"
               << "(IORrrr (ANDrri (SUBrir 0, (ANDext R1Regs:$c, 1)), imm:$t),"
               << "(ANDrrr (SUBrri (ANDext R1Regs:$c, 1), 1), R32Regs:$f))>;"
               << std::endl << std::endl
                
               << "def : Pat<(i1 (select R1Regs:$c, R1Regs:$t, R1Regs:$f)),"
               << "(IORbbb (ANDbbb R1Regs:$c, R1Regs:$t), " 
               << "(ANDbbb (XORbbj R1Regs:$c, 1), R1Regs:$f))>;" 
               << std::endl << std::endl
                
               << "def : Pat<(i1 (select R1Regs:$c, (i1 0), R1Regs:$f)),"
               << "(ANDbbb (XORbbj R1Regs:$c, 1), R1Regs:$f)>;" 
               << std::endl << std::endl
                
               << "def : Pat<(i1 (select R1Regs:$c, R1Regs:$t, (i1 -1))),"
               << "(IORbbb (ANDbbb R1Regs:$c, R1Regs:$t)," 
               << "(XORbbj R1Regs:$c, 1))>;"
               << std::endl << std::endl;
            
            os << "def : Pat<(f32 (select R1Regs:$c, FPRegs:$t,FPRegs:$f)),"
               << "(IORfff (ANDfff FPRegs:$t, (SUBfir 0, (ANDext R1Regs:$c, 1))),"
               << "(ANDfff FPRegs:$f, (SUBfri (ANDext R1Regs:$c,1),1)))>;"
               << std::endl << std::endl
                
               << "def : Pat<(f16 (select R1Regs:$c,HFPRegs:$t,HFPRegs:$f)),"
               << "(IORhhh (ANDhhh HFPRegs:$t, (SUBhir 0, (ANDext R1Regs:$c, 1))),"
               << "(ANDhhh HFPRegs:$f, (SUBhri (ANDext R1Regs:$c,1),1)))>;"
               << std::endl << std::endl;
            }
        } else {
            opNames_["SELECT_I1bb"] = "CMOV_SELECT";
            opNames_["SELECT_I1bj"] = "CMOV_SELECT";
            opNames_["SELECT_I1jb"] = "CMOV_SELECT";
            opNames_["SELECT_I1jj"] = "CMOV_SELECT";
            opNames_["SELECT_I32rr"] = "CMOV_SELECT";
            opNames_["SELECT_I32ir"] = "CMOV_SELECT";
            opNames_["SELECT_I32ri"] = "CMOV_SELECT";
            opNames_["SELECT_I32ii"] = "CMOV_SELECT";
            opNames_["SELECT_F32"] = "CMOV_SELECT";
            opNames_["SELECT_F16"] = "CMOV_SELECT";
            if (mach_.is64bit()) {
                opNames_["SELECT_I64rr"] = "CMOV_SELECT";
                opNames_["SELECT_I64ir"] = "CMOV_SELECT";
                opNames_["SELECT_I64ri"] = "CMOV_SELECT";
                opNames_["SELECT_I64ii"] = "CMOV_SELECT";
                opNames_["SELECT_F64"] = "CMOV_SELECT";

                os  << "def SELECT_I64rr : InstTCE<(outs R64IRegs:$dst),"
                    << "(ins R1Regs:$c, R64IRegs:$T, R64IRegs:$F),"
                    << "\"# SELECT_I64 PSEUDO!\","
                    << "[(set R64IRegs:$dst,"
                    << "(select R1Regs:$c, R64IRegs:$T, R64IRegs:$F))]>;"
                    << std::endl << std::endl

                    << "def : Pat<(i64 (select R64IRegs:$c, R64IRegs:$T, R64IRegs:$F)),"
                    << "(SELECT_I64rr (MOVI64I1ss R64Regs:$c),"
                    << "R64IRegs:$T, R64IRegs:$F)>;"
                    << std::endl << std::endl

                    << "def SELECT_I64ri : InstTCE<(outs R64IRegs:$dst),"
                    << "(ins R64IRegs:$c, R64IRegs:$T, i64imm:$F),"
                    << "\"# SELECT_I64 PSEUDO!\","
                    << "[(set R64IRegs:$dst,"
                    << "(select R64IRegs:$c, R64IRegs:$T, (i64 imm:$F)))]>;"
                    << std::endl << std::endl

                    << "def SELECT_I64ir : InstTCE<(outs R64IRegs:$dst),"
                    << "(ins R64IRegs:$c, i64imm:$T, R64IRegs:$F),"
                    << "\"# SELECT_I64 PSEUDO!\","
                    << "[(set R64IRegs:$dst,"
                    << "(select R64IRegs:$c, (i64 imm:$T), R64IRegs:$F))]>;"
                    << std::endl << std::endl

                    << "def SELECT_I64ii : InstTCE<(outs R64IRegs:$dst),"
                    << "(ins R64IRegs:$c, i64imm:$T, i64imm:$F),"
                    << "\"# SELECT_I64 PSEUDO!\","
                    << "[(set R64IRegs:$dst,"
                    << "(select R64IRegs:$c, (i64 imm:$T), (i64 imm:$F)))]>;"
                    << std::endl << std::endl

                    << "def SELECT_F64 : InstTCE<(outs R64FPRegs:$dst),"
                    << "(ins R1Regs:$c, R64FPRegs:$T, R64FPRegs:$F),"
                    << "\"# SELECT_F64 PSEUDO!\","
                    << "[(set R64FPRegs:$dst,"
                    << "(select R1Regs:$c, R64FPRegs:$T, R64FPRegs:$F))]>;"
                    << std::endl << std::endl;
            }

            os << "def SELECT_I1bb : InstTCE<(outs R1Regs:$dst),"
               << "(ins R1Regs:$c, R1Regs:$T, R1Regs:$F),"
               << "\"# SELECT_I1 PSEUDO!\","
               << " [(set R1Regs:$dst,"
               << "(select R1Regs:$c, R1Regs:$T, R1Regs:$F))]>;"
               << std::endl << std::endl
                
               << "def SELECT_I1bj : InstTCE<(outs R1Regs:$dst),"
               << " (ins R1Regs:$c, R1Regs:$T, i1imm:$F),"
               << "\"# SELECT_I1 PSEUDO!\","
               << "[(set R1Regs:$dst,"
               << "(select R1Regs:$c, R1Regs:$T, (i1 imm:$F)))]>;"
               << std::endl << std::endl
                
               << "def SELECT_I1jb : InstTCE<(outs R1Regs:$dst),"
               << "(ins R1Regs:$c, i1imm:$T, R1Regs:$F),"
               << "\"# SELECT_I1 PSEUDO!\","
               << "[(set R1Regs:$dst," 
               << "(select R1Regs:$c, (i1 imm:$T), R1Regs:$F))]>;"
               << std::endl << std::endl
                
               << "def SELECT_I1jj : InstTCE<(outs R1Regs:$dst),"
               << "(ins R1Regs:$c, i1imm:$T, i1imm:$F),"
               << "\"# SELECT_I1 PSEUDO!\","
               << "[(set R1Regs:$dst,"
               << "(select R1Regs:$c, (i1 imm:$T), (i1 imm:$F)))]>;"
               << std::endl << std::endl
                
               << "def SELECT_I32rr : InstTCE<(outs R32IRegs:$dst),"
               << "(ins R1Regs:$c, R32IRegs:$T, R32IRegs:$F),"
               << "\"# SELECT_I32 PSEUDO!\","
               << "[(set R32IRegs:$dst,"
               << "(select R1Regs:$c, R32IRegs:$T, R32IRegs:$F))]>;"
               << std::endl << std::endl
// select with the cond in an i32 (produced by expanded vselects with i32 cond vectors)
                
               << "def : Pat<(i32 (select R32IRegs:$c, R32IRegs:$T, R32IRegs:$F)),"
               << "(SELECT_I32rr (MOVI32I1rr R32Regs:$c),"
               << "R32IRegs:$T, R32IRegs:$F)>;" 
               << std::endl << std::endl
                
               << "def SELECT_I32ri : InstTCE<(outs R32IRegs:$dst),"
               << "(ins R32IRegs:$c, R32IRegs:$T, i32imm:$F),"
               << "\"# SELECT_I32 PSEUDO!\","
               << "[(set R32IRegs:$dst,"
               << "(select R32IRegs:$c, R32IRegs:$T, (i32 imm:$F)))]>;"
               << std::endl << std::endl
                
               << "def SELECT_I32ir : InstTCE<(outs R32IRegs:$dst),"
               << "(ins R32IRegs:$c, i32imm:$T, R32IRegs:$F),"
               << "\"# SELECT_I32 PSEUDO!\","
               << "[(set R32IRegs:$dst,"
               << "(select R32IRegs:$c, (i32 imm:$T), R32IRegs:$F))]>;"
               << std::endl << std::endl
                
               << "def SELECT_I32ii : InstTCE<(outs R32IRegs:$dst),"
               << "(ins R32IRegs:$c, i32imm:$T, i32imm:$F),"
               << "\"# SELECT_I32 PSEUDO!\","
               << "[(set R32IRegs:$dst,"
               << "(select R32IRegs:$c, (i32 imm:$T), (i32 imm:$F)))]>;"
               << std::endl << std::endl
                
               << "def SELECT_F32 : InstTCE<(outs FPRegs:$dst),"
               << "(ins R1Regs:$c, FPRegs:$T, FPRegs:$F),"
               << "\"# SELECT_F32 PSEUDO!\","
               << "[(set FPRegs:$dst,"
               << "(select R1Regs:$c, FPRegs:$T, FPRegs:$F))]>;"
               << std::endl << std::endl
                
               << "def SELECT_F16 : InstTCE<(outs HFPRegs:$dst),"
               << "(ins R1Regs:$c, HFPRegs:$T, HFPRegs:$F),"
               << "\"# SELECT_F16 PSEUDO!\","
               << "[(set HFPRegs:$dst, "
               << "(select R1Regs:$c, HFPRegs:$T, HFPRegs:$F))]>;"
               << std::endl << std::endl;
        }            
    } else {
        os << "// Has select instr!. " << std::endl;
    }
}

void TDGen::writeCallSeqStart(std::ostream& os) {

    bool is64bit = mach_.is64bit();
#ifdef LLVM_OLDER_THAN_5_0
    if (!is64bit) {
        os << "def SDT_TCECallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i32>]>;";
    } else {
        os << "def SDT_TCECallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i64>]>;";
    }
  os << std::endl << std::endl
     << "def callseq_start : SDNode<\"ISD::CALLSEQ_START\", "
     << "SDT_TCECallSeqStart, [SDNPHasChain, SDNPOutGlue]>;" << std::endl
     << std::endl
     << "let Defs = [SP], Uses = [SP] in {" << std::endl;
  if (!is64bit) {
      os << "def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt),";
  } else {
      os << "def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i64imm:$amt),";
  }
  os << "\"# ADJCALLSTACKDOWN $amt\","
     << "[(callseq_start timm:$amt)]>;}" << std::endl << std::endl;

#else

  if (!is64bit) {
      os << "def SDT_TCECallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i32>,"
         << "SDTCisVT<1, i32> ]>;" << std::endl << std::endl;
  } else {
      os << "def SDT_TCECallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i64>,"
         << "SDTCisVT<1, i64> ]>;" << std::endl << std::endl;
  }
  os << "def callseq_start : SDNode<\"ISD::CALLSEQ_START\", "
     << "SDT_TCECallSeqStart, [SDNPHasChain, SDNPOutGlue]>;" << std::endl
     << std::endl
     << "let Defs = [SP], Uses = [SP] in {" << std::endl
     << "def ADJCALLSTACKDOWN : Pseudo<(outs),";
  if (!is64bit) {
      os << "(ins i32imm:$amt1, i32imm:$amt2),";
  } else {
      os << "(ins i64imm:$amt1, i64imm:$amt2),";
  }
  os << "\"# ADJCALLSTACKDOWN $amt1, $amt2\","
     << "[(callseq_start timm:$amt1, timm:$amt2)]>;}"
     << std::endl << std::endl;

#endif
}

void TDGen::write64bitMoveDefs(std::ostream& o) {
    o << std::endl << "// 64-bit register->register move definitions."
      << std::endl << "let isAsCheapAsAMove = 1 in {" << std::endl;

    o << "def MOV64rr : InstTCE<(outs R64Regs:$dst), (ins R64Regs:$src),"
      << "	           \"$src -> $dst;\", []>;" << std::endl;

    o << "def PRED_TRUE_MOV64rr : InstTCE<(outs R64Regs:$dst), "
      << "(ins R1Regs:$pred, R64Regs:$src), \"$src -> $dst;\", []>;"
      << std::endl;

    o << "def PRED_FALSE_MOV64rr : InstTCE<(outs R64Regs:$dst), "
      << "(ins R1Regs:$pred, R64Regs:$src), \"$src -> $dst;\", []>;"
      << std::endl;

    o << "} // end of is as cheap as move" << std::endl;
}

void TDGen::genTCEInstrInfo_copyPhys64bitReg(std::ostream&o) const {
    o << std::endl
      << "#include <llvm/CodeGen/MachineInstrBuilder.h>" << std::endl
      << "// copies 64-bit reg to a another" << std::endl
      << "bool TCEInstrInfoSIMD::copyPhys64bitReg(" << std::endl
      << "\tMachineBasicBlock& mbb," << std::endl
      << "\tMachineBasicBlock::iterator mbbi," << std::endl
#ifdef LLVM_OLDER_THAN_3_9
      <<  "DebugLoc dl," << std::endl
#else
      <<  "const DebugLoc& dl," << std::endl
#endif
      << "\tunsigned destReg, unsigned srcReg," << std::endl
      << "\tbool killSrc) const {" << std::endl
      << std::endl;

    if (mach_.is64bit()) {
        o << "\tif (TCE::R64RegsRegClass.contains(destReg, srcReg)) {\n"
          << "\t\tBuildMI(mbb, mbbi, dl, get(TCE::MOV64rr), destReg)\n"
          << "\t\t\t.addReg(srcReg, getKillRegState(killSrc));" << std::endl
          << "\t\treturn true;" << std::endl
          << "}" << std::endl;
    }
    o << "\treturn false;" << std::endl
      << "}";
}

void TDGen::writeAddressingModeDefs(std::ostream& o) {
    if (!mach_.is64bit()) {
        o << std::endl
          << "// Addressing modes." << std::endl
          << "def ADDRrr : ComplexPattern<i32, 2, \"SelectADDRrr\", [], []>;" << std::endl
          << "def ADDRri : ComplexPattern<i32, 2, \"SelectADDRri\", [frameindex], []>;" << std::endl
          << std::endl
          << "// Address operands" << std::endl
          << "def MEMrr : Operand<i32> {" << std::endl
          << "let PrintMethod = \"printMemOperand\";" << std::endl
          << "let MIOperandInfo = (ops R32IRegs, R32IRegs);" << std::endl
          << "}" << std::endl
          << "def MEMri : Operand<i32> {" << std::endl
          << "let PrintMethod = \"printMemOperand\";" << std::endl
          << "let MIOperandInfo = (ops R32IRegs, i32imm);" << std::endl
          << "}" << std::endl
          << std::endl
          << "// Branch targets have OtherVT type." << std::endl
          << "def brtarget : Operand<OtherVT>; " << std::endl
          << "def calltarget : Operand<i32>;" << std::endl;

        o << "def SDT_TCECall    : SDTypeProfile<0, 1, [SDTCisVT<0, i32>]>;" << std::endl;
    } else {
        o << std::endl
          << "// Addressing modes." << std::endl
          << "def ADDRrr : ComplexPattern<i64, 2, \"SelectADDRrr\", [], []>;" << std::endl
          << "def ADDRri : ComplexPattern<i64, 2, \"SelectADDRri\", [frameindex], []>;" << std::endl
          << std::endl
          << "// Address operands" << std::endl
          << "def MEMrr : Operand<i64> {" << std::endl
          << "let PrintMethod = \"printMemOperand\";" << std::endl
          << "let MIOperandInfo = (ops R64IRegs, R64IRegs);" << std::endl
          << "}" << std::endl
          << "def MEMri : Operand<i64> {" << std::endl
          << "let PrintMethod = \"printMemOperand\";" << std::endl
          << "let MIOperandInfo = (ops R64IRegs, i64imm);" << std::endl
          << "}" << std::endl
          << std::endl
          << "// Branch targets have OtherVT type." << std::endl
          << "def brtarget : Operand<OtherVT>; " << std::endl
          << "def calltarget : Operand<i64>;" << std::endl;

        o << "def SDT_TCECall    : SDTypeProfile<0, 1, [SDTCisVT<0, i64>]>;" << std::endl;
    }
}

void TDGen::writeMiscPatterns(std::ostream& o) {
    if (!mach_.is64bit()) {
        o << "// zero extending moves used in some patterns" << std::endl
          << "def ANDext : InstTCE<(outs R32IRegs:$dst), (ins R1Regs:$src, i32imm:$val), \"\", []>;" << std::endl
          << "def PRED_TRUE_ANDext : InstTCE<(outs R32IRegs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i32imm:$val), \"\", []>;" << std::endl
          << "def PRED_FALSE_ANDext : InstTCE<(outs R32IRegs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i32imm:$val),\"\", []>;" << std::endl
          << "def XORbicmp: InstTCE<(outs R1Regs:$dst),"
          << " (ins R1Regs:$src, i32imm:$val), \"\", []>;" << std::endl
          << "def PRED_TRUE_XORbicmp: InstTCE<(outs R1Regs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i32imm:$val), \"\", []>;" << std::endl
          << "def PRED_FALSE_XORbicmp: InstTCE<(outs R1Regs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i32imm:$val), \"\", []>;" << std::endl;

        o << "def: Pat <(i32 (anyext R1Regs:$src)), (ANDext R1Regs:$src, 1)>;" << std::endl
          << "def: Pat <(i32 (zext R1Regs:$src)), (ANDext R1Regs:$src, 1)>;" << std::endl;

        o << "// select of 1 or 0." << std::endl
          << "def : Pat<(i32 (select R1Regs:$c, (i32 1), (i32 0))),"
          << " (ANDext R1Regs:$c, 1)>;" << std::endl;

        o << std::endl
          << "def: Pat <(i32 (sext R1Regs:$src)), (SUBrir 0,(ANDext R1Regs:$src, 1))>;"
          << std::endl;

        o << "// ------ Shift (emulation) patterns. " << std::endl
          << "def: Pat <(i32 (shl R32IRegs:$val, (i32 1))),"
          << " (ADDrrr R32Regs:$val, R32Regs:$val)>;" << std::endl
          << "def: Pat <(i32 (TCESHLConst R32IRegs:$val, (i32 1))),"
          << " (ADDrrr R32IRegs:$val, R32IRegs:$val)>;" << std::endl;

        o << "// ----- Global addresses, constant pool entries ------" << std::endl
          << "def TCEGlobalAddr : SDNode<\"TCEISD::GLOBAL_ADDR\", SDTIntUnaryOp>;" << std::endl
          << "def TCEConstPool : SDNode<\"TCEISD::CONST_POOL\", SDTIntUnaryOp>;" << std::endl
          << "def : Pat<(TCEGlobalAddr tglobaladdr:$in), (MOVI32ri tglobaladdr:$in)>;" << std::endl
          << "def : Pat<(TCEGlobalAddr tconstpool:$in), (MOVI32ri tconstpool:$in)>;" << std::endl
          << "def : Pat<(TCEConstPool tglobaladdr:$in), (MOVI32ri tglobaladdr:$in)>;" << std::endl
          << "def : Pat<(TCEConstPool tconstpool:$in), (MOVI32ri tconstpool:$in)>;" << std::endl;


        o << "// some peephole patterns." << std::endl
          << "// 1-bit select with imm values - xor or mov." << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, (i1 0), (i1 -1))), (XORbbj R1Regs:$c, 1)>;" << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, (i1 -1), (i1 0))), (MOVI1rr R1Regs:$c)>;" << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, (i1 -1), R1Regs:$F)), (IORbbb R1Regs:$c, R1Regs:$F)>;" << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, R1Regs:$T, (i1 0))), (ANDbbb R1Regs:$c, R1Regs:$T)>;" << std::endl;

        o <<  "// 1-bit comparison between booleans - xor or xnor(implemented with 2 xors)" << std::endl
          << "def : Pat<(i1 (setne R1Regs:$op1, R1Regs:$op2)), (XORbbb R1Regs:$op1, R1Regs:$op2)>;" << std::endl
          << "// TODO: should the temp values be converted to i32? usually more i32 regs." << std::endl
          << "def : Pat<(i1 (seteq R1Regs:$op1, R1Regs:$op2)), (XORbbj (XORbbb R1Regs:$op1, R1Regs:$op2), 1)>;" << std::endl;

        o << "def TCEBlockAddress : SDNode<\"TCEISD::BLOCK_ADDR\", SDTIntUnaryOp>;" << std::endl
          << "def : Pat<(TCEBlockAddress tblockaddress:$src1), (MOVI32ri tblockaddress:$src1)>;" << std::endl;

    } else {
        o << "// zero extending moves used in some patterns" << std::endl
          << "def ANDext : InstTCE<(outs R64IRegs:$dst),"
          << " (ins R1Regs:$src, i64imm:$val), \"\", []>;" << std::endl
          << "def PRED_TRUE_ANDext : InstTCE<(outs R64IRegs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i64imm:$val), \"\", []>;" << std::endl
          << "def PRED_FALSE_ANDext : InstTCE<(outs R64IRegs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i64imm:$val),\"\", []>;" << std::endl
          << "def XORbicmp: InstTCE<(outs R1Regs:$dst),"
          << " (ins R1Regs:$src, i64imm:$val), \"\", []>;" << std::endl
          << "def PRED_TRUE_XORbicmp: InstTCE<(outs R1Regs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i64imm:$val), \"\", []>;" << std::endl
          << "def PRED_FALSE_XORbicmp: InstTCE<(outs R1Regs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i64imm:$val), \"\", []>;" << std::endl;

        o << "def: Pat <(i64 (anyext R1Regs:$src)), (ANDext R1Regs:$src, 1)>;" << std::endl
          << "def: Pat <(i64 (zext R1Regs:$src)), (ANDext R1Regs:$src, 1)>;" << std::endl;

        o << "// select of 1 or 0." << std::endl
          << "def : Pat<(i64 (select R1Regs:$c, (i64 1), (i64 0))), (ANDext R1Regs:$c, 1)>;" << std::endl;

        o << std::endl
          << "def: Pat <(i64 (sext R1Regs:$src)), (SUB64sas 0,(ANDext R1Regs:$src, 1))>;"
          << std::endl;

        o << "// ------ Shift (emulation) patterns. " << std::endl
          << "def: Pat <(i64 (shl R64IRegs:$val, (i64 1))),"
          << " (ADD64sss R64Regs:$val, R64Regs:$val)>;" << std::endl
          << "def: Pat <(i64 (TCESHLConst R64IRegs:$val, (i64 1))),"
          << " (ADD64sss R64IRegs:$val, R64IRegs:$val)>;" << std::endl;

        o << "// ----- Global addresses, constant pool entries ------" << std::endl
          << "def TCEGlobalAddr : SDNode<\"TCEISD::GLOBAL_ADDR\", SDTIntUnaryOp>;" << std::endl
          << "def TCEConstPool : SDNode<\"TCEISD::CONST_POOL\", SDTIntUnaryOp>;" << std::endl
          << "def : Pat<(TCEGlobalAddr tglobaladdr:$in), (MOVI64sa tglobaladdr:$in)>;" << std::endl
          << "def : Pat<(TCEGlobalAddr tconstpool:$in), (MOVI64sa tconstpool:$in)>;" << std::endl
          << "def : Pat<(TCEConstPool tglobaladdr:$in), (MOVI64sa tglobaladdr:$in)>;" << std::endl
          << "def : Pat<(TCEConstPool tconstpool:$in), (MOVI64sa tconstpool:$in)>;" << std::endl;

        o << "// some peephole patterns." << std::endl
          << "// 1-bit select with imm values - xor or mov." << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, (i1 0), (i1 -1))), (XOR64bbj R1Regs:$c, 1)>;" << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, (i1 -1), (i1 0))), (MOVI1rr R1Regs:$c)>;" << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, (i1 -1), R1Regs:$F)), (IOR64bbb R1Regs:$c, R1Regs:$F)>;" << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, R1Regs:$T, (i1 0))), (AND64bbb R1Regs:$c, R1Regs:$T)>;" << std::endl;

        o <<  "// 1-bit comparison between booleans - xor or xnor(implemented with 2 xors)" << std::endl
          << "def : Pat<(i1 (setne R1Regs:$op1, R1Regs:$op2)), (XOR64bbb R1Regs:$op1, R1Regs:$op2)>;" << std::endl
          << "// TODO: should the temp values be converted to i64? usually more i64 regs." << std::endl
          << "def : Pat<(i1 (seteq R1Regs:$op1, R1Regs:$op2)), (XOR64bbj (XOR64bbb R1Regs:$op1, R1Regs:$op2), 1)>;" << std::endl;

        o << "def TCEBlockAddress : SDNode<\"TCEISD::BLOCK_ADDR\", SDTIntUnaryOp>;" << std::endl
          << "def : Pat<(TCEBlockAddress tblockaddress:$src1), (MOVI64sa tblockaddress:$src1)>;" << std::endl;
    }
}

void TDGen::writeConstShiftPat(std::ostream& os,
                               const TCEString& nodeName,
                               const TCEString& opNameBase, int i) {

    if (!mach_.is64bit()) {
        TCEString opName = opNameBase; opName << i << "_32rr";
        if (opNames_.find(opName) != opNames_.end()) {
            os << "def : Pat<(i32 (" << nodeName
               << " R32IRegs:$val, (i32 " << i << "))), ("
               << opName << " R32IRegs:$val)>;" << std::endl;
        }
    } else {
        TCEString opName = opNameBase; opName << i << "_64rr";
        if (opNames_.find(opName) != opNames_.end()) {
            os << "def : Pat<(i64 (" << nodeName
               << " R64IRegs:$val, (i64 " << i << "))), ("
               << opName << " R64IRegs:$val)>;" << std::endl;
        }
    }
}


void TDGen::createConstShiftPatterns(std::ostream& os) {
    int bits = mach_.is64bit() ? 64: 32;
    for (int i = 1; i < bits; i++) {
        writeConstShiftPat(os, "TCESRAConst", "SHR", i);
        writeConstShiftPat(os, "TCESRLConst", "SHRU", i);
        writeConstShiftPat(os, "TCESHLConst", "SHL", i);
    }
}

void TDGen::createBoolAndHalfLoadPatterns(std::ostream& os) {

    // TODO: what about true/false versions of these ops?

    TCEString load = littleEndian_ ? "LD8" : "LDQ";
    TCEString uload = littleEndian_ ? "LDU8" : "LDQU";
    TCEString wload = littleEndian_ ?
        (mach_.is64bit() ? "LD64" : "LD32") : "LDW";
    if (mach_.hasOperation(load)) {
        os << "def " << load
           << "Br : InstTCE<(outs R1Regs:$op2), (ins MEMrr:$op1), \"\", "
           << "[(set R1Regs:$op2, (sextloadi1 ADDRrr:$op1))]>;" << std::endl
           << "def " << load
           << "Bi : InstTCE<(outs R1Regs:$op2), (ins MEMri:$op1), \"\", "
           << "[(set R1Regs:$op2, (sextloadi1 ADDRri:$op1))]>; " << std::endl;

        opNames_[load + "Br"] = load;
        opNames_[load + "Bi"] = load;

    }
    if (mach_.hasOperation(uload)) {
        os << "def " << uload
           << "Br : InstTCE<(outs R1Regs:$op2), (ins MEMrr:$op1), \"\", "
           << "[(set R1Regs:$op2, (zextloadi1 ADDRrr:$op1))]>;" << std::endl
           << "def " << uload
           << "Bi : InstTCE<(outs R1Regs:$op2), (ins MEMri:$op1), \"\", "
           << "[(set R1Regs:$op2, (zextloadi1 ADDRri:$op1))]>;" << std::endl;
        opNames_[uload + "Br"] = uload;
        opNames_[uload + "Bi"] = uload;

        os << "def : Pat<(i1 (load ADDRrr:$addr)), ("
           << uload << "Br ADDRrr:$addr)>;" << std::endl;
        os << "def : Pat<(i1 (load ADDRri:$addr)), ("
           << uload << "Bi ADDRri:$addr)>;" << std::endl;
    } else {
        if (mach_.hasOperation(load)) {
            os << "def : Pat<(i1 (load ADDRrr:$addr)), ("
               << load << "Br ADDRrr:$addr)>;" << std::endl;
            os << "def : Pat<(i1 (load ADDRri:$addr)), ("
               << load << "Bi ADDRri:$addr)>;" << std::endl;
        }
    }

    // if no 8-bit loads, create 32/64-bit loads for stack access but
    // no patterns for isel as only the stack is 32/64-bit aligned.
    // 1- and 8-bit loads on isel will be handled by lowering.
    if (!mach_.hasOperation(load) &&
        !mach_.hasOperation(uload) &&
        mach_.hasOperation(wload)) {
        os << "def " << wload
           << "Br : InstTCE<(outs R1Regs:$op2), (ins MEMrr:$op1), \"\", "
           << "[]>;" << std::endl
        << "def " << wload
        << "Bi : InstTCE<(outs R1Regs:$op2), (ins MEMri:$op1), \"\", "
        << "[]>;" << std::endl;

        opNames_[wload + "Br"] = wload;
        opNames_[wload + "Bi"] = wload;
    }

    TCEString halfLoad = littleEndian_ ? "LD16" : "LDH";
    if (!mach_.hasOperation(halfLoad)) {
        TCEString halfULoad = littleEndian_ ? "LDU16" : "LDHU";
        if (mach_.hasOperation(halfULoad)) {
            halfLoad = halfULoad;
        } else {
            return;
        }
    }

    os << "def " << halfLoad << "hr : InstTCE<(outs HFPRegs:$op2), "
       << "(ins MEMrr:$op1), \"\", [(set HFPRegs:$op2, "
       << "(load ADDRrr:$op1))]>;" << std::endl;

    os << "def " << halfLoad << "hi : InstTCE<(outs HFPRegs:$op2), "
       << "(ins MEMri:$op1), \"\", [(set HFPRegs:$op2, "
       << "(load ADDRri:$op1))]>;" << std::endl;

    opNames_[halfLoad + "hr"] = halfLoad;
    opNames_[halfLoad + "hi"] = halfLoad;

    // TODO: what about 32-bit?
}
