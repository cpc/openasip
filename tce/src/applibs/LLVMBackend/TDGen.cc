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
unsigned const TDGen::REQUIRED_I32_REGS = 5;


/**
 * Constructor.
 *
 * @param mach Machine to generate plugin for.
 */
TDGen::TDGen(const TTAMachine::Machine& mach) :
    mach_(mach), dregNum_(0), maxVectorSize_(1), 
    highestLaneInt_(-1), highestLaneBool_(-1),
    hasExBoolRegs_(false), hasExIntRegs_(false), hasSelect_(false),
    littleEndian_(mach.isLittleEndian()) {
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
    if (reg.rf.find("EX_") ==0) {
        templ += "_Ex";
        regsInClasses_[templ].push_back(regName);
    } else if (reg.rf.find("L_") == 0) {
        templ += "_" + reg.rf.substr(0,3);
        regsInClasses_[templ].push_back(regName);

    }

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
    analyzeRegisterFileClasses();

    analyzeRegisters();

    if (!checkRequiredRegisters()) {
        return false;
    }

    assert(regs32bit_.size() >= REQUIRED_I32_REGS);

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

    o << "class TCEVectorReg<string n, list<Register> subregs> : "
      << "RegisterWithSubRegs<n, subregs> {"
      << std::endl
      << "    let Namespace = \"TCE\";" << std::endl
      << "}" << std::endl;

  // Subregister indices.
    o << "def lane0 : SubRegIndex<32>;" << std::endl 
      << "def lane1 : SubRegIndex<32>;" << std::endl
      << "def lane2 : SubRegIndex<32>;" << std::endl
      << "def lane3 : SubRegIndex<32>;" << std::endl
      << "def lane4 : SubRegIndex<32>;" << std::endl
      << "def lane5 : SubRegIndex<32>;" << std::endl
      << "def lane6 : SubRegIndex<32>;" << std::endl
      << "def lane7 : SubRegIndex<32>;" << std::endl << std::endl
        
      << "def subvector2_0 : SubRegIndex<64>;" << std::endl
      << "def subvector2_2 : SubRegIndex<64>;" << std::endl
      << "def subvector2_4 : SubRegIndex<64>;" << std::endl
      << "def subvector2_6 : SubRegIndex<64>;" << std::endl
      << "def subvector4_0 : SubRegIndex<128>;" << std::endl
      << "def subvector4_4 : SubRegIndex<128>;" << std::endl << std::endl;

    writeRegisterClasses(o);
   
    writeRARegisterInfo(o);
    write1bitRegisterInfo(o);
    //write16bitRegisterInfo(o);
    write32bitRegisterInfo(o);
    writeVectorRegisterInfo(o);

    return true;
}

void
TDGen::writeRegisterClasses(std::ostream& o) {

    o << "class R1<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << "}" << std::endl;

    if (hasExBoolRegs_) {
        o << "class R1_Ex<string n, list<Register> aliases> : R1<n, aliases> {"
          << "}" << std::endl;
    }
    
    o << "class R32<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << "}" << std::endl;
    if (hasExIntRegs_) {
        o << "class R32_Ex<string n, list<Register> aliases> : R32<n, aliases> {}"
          << std::endl;
    }
    
    o << "class R16<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << "}" << std::endl;
    if (hasExIntRegs_) {
        o << "class R16_Ex<string n, list<Register> aliases> : R16<n, aliases> {}"
          << std::endl;
    }

    for (int i = 0; i <= highestLaneInt_; i++) {
        o << "class R32_L_" << i << "<string n, list<Register> aliases> : R32<n, aliases>{}" << std::endl;
    }

    for (int i = 0; i <= highestLaneBool_; i++) {
        o << "class R1_L_" << i << "<string n, list<Register> aliases> : R1<n, aliases>{}" << std::endl;
    }

    o << "class V2R32<string n, list<Register> aliases> : TCEVectorReg<n, aliases> {"
      << "}" << std::endl;

    o << "class V4R32<string n, list<Register> aliases> : TCEVectorReg<n, aliases> {"
      << "}" << std::endl;

    o << "class V8R32<string n, list<Register> aliases> : TCEVectorReg<n, aliases> {"
      << "}" << std::endl;

    o << std::endl;

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

    analyzeRegisters(ONLY_EXTRAS);
    analyzeRegisters(ONLY_NORMAL);
    analyzeRegisters(ONLY_LANES);
}

void 
TDGen::analyzeRegisterFileClasses() {

    const TTAMachine::Machine::RegisterFileNavigator nav =
        mach_.registerFileNavigator();
    
    for (int i = 0; i < nav.count(); i++) {
        const TTAMachine::RegisterFile* rf = nav.item(i);
        unsigned width = rf->width();
		if (rf->name().find("EX_") == 0) {
            switch (width) {
            case 1:
                hasExBoolRegs_ = true;
                break;
            case 32:
                hasExIntRegs_ = true;
                break;
            default:
                break;
                // other sizes not supported yet?
            }
        }
		if (rf->name().find("L_") == 0) {
            // only works up to 8 first;
            int lane = rf->name()[2] - 48;
            switch (width) {
            case 1:
                if (lane < 8 && lane > highestLaneBool_) {
                    highestLaneBool_ = lane;
                }
                /* fall through */
            case 32:
                if (lane < 8 && lane > highestLaneInt_) {
                    highestLaneInt_ = lane;
                }
            default:
                break;
                // other sizes not supported yet?
            }
        }
    }
}


/**
 * Iterates through some register files.
 *
 * @param regsToProcess which clusters to analyze
 */
void TDGen::analyzeRegisters(RegsToProcess regsToProcess) {

    const TTAMachine::Machine::RegisterFileNavigator nav =
        mach_.registerFileNavigator();

    bool regsFound = true;
    int currentIdx = 0;
    while (regsFound) {
        regsFound = false;
        for (int i = 0; i < nav.count(); i++) {
            const TTAMachine::RegisterFile* rf = nav.item(i);

	    if (regsToProcess != ALL_REGISTERS) {
		if (rf->name().find("EX_") == 0) {
		    if (regsToProcess != ONLY_EXTRAS) {
			continue;
		    }
		} else if (rf->name().find("L_") == 0) {
		    if (regsToProcess != ONLY_LANES) {
			continue;
		    }
		} else {
		    if (regsToProcess != ONLY_NORMAL) {
			continue;
		    }
		}
	    }
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

    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 1-bit RF classes
        if (ri->first.find("R1") == 0) {

            o << std::endl
              << "def " << ri->first << "Regs : RegisterClass<\"TCE\", [i1], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")> {" << std::endl
              << " let Size=32;" << std::endl
              << "}" << std::endl;
        }
    }
}

/**
 * Writes 32-bit register definitions to the output stream.
 */
void
TDGen::write32bitRegisterInfo(std::ostream& o) {

    // --- Hardcoded reserved registers. ---
    writeRegisterDef(o, regs32bit_[0], "SP", "R32", "", RESERVED);
    writeRegisterDef(o, regs32bit_[1], "IRES0", "R32", "", RESULT);
    writeRegisterDef(o, regs32bit_[2], "FP", "R32", "", RESERVED);
    writeRegisterDef(
        o, regs32bit_[3], "KLUDGE_REGISTER", "R32", "", RESERVED);
    // -------------------------------------
    
    for (unsigned i = 4; i < regs32bit_.size(); i++) {
        std::string regName = "I" + Conversion::toString(i);
        writeRegisterDef(o, regs32bit_[i], regName, "R32", "", GPR);
        
        if (regsInRFClasses_.find(regs32bit_[i].rf) == regsInRFClasses_.end()) {
            regsInRFClasses_[regs32bit_[i].rf] = std::vector<std::string>();
        }
        
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

    if (regs64bit_.size() < 1) {
        RegInfo reg = { "dummy64", 0 };
        writeRegisterDef(o, reg, "DIRES0", "Ri64", "", RESERVED);
        i64regs = "DIRES0";
    } else {
  
      writeRegisterDef(o, regs64bit_[0], "DIRES0", "Ri64", "", RESERVED);
        for (unsigned i = 1; i < regs64bit_.size(); i++) {
            std::string regName = "DI" + Conversion::toString(i);
            i64regs += regName;
            i64regs += ", ";
            writeRegisterDef(o, regs64bit_[i], regName, "Ri64", "", GPR);
        }
        i64regs += "DIRES0";
    }

    o << std::endl
      << "def I64Regs : RegisterClass<\"TCE\", [i64], 32, (add " // DIRES
      << i64regs << ")> ;"
      << std::endl;

    std::string f64regs;

    if (regs64bit_.size() < 1) {
        RegInfo reg = { "dummy64", 0 };
        writeRegisterDef(o, reg, "DRES0", "Rf64", "", RESERVED);
        f64regs = "DRES0";
    } else {
        writeRegisterDef(
            o, regs64bit_[0], "DRES0", "Ri64", "DIRES0", RESERVED);
        for (unsigned i = 1; i < regs64bit_.size(); i++) {
            std::string regName = "D" + Conversion::toString(i);
            std::string aliasName = "DI" + Conversion::toString(i);
            f64regs += regName;
            f64regs += ", ";
            writeRegisterDef(
                o, regs64bit_[i], regName, "Rf64", aliasName, GPR);
        }
        f64regs += "DRES0";
    }
    o << std::endl
      << "def F64Regs : RegisterClass<\"TCE\", [f64], 32, (add "
      << f64regs << ")>;" << std::endl;
}



void
TDGen::writeVectorRegisterInfo(
    std::ostream& o, int vectorWidth) {

    std::string vectorRegsAll;
    std::vector<std::string> vectorRegs(8);

    if (vectorWidth <= maxVectorSize_) {
        for (unsigned i = 3; i < regs32bit_.size(); i++) {
            if (regs32bit_[i].rf.find("L_") == 0) {
                bool ok = true;
                unsigned int regIndex = regs32bit_[i].idx;
                std::vector<RegInfo> subRegs;
                TCEString lane0RfName = regs32bit_[i].rf;
                TCEString vecRegRfName = 
                    "_VECTOR_" + Conversion::toString(vectorWidth) + "_" + 
                    lane0RfName;

                int firstLaneIndex = regs32bit_[i].rf[2] - 48;
                if (firstLaneIndex % vectorWidth) {
                    continue;
                }

                TCEString subRegName;
                if (vectorWidth == 2) {
                    subRegName = "I" + Conversion::toString(i);
                } else {
                    subRegName = "_VEC32_";
                    subRegName << (vectorWidth >> 1) << "_" << i;
                }
                // TODO: this should be able to skip RFs
                
                TCEString curRfName;
                int j = 0;
                int laneIndex = 1;
                while (laneIndex < vectorWidth) {
                    j++;
                    TCEString laneRfName = lane0RfName;
                    // rf name should be same as first but
                    // different lane number name
                    laneRfName[2] += laneIndex;
                    // reached last register?
                    if (i+j >= regs32bit_.size()) {
                        ok = false;
                        break;
                    }                        
                    curRfName = regs32bit_[i+j].rf;

                    // not a lane rf?
                    RegInfo& gprRegInfo = regs32bit_[i+j];
                    if (gprRegInfo.idx != regIndex) {
                        ok = false;
                        break;
                    } else {
                        // not correct RF, try next..
                        if (gprRegInfo.rf != laneRfName) {
                            continue;
                        }

                        // now we have a vector rf from correct lane?
                        // create the subreg information
                        if (vectorWidth == 2) {
                            TCEString aliasIndex = Conversion::toString(i+j);
                            subRegName+= ", I";
                            subRegName+= aliasIndex;
                        } else {
                            
                            if (laneIndex  == vectorWidth >> 1) {
                                subRegName << " , _VEC32_"
                                           << (vectorWidth >> 1) << "_" << i + j;
                            }
                        }
                        vecRegRfName += "+" + gprRegInfo.rf;
                        laneIndex++;
                    }
                }
                if (laneIndex != vectorWidth) {
                    ok = false;
                }
                if (ok) {
                    TCEString regName = "_VEC32_";
                    regName << vectorWidth << "_" << Conversion::toString(i);

                    if (vectorRegsAll != "") {
                        vectorRegsAll += ", " + regName;
                    } else {
                        vectorRegsAll = regName;
                    }

                    if (vectorRegs[firstLaneIndex] == "") {
                        vectorRegs[firstLaneIndex] = regName;
                    } else {
                        vectorRegs[firstLaneIndex] += ", " + regName;
                    }

                    RegInfo vecRegInfo = { vecRegRfName, regIndex };
                    o << "let SubRegIndices = [";
                    if (vectorWidth == 2) {
                        o << "lane" << firstLaneIndex << ", lane" << (firstLaneIndex+1);
                    } else {
                        o << "subvector" << (vectorWidth >> 1) << "_"
                          <<  firstLaneIndex << " , "
                          << "subvector" << (vectorWidth >> 1) << "_"
                          << firstLaneIndex + (vectorWidth>>1);
                    }
                    o << "], CoveredBySubRegs = 1 in {" << std::endl;

                    writeRegisterDef(
                        o, vecRegInfo, regName, 
                        TCEString("V") + Conversion::toString(vectorWidth)
                        + "R32", subRegName, GPR);

                    o << "}" << std::endl;
                }
            }
        }
    }
    TCEString regClassBase("V"); regClassBase << vectorWidth << "R32";

    bool hasAllVectorRegs = true;
    for (int i = 0; i < 8; i += vectorWidth) {
        if (vectorRegs[i] == "") {
            hasAllVectorRegs = false;
        }
    }

    // only low lane ones are really used for data (ie. first element in lane 0)
    if (!hasAllVectorRegs) {
        RegInfo reg = {TCEString("dummyvec") + Conversion::toString(vectorWidth), 0};
        TCEString nameI("V"); nameI << vectorWidth << "R32DUMMY" ;
        writeRegisterDef(o, reg, nameI, regClassBase, "", RESERVED);
    }

    for (int i = 0; i < 8; i += vectorWidth) {
        if (vectorRegs[i] == "") {
            o << "def " << regClassBase << "_L_" << i
              << "Regs : RegisterClass<\"TCE\", [v" << vectorWidth
              << "i32, v" << vectorWidth << "f32], " << 32 * vectorWidth
              << ", (add V" << vectorWidth << "R32DUMMY)> ;" << std::endl;
            
            o << "def " << regClassBase << "_L_" << i
              << "IRegs : RegisterClass<\"TCE\", [v" << vectorWidth
              << "i32], " << 32 * vectorWidth  << ", (add V" << vectorWidth << "R32DUMMY)> ;"
              << std::endl;

            o << "def " << regClassBase << "_L_" << i
              << "FPRegs : RegisterClass<\"TCE\", [v" << vectorWidth
              << "f32], " << 32 * vectorWidth  << ", (add V" << vectorWidth << "R32DUMMY)> ;"
              << std::endl << std::endl;

            /*o << "def " << regClassBase << "_L_" << i
              << "HFPRegs : RegisterClass<\"TCE\", [v" << vectorWidth
              << "f16], " << 32 * vectorWidth  << ", (add V" << vectorWidth << "R32DUMMY)> ;"
              << std::endl << std::endl;*/
        } else {
            o << "def " << regClassBase << "_L_" << i
              << "Regs : RegisterClass<\"TCE\", [v" << vectorWidth
              << "i32, v" << vectorWidth << "f32], " << 32 * vectorWidth
              << ", (add " << vectorRegs[i] << ")>;" << std::endl;

            o << "def " << regClassBase << "_L_" << i
              << "IRegs : RegisterClass<\"TCE\", [v" << vectorWidth
              << "i32], " << 32 * vectorWidth << ", (add " << vectorRegs[i] << ")>;"
              << std::endl;

            o << "def " << regClassBase << "_L_" << i
              << "FPRegs : RegisterClass<\"TCE\", [v" << vectorWidth
              << "f32], " << 32 * vectorWidth << ", (add " << vectorRegs[i] << ")>;"
              << std::endl << std::endl;

            /*o << "def " << regClassBase << "_L_" << i
              << "HFPRegs : RegisterClass<\"TCE\", [v" << vectorWidth
              << "f16], " << 32 * vectorWidth << ", (add " << vectorRegs[i] << ")>;"
              << std::endl << std::endl;*/
        }
    }

    // add class of all vector lanes, also ones whose lanes don't match
    // for example 2-wide vector consisting of lanes 2 and 3.
    if (vectorRegsAll == "") {
        // dummy already written, no need to write again.
        o << "def " << regClassBase << "Regs : RegisterClass<\"TCE\", [v" << vectorWidth
          << "i32, v" << vectorWidth << "f32], " << 32 * vectorWidth
          << ", (add V" << vectorWidth << "R32DUMMY)> ;" << std::endl;

        o << "def " << regClassBase << "IRegs : RegisterClass<\"TCE\", [v" << vectorWidth
          << "i32], " << 32 * vectorWidth << ", (add V" << vectorWidth
          << "R32DUMMY)> ;" << std::endl;

        o << "def " << regClassBase << "FPRegs : RegisterClass<\"TCE\", [v" << vectorWidth
          << "f32], " << 32 * vectorWidth << ", (add V" << vectorWidth << "R32DUMMY)> ;"
          << std::endl << std::endl;
    } 
    else {
        o << "def " << regClassBase << "Regs : RegisterClass<\"TCE\", [v" << vectorWidth
          << "i32, v" << vectorWidth << "f32], " << 32 * vectorWidth << ", (add "
          << vectorRegsAll << ")> ;" << std::endl;

        o << "def " << regClassBase << "IRegs : RegisterClass<\"TCE\", [v" << vectorWidth
          << "i32], " << 32 * vectorWidth << ", (add "
          << vectorRegsAll << ")> ;" << std::endl;

        o << "def " << regClassBase << "FPRegs : RegisterClass<\"TCE\", [v" << vectorWidth
          << "f32], " << 32 * vectorWidth << ", (add "
          << vectorRegsAll << ")> ;" << std::endl << std::endl;
    }
}

/**
 * Writes register definitions for vector registers consisting from
 * multiple ordinar register files to the output stream.
 */
void
TDGen::writeVectorRegisterInfo(std::ostream& o) {

    const TTAMachine::Machine::RegisterFileNavigator nav =
        mach_.registerFileNavigator();

    int biggestLane = -1;
    for (int i = 0; i < nav.count(); i++) {
        const TTAMachine::RegisterFile* rf = nav.item(i);
        
        if (rf->name().find("L_") == 0 && rf->width() == 32) {
            int lane = rf->name()[2] - 48;
            if (lane > biggestLane) {
                biggestLane = lane;
            }
        }
    }


    maxVectorSize_ = MathTools::roundDownToPowerTwo(biggestLane+1);

    if (maxVectorSize_ == 0) {
        maxVectorSize_ = 1;
    }
    for (int width = 2; width <= 8; width<<=1) {
        writeVectorRegisterInfo(o, width);
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
    o << "def RAReg : RegisterClass<\"TCE\", [i32], 32, (add RA)>;" << 
	std::endl;
}


/**
 * Checks that the target machine has required registers to build a usable
 * plugin.
 *
 * @return True if required registers were found, false if not.
 */
bool
TDGen::checkRequiredRegisters() {
    if (regs32bit_.size() < REQUIRED_I32_REGS) {
        std::string msg =
            (boost::format(
                "Architecture doesn't meet the minimal requirements.\n"
                "Only %d 32 bit general purpose registers found. At least %d\n"
                "needed. ")
             % regs32bit_.size() % REQUIRED_I32_REGS)
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
        LLVMBackend::llvmRequiredOpset(true, littleEndian_);

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
        opNames_["LD32fr"] = "LD32";
        opNames_["LD32fi"] = "LD32";
        opNames_["ST32fr"] = "ST32";
        opNames_["ST32fi"] = "ST32";
        
        opNames_["ST16hr"] = "ST16";
        opNames_["ST16hi"] = "ST16";

    } else {
        opNames_["LDWfr"] = "LDW";
        opNames_["LDWfi"] = "LDW";
        opNames_["STWfr"] = "STW";
        opNames_["STWfi"] = "STW";
        
        opNames_["STHhr"] = "STH";
        opNames_["STHhi"] = "STH";
    }

    // some global/address immediate if conversion fails
    // so commented out
    if (hasConditionalMoves_) {
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

        // these can be written even if these have broken dag as the
        // dag is ignored
        if (op.name() == "STW2" || op.name() == "STW4" || op.name() == "STW8") {
            int vectorWidth = Conversion::toInt(op.name().substr(3));
            // vector store
            writeVectorStoreDefs(os, op, vectorWidth);
            continue;
        }

        if (op.name() == "STH2" || op.name() == "STH4" || op.name() == "STH8") {
            int vectorWidth = Conversion::toInt(op.name().substr(3));
            // vector store
            writeVectorTruncStoreDefs(os, op, 16, vectorWidth);
            continue;
        }

        if (op.name() == "STQ2" || op.name() == "STQ4" || op.name() == "STQ8") {
            int vectorWidth = Conversion::toInt(op.name().substr(3));
            // vector store
            writeVectorTruncStoreDefs(os, op, 8, vectorWidth);
            continue;
        }

        if (op.name() == "LDW2" || op.name() == "LDW4" || op.name() == "LDW8") {
            int vectorWidth = Conversion::toInt(op.name().substr(3));
            // vector store
            writeVectorLoadDefs(os, op, TCEString("load"), vectorWidth);
            continue;
        }

        if (op.name() == "LDH2" || op.name() == "LDH4" || op.name() == "LDH8") {
            int vectorWidth = Conversion::toInt(op.name().substr(3));
            // vector store
            writeVectorLoadDefs(os, op, TCEString("sextloadvi16"), vectorWidth);
            writeVectorAnyextPattern(os, op, TCEString("extloadvi16"), vectorWidth);
            continue;
        }

        if (op.name() == "LDHU2" || op.name() == "LDHU4" || op.name() == "LDHU8") {
            int vectorWidth = Conversion::toInt(op.name().substr(4));
            // vector store
            writeVectorLoadDefs(os, op, TCEString("zextloadvi16"), vectorWidth);
            continue;
        }

        if (op.name() == "LDQ2" || op.name() == "LDQ4" || op.name() == "LDQ8") {
            int vectorWidth = Conversion::toInt(op.name().substr(3));
            // vector store
            writeVectorLoadDefs(os, op, TCEString("sextloadvi8"), vectorWidth);
            writeVectorAnyextPattern(os, op, TCEString("extloadvi8"), vectorWidth);
            continue;
        }

        if (op.name() == "LDQU2" || op.name() == "LDQU4" || op.name() == "LDQU8") {
            int vectorWidth = Conversion::toInt(op.name().substr(4));
            // vector store
            writeVectorLoadDefs(os, op, TCEString("zextloadvi8"), vectorWidth);
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
    createSelectPatterns(os);
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

    const TTAMachine::Machine::FunctionUnitNavigator fuNav =
        mach_.functionUnitNavigator();

    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit* fu = fuNav.item(i);
        for (int o = 0; o < fu->operationCount(); o++) {
            const std::string opName =
                StringTools::stringToLower(fu->operation(o)->name());

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
            if (opName == "sqrtf") hasSQRTF = true;

            if (littleEndian_) {
                if (opName == "ld16" || opName == "ldu16") {
                    has16bitLoads = true;
                } else if(opName == "ld8" || opName == "ldu8") {
                    has8bitLoads = true;
                }
            } else {
                if (opName == "ldh" || opName == "ldhu") {
                    has16bitLoads = true;
                } else if (opName == "ldq" || opName == "ldqu") {
                    has8bitLoads = true;
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
      << has16bitLoads << ";}" << std::endl

      << "int GeneratedTCEPlugin::maxVectorSize() const { return "
      << maxVectorSize_ << "; }" << std::endl;

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
 * m = float2 vec?
 * n = float4 vec?
 * o = float8 vec?
 * p
 * q
 * r = integer Register
 * s
 * t
 * u
 * v = vector2 (integer)
 * w = vector4 (integer)
 * x = vector8 (integer)
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
        op.name() != "LD32" && op.name() != "LD64" &&
        op.name() != "ST8" && op.name() != "ST16" &&
        op.name() != "ST32" && op.name() != "ST64" &&
        op.name() != "ALD8" && op.name() != "ALDU8" &&
        op.name() != "ALD16" && op.name() != "ALDU16" &&
        op.name() != "ALD32" && op.name() != "ALD64" &&
        op.name() != "AST8" && op.name() != "AST16" &&
        op.name() != "AST32" && op.name() != "AST64") {

        if (op.readsMemory()) attrs += " mayLoad = 1";
        if (op.readsMemory() && op.writesMemory()) attrs += ", ";
        if (op.writesMemory()) attrs += " mayStore = 1";
    }

    // no bool outs for some operatios
    if (op.name() == "CFI" || op.name() == "CFIU") {
        writeOperationDef(o, op, "rf", attrs, skipPattern);
        writeOperationDef(o, op, "vm", attrs, skipPattern, "_VECTOR_2_");
        writeOperationDef(o, op, "wn", attrs, skipPattern, "_VECTOR_4_");
        writeOperationDef(o, op, "xo", attrs, skipPattern, "_VECTOR_8_");

        return;
    }
        
    // rotations are allways n x n -> n bits.
    if (op.name() == "ROTL" || op.name() == "ROTR" ||
        op.name() == "SHL" || op.name() == "SHR" || op.name() == "SHRU") {
        writeOperationDefs(o, op, "rrr", attrs, skipPattern);

        writeOperationDef(o, op, "vvv", attrs, skipPattern, "_VECTOR_2_");
        writeOperationDef(o, op, "www", attrs, skipPattern, "_VECTOR_4_");
        writeOperationDef(o, op, "xxx", attrs, skipPattern, "_VECTOR_8_");
        return;
    }

    if (op.name() == "SXHW" || op.name() == "SXQW") {
        writeOperationDef(o, op, "rr", attrs, skipPattern);
        writeOperationDef(o, op, "vv", attrs, skipPattern, "_VECTOR_2_");
        writeOperationDef(o, op, "ww", attrs, skipPattern, "_VECTOR_4_");
        writeOperationDef(o, op, "xx", attrs, skipPattern, "_VECTOR_8_");
        return;
    }

    // these can have 1-bit inputs
    if (op.name() == "XOR" || op.name() == "IOR" || op.name() == "AND" ||
        op.name() == "ANDN" || op.name() == "ADD" || op.name() == "SUB") {
        
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

            writeOperationDef(o, op, "ii", attrs, skipPattern);
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
            outOperand.type() == Operand::SINT_WORD) {

            // 32 to 1-bit operations
            operandTypes[0] = 'b';
            writeOperationDefs(o, op, operandTypes, attrs, skipPattern);
        } 

        // create vector versions.

	// TODO: no half vectors yet, pending f16v4..f16v16 types in llvm.
	if (outOperand.type() != Operand::HALF_FLOAT_WORD && 
            op.operand(1).type() != Operand::HALF_FLOAT_WORD) { 
            for (int i = 0, w = 2; i < 3; i++, w<<=1) {
                char floatChar = 'm' + i;
                char intChar = 'v' + i;
                TCEString s = createDefaultOperandTypeString(op);
                for (unsigned int j = 0; j < s.length(); j++) {
                    if (s[j] == 'r') {
                        s[j] = intChar;
                    } else if (s[j] == 'f') {
                        s[j] = floatChar;
                    }
                }
                writeOperationDef(
                    o, op, s , attrs, skipPattern, 
                    TCEString("_VECTOR_") + Conversion::toString(w) + "_");
            }
        }
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
            switch(c) {
            case 'r':
                c = 'i';
                break;
            case 'b':
                c = 'j';
                break;
            case 'f':
                c = 'k';
                break;
            case 'h':
                c = 'l';
                break;
            default:
                continue;
            }
            writeOperationDef(o, op, opTypes, attrs, skipPattern, backendPrefix);
        }
    }
}

void TDGen::writeVectorStoreDefs(
    std::ostream& o, const TCEString& opName, 
    const TCEString& opNameSuffix, bool addrImm,
    const TCEString& dataType) {

    TCEString addrType;
    TCEString addrTypePat;
    TCEString opNameLLVM = opName + opNameSuffix;

    if (addrImm) {
        addrType = "MEMri";
        addrTypePat = "ADDRri";
    } else {
        addrType = "MEMrr";
        addrTypePat = "ADDRrr";
    }

    o << "def " << opNameLLVM << " : InstTCE<(outs), (ins " << addrType << ":$addr, " << dataType << ":$data),"
      << "\"\", [(store " << dataType << ":$data, " << addrTypePat << ":$addr)]>;" << std::endl;

    o << "def PRED_TRUE_" << opNameLLVM << " : InstTCE<(outs), (ins R1Regs:$pred, " 
      << addrType << ":$addr, " << dataType << ":$data),"
      << "\"\", []>;" << std::endl;

    o << "def PRED_FALSE_" << opNameLLVM << " : InstTCE<(outs), (ins R1Regs:$pred, " 
      << addrType << ":$addr, " << dataType << ":$data),"
      << "\"\", []>;" << std::endl;

    opNames_[opNameLLVM] = opName;
    opNames_["PRED_TRUE_" + opNameLLVM] = opName;
    opNames_["PRED_FALSE_" + opNameLLVM] = opName;

    truePredOps_[opNameLLVM] = "PRED_TRUE_" + opNameLLVM;
    falsePredOps_[opNameLLVM] = "PRED_FALSE_" + opNameLLVM;
}


void 
TDGen::writeVectorStoreDefs(
    std::ostream& o,
    Operation& op, int vectorLen) {


    TCEString dataTypeInt = "V";
    TCEString dataTypeFP = "V";
    dataTypeInt << vectorLen << "R32IRegs";
    dataTypeFP << vectorLen << "R32FPRegs";
    
    writeVectorStoreDefs(o, op.name(), "vr", false, dataTypeInt);//, loadPatternName);
    writeVectorStoreDefs(o, op.name(), "vi", true, dataTypeInt);//, loadPatternName);
    writeVectorStoreDefs(o, op.name(), "mr", false, dataTypeFP);//, loadPatternName);
    writeVectorStoreDefs(o, op.name(), "mi", true, dataTypeFP);//, loadPatternName);
}

void 
TDGen::writeVectorTruncStoreDefs(
    std::ostream& o,
    Operation& op, int bitsize, int vectorLen) {

    o << "def " << op.name() << "vr : InstTCE<(outs), (ins MEMrr:$addr, V" << vectorLen << "R32IRegs:$data),"
      << "\"\", [(truncstorev" << vectorLen << "i" << bitsize <<" V" << vectorLen << "R32IRegs:$data, ADDRrr:$addr)]>;" << std::endl;
    
    o << "def " << op.name() << "vi : InstTCE<(outs), (ins MEMri:$addr, V" << vectorLen << "R32IRegs:$data),"
      << "\"\", [(truncstorev" << vectorLen << "i" << bitsize << " V" << vectorLen << "R32IRegs:$data, ADDRri:$addr)]>;" << std::endl;

    o << "def PRED_TRUE_" << op.name() << "vr : InstTCE<(outs), (ins R1Regs:$pred, MEMrr:$addr, V" << vectorLen << "R32IRegs:$data),"
      << "\"\", []>;" << std::endl;
    
    o << "def PRED_TRUE_" << op.name() << "vi : InstTCE<(outs), (ins R1Regs:$pred, MEMri:$addr, V" << vectorLen << "R32IRegs:$data),"
      << "\"\", []>;" << std::endl;

    o << "def PRED_FALSE_" << op.name() << "vr : InstTCE<(outs), (ins R1Regs:$pred, MEMrr:$addr, V" << vectorLen << "R32IRegs:$data),"
      << "\"\", []>;" << std::endl;
    
    o << "def PRED_FALSE_" << op.name() << "vi : InstTCE<(outs), (ins R1Regs:$pred, MEMri:$addr, V" << vectorLen << "R32IRegs:$data),"
      << "\"\", []>;" << std::endl;

    opNames_[op.name() + "vr"] = op.name();
    opNames_[op.name() + "vi"] = op.name();
    opNames_["PRED_TRUE_" + op.name() + "vr"] = "?" + op.name();
    opNames_["PRED_TRUE_" + op.name() + "vi"] = "?" + op.name();
    opNames_["PRED_FALSE_" + op.name() + "vr"] = "!" + op.name();
    opNames_["PRED_FALSE_" + op.name() + "vi"] = "!" + op.name();

    truePredOps_[op.name() + "vr"] = "PRED_TRUE_" + op.name() + "vr";
    falsePredOps_[op.name() + "vr"] = "PRED_FALSE_" + op.name() + "vr";

    truePredOps_[op.name() + "vi"] = "PRED_TRUE_" + op.name() + "vi";
    falsePredOps_[op.name() + "vi"] = "PRED_FALSE_" + op.name() + "vi";
}

void
TDGen::writeVectorLoadDefs(
    std::ostream& o,
    Operation& op, const TCEString& loadPatternName, int vectorLen) {
   
    TCEString resultTypeInt = "V";
    TCEString resultTypeFP = "V";
    resultTypeInt << vectorLen << "R32IRegs";
    resultTypeFP << vectorLen << "R32FPRegs";
    
    writeVectorLoadDefs(o, op.name(), "vr", false, resultTypeInt, loadPatternName);
    writeVectorLoadDefs(o, op.name(), "vi", true, resultTypeInt, loadPatternName);
    writeVectorLoadDefs(o, op.name(), "mr", false, resultTypeFP, loadPatternName);
    writeVectorLoadDefs(o, op.name(), "mi", true, resultTypeFP, loadPatternName);
}

void TDGen::writeVectorLoadDefs(std::ostream& o, const TCEString& opName, const TCEString& opNameSuffix, bool addrImm,
				const TCEString& resultType, const TCEString& loadPatternName) {

    TCEString addrType;
    TCEString addrTypePat;
    TCEString opNameLLVM = opName + opNameSuffix;

    if (addrImm) {
        addrType = "MEMri";
        addrTypePat = "ADDRri";
    } else {
        addrType = "MEMrr";
        addrTypePat = "ADDRrr";
    }

    o << "def " << opNameLLVM << " : InstTCE<(outs " << resultType << ":$data), (ins " << addrType << ":$addr),"
      << "\"\", [(set " << resultType << ":$data, (" << loadPatternName << " " << addrTypePat << ":$addr))]>;" << std::endl;

    o << "def PRED_TRUE_" << opNameLLVM << " : InstTCE<(outs " << resultType << ":$data), (ins R1Regs:$pred, " << addrType << ":$addr),"
      << "\"\", []>;" << std::endl;

    o << "def PRED_FALSE_" << opNameLLVM << " : InstTCE<(outs " << resultType << ":$data), (ins R1Regs:$pred, " << addrType << ":$addr),"
      << "\"\", []>;" << std::endl;

    opNames_[opNameLLVM] = opName;
    opNames_["PRED_TRUE_" + opNameLLVM] = opName;
    opNames_["PRED_FALSE_" + opNameLLVM] = opName;

    truePredOps_[opNameLLVM] = "PRED_TRUE_" + opNameLLVM;
    falsePredOps_[opNameLLVM] = "PRED_FALSE_" + opNameLLVM;

}

void TDGen::writeVectorAnyextPattern(
    std::ostream& o,
    Operation& op, const TCEString& loadPatternName, int vectorLen) {
    TCEString vregi = "V"; vregi << vectorLen << "R32IRegs";
    TCEString vregfp = "V"; vregfp << vectorLen << "R32FPRegs";
    o << "def : Pat<(v" << vectorLen << "i32 (" << loadPatternName << " ADDRrr:$addr)), (" << 
                              op.name() << "vr ADDRrr:$addr)>;" << std::endl;

    o << "def : Pat<(v" << vectorLen << "i32 (" << loadPatternName << " ADDRri:$addr)), (" << 
                              op.name() << "vr ADDRri:$addr)>;" << std::endl;

    o << "def : Pat<(v" << vectorLen << "f32 (" << loadPatternName << " ADDRrr:$addr)), (" << 
                              op.name() << "mr ADDRrr:$addr)>;" << std::endl;

    o << "def : Pat<(v" << vectorLen << "f32 (" << loadPatternName << " ADDRri:$addr)), (" << 
                              op.name() << "mi ADDRri:$addr)>;" << std::endl;
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

    
    std::string outputs, inputs, asmstr, pattern;
    outputs = "(outs" + patOutputs(op, operandTypes) + ")";
    inputs = "(ins " + patInputs(op, operandTypes) + ")";
    std::string predicatedInputs = 
	"(ins R1Regs:$pred, " +patInputs(op, operandTypes)+ ")";

    asmstr = "\"\"";
    
    if (!skipPattern) {
        if (llvmOperationPattern(op,'r') != "" || 
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
    } else if (operand.type() != Operand::UINT_WORD &&
        operand.type() != Operand::SINT_WORD &&
        operand.type() != Operand::RAW_DATA) {
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
        std::string llvmPat = llvmOperationPattern(op, 'r');
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
                    inputType = 'i';
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
                op.name() == "GE" ||op.name() == "GEU" ||
                op.name() == "GT" || op.name() == "GTU" ||
                op.name() == "LE" || op.name() == "LEU" ||
                op.name() == "LT" || op.name() == "LTU" ||
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
    if (opName == "sub") return "sub %1%, %2%";
    if (opName == "mul") return "mul %1%, %2%";
    if (opName == "div") return "sdiv %1%, %2%";
    if (opName == "divu") return "udiv %1%, %2%";
    if (opName == "mod") return "srem %1%, %2%";
    if (opName == "modu") return "urem %1%, %2%";

    if (opName == "shl") return "shl %1%, %2%";
    if (opName == "shr") return "sra %1%, %2%";
    if (opName == "shru") return "srl %1%, %2%";
    if (opName == "rotl") return "rotl %1%, %2%";
    if (opName == "rotr") return "rotr %1%, %2%";

    if (opName == "and") return "and %1%, %2%";
    if (opName == "ior") return "or %1%, %2%";
    if (opName == "xor") return "xor %1%, %2%";

    if (opName == "eq") return "seteq %1%, %2%";
    if (opName == "ne") return "setne %1%, %2%";
    if (opName == "lt") return "setlt %1%, %2%";
    if (opName == "le") return "setle %1%, %2%";
    if (opName == "gt") return "setgt %1%, %2%";
    if (opName == "ge") return "setge %1%, %2%";
    if (opName == "ltu") return "setult %1%, %2%";
    if (opName == "leu") return "setule %1%, %2%";
    if (opName == "gtu") return "setugt %1%, %2%";
    if (opName == "geu") return "setuge %1%, %2%";

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
    if (opName == "chf") return "fextend %1%";
#else
    if (opName == "cfh") return "fpround %1%";
    if (opName == "chf") return "fpextend %1%";
#endif

    if (opName == "cih") return "sint_to_fp %1%";
    if (opName == "chi") return "fp_to_sint %1%";
    if (opName == "cihu") return "uint_to_fp %1%";
    if (opName == "chiu") return "fp_to_uint %1%";

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
        if (opName == "ld32") return "load %1%";
        //if (opName == "ldd") return "load";
        
        if (opName == "st8") return "truncstorei8 %2%, %1%";
        if (opName == "st16") return "truncstorei16 %2%, %1%";
        if (opName == "st32") return "store %2%, %1%";
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

    if (opName == "sxhw") {
        switch (operandType) {
        case 'x':
            return "sext_inreg %1%, v8i16";
        case 'w':
            return "sext_inreg %1%, v4i16";
        case 'v':
            return "sext_inreg %1%, v2i16";
        case 'r':
        default:
            return "sext_inreg %1%, i16";
        }
    }
    if (opName == "sxqw") {
        switch (operandType) {
        case 'x':
            return "sext_inreg %1%, v8i8";
        case 'w':
            return "sext_inreg %1%, v4i8";
        case 'v':
            return "sext_inreg %1%, v2i8";
        case 'r':
        default:
            return "sext_inreg %1%, i8";
        }
    }

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

    if (opName == "shl") return "shl";
    if (opName == "shr") return "sra";
    if (opName == "shru") return "srl";
    if (opName == "rotl") return "rotl";
    if (opName == "rotr") return "rotr";

    if (opName == "and") return "and";
    if (opName == "ior") return "or";
    if (opName == "xor") return "xor";

    if (opName == "eq") return "seteq";
    if (opName == "ne") return "setne";
    if (opName == "lt") return "setlt";
    if (opName == "le") return "setle";
    if (opName == "gt") return "setgt";
    if (opName == "ge") return "setge";
    if (opName == "ltu") return "setult";
    if (opName == "leu") return "setule";
    if (opName == "gtu") return "setugt";
    if (opName == "geu") return "setuge";

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
        if (opName == "ld32") return "load";
        //if (opName == "ldd") return "load";
        
        if (opName == "st8") return "truncstorei8";
        if (opName == "st16") return "truncstorei16";
        if (opName == "st32") return "store";
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

    if (opName == "sxhw") return "sext_inreg";
    if (opName == "sxqw") return "sext_inreg";
    if (opName == "sxbw") return "sext_inreg";

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
    
    // if operation has llvm pattern
    if (llvmOperationPattern(op,'r') != "") {
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
            case 'h':
                return "(f16 " + Conversion::toString(node.value()) + ")";
                // TODO: f16 vectors not yet implemented
            case 'f':
            case 'k':
                return "(f32 " + Conversion::toString(node.value()) + ")";
            case 'm':
                return ("(v2f32 (build_vector (f32 " + 
                        Conversion::toString(node.value()) +
                        "),(f32 " +
                        Conversion::toString(node.value()) +
                        ")))");
            case 'n':
                return ("(v4f32 (build_vector (f32 " + 
                        Conversion::toString(node.value()) +
                        "),(f32 " +
                        Conversion::toString(node.value()) +
                        "),(f32 " +
                        Conversion::toString(node.value()) +
                        "),(f32 " +
                        Conversion::toString(node.value()) +
                        ")))");
            case 'o':
                return ("(v8f32 (build_vector (f32 " + 
                        Conversion::toString(node.value()) +
                        "),(f32 " +
                        Conversion::toString(node.value()) +
                        "),(f32 " +
                        Conversion::toString(node.value()) +
                        "),(f32 " +
                        Conversion::toString(node.value()) +
                        "),(f32 " +
                        Conversion::toString(node.value()) +
                        "),(f32 " +
                        Conversion::toString(node.value()) +
                        "),(f32 " +
                        Conversion::toString(node.value()) +
                        "),(f32 " +
                        Conversion::toString(node.value()) +
                        ")))");
            case 'r':
            case 'i':
                return "(i32 " + Conversion::toString(node.value()) + ")";
            case 'v':
                return ("(v2i32 (build_vector (i32 " + 
                        Conversion::toString(node.value()) +
                        "),(i32 " +
                        Conversion::toString(node.value()) +
                        ")))");
            case 'w':
                return ("(v4i32 (build_vector (i32 " + 
                        Conversion::toString(node.value()) +
                        "),(i32 " +
                        Conversion::toString(node.value()) +
                        "),(i32 " +
                        Conversion::toString(node.value()) +
                        "),(i32 " +
                        Conversion::toString(node.value()) +
                        ")))");
            case 'x':
                return ("(v8i32 (build_vector (i32 " + 
                        Conversion::toString(node.value()) +
                        "),(i32 " +
                        Conversion::toString(node.value()) +
                        "),(i32 " +
                        Conversion::toString(node.value()) +
                        "),(i32 " +
                        Conversion::toString(node.value()) +
                        "),(i32 " +
                        Conversion::toString(node.value()) +
                        "),(i32 " +
                        Conversion::toString(node.value()) +
                        "),(i32 " +
                        Conversion::toString(node.value()) +
                        "),(i32 " +
                        Conversion::toString(node.value()) +
                        ")))");
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
                            operand.type() == Operand::UINT_WORD ||
                            operand.type() == Operand::RAW_DATA) {
                            operationName += 'i';
                        } else {
                            operationName += 'k';
                        }
                    } else {
                        TerminalNode* t = 
                            dynamic_cast<TerminalNode*>(
                                &(dag.tailNode(edge)));
                        assert (t != NULL);
                        int strIndex = t->operandIndex() -1 + 
                            op.numberOfOutputs();
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
            if (match) {
                return "MEMri:$op" + Conversion::toString(idx);
            } else {
                return "ADDRri:$op" + Conversion::toString(idx);
            }
        case 'r':
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
        case 'i':
            if (match) {
                return "i32imm:$op" + Conversion::toString(idx);
            } else {
                return "(i32 imm:$op" + Conversion::toString(idx) + ")";
            }
        case 'j':
            if (match) {
                return "i1imm:$op" + Conversion::toString(idx);
            } else {
                return "(i1 imm:$op" + Conversion::toString(idx) + ")";
            }
        case 'r':
            return "R32IRegs:$op" + Conversion::toString(idx);
        case 'b':
            return "R1Regs:$op" + Conversion::toString(idx);
        case 'v':
            return "V2R32IRegs:$op" + Conversion::toString(idx);
        case 'w':
            return "V4R32IRegs:$op" + Conversion::toString(idx);
        case 'x':
            return "V8R32IRegs:$op" + Conversion::toString(idx);
        case 'f':
            if (operand.type() == Operand::RAW_DATA) {
                return "R32FPRegs:$op" + Conversion::toString(idx);
            }
            /* fall through */
        case 'h':
            if (operand.type() == Operand::RAW_DATA) {
                return "R32HFPRegs:$op" + Conversion::toString(idx);
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
            return "R32FPRegs:$op" + Conversion::toString(idx);
        case 'm':
            return "V2R32FPRegs:$op" + Conversion::toString(idx);
        case 'n':
            return "V4R32FPRegs:$op" + Conversion::toString(idx);
        case 'o':
            return "V8R32FPRegs:$op" + Conversion::toString(idx);

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
            return "R32HFPRegs:$op" + Conversion::toString(idx);
        case 'm':
            //return "V2R32HFPRegs:$op" + Conversion::toString(idx);
        case 'n':
            //return "V4R32HFPRegs:$op" + Conversion::toString(idx);
        case 'o':
            //return "V8R32HFPRegs:$op" + Conversion::toString(idx);

        default:
            std::string msg = 
                "invalid operation type for half operand:";
            msg += operandType;
            throw InvalidData(__FILE__, __LINE__, __func__, msg);
        }
    } else if (operand.type() == Operand::DOUBLE_WORD) {
        // TODO: immediate check??
        return "F64Regs:$op" + Conversion::toString(idx);
    } else {
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
    if (!mach_.hasOperation(halfLoad)) {
        halfLoad = littleEndian_ ? "LDU16;" : "LDHU";
        if (!mach_.hasOperation(halfLoad)) {
            halfLoad="";
        }
    }

    if (!littleEndian_) {
        if (opNames_.find("STW2vr") != opNames_.end()) {
            os << "\tif (rc == " << prefix << "TCE::V2R32I" << rcpf 
               << ") return TCE::STW2vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V2R32FP" << rcpf 
               << ") return TCE::STW2mr;"
               << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::V2R32_L_0I" << rcpf 
               << ") return TCE::STW2vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V2R32_L_0FP" << rcpf << ") return TCE::STW2mr;"
               << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::V2R32_L_2I" << rcpf << ") return TCE::STW2vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V2R32_L_2FP" << rcpf << ") return TCE::STW2mr;"
               << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::V2R32_L_4I" << rcpf << ") return TCE::STW2vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V2R32_L_4FP" << rcpf << ") return TCE::STW2mr;"
               << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::V2R32_L_6I" << rcpf << ") return TCE::STW2vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V2R32_L_6FP" << rcpf << ") return TCE::STW2mr;"
               << std::endl;
        }
        if (opNames_.find("STW4vr") != opNames_.end()) {
            os << "\tif (rc == " << prefix << "TCE::V4R32I" << rcpf << ") return TCE::STW4vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V4R32FP" << rcpf << ") return TCE::STW4mr;"
               << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::V4R32_L_0I" << rcpf << ") return TCE::STW4vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V4R32_L_0FP" << rcpf << ") return TCE::STW4mr;"
               << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::V4R32_L_4I" << rcpf << ") return TCE::STW4vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V4R32_L_4FP" << rcpf << ") return TCE::STW4mr;"
               << std::endl;
        }
        if (opNames_.find("STW8vr") != opNames_.end()) {
            os << "\tif (rc == " << prefix << "TCE::V8R32I" << rcpf << ") return TCE::STW8vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V8R32FP" << rcpf << ") return TCE::STW8mr;"
               << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::V8R32_L_0I" << rcpf << ") return TCE::STW8vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V8R32_L_0FP" << rcpf << ") return TCE::STW8mr;"
               << std::endl;
        }
    }
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
        << "\tassert(0&&\"Storing given regclass to stack not supported. "
        << "Bug in backend?\");"
        << std::endl
        << "} " << std::endl
        << std::endl
              
        << "int GeneratedTCEPlugin::getLoad(const TargetRegisterClass *rc)"
        << " const {" << std::endl;

    os << "\tif (rc == " << prefix << rapf << ") return TCE::" << intLoad << "RAr;"
       << std::endl;

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
    }

    for (RegClassMap::iterator ri = regsInRFClasses_.begin(); 
         ri != regsInRFClasses_.end(); ri++) {
 
            os << "\tif (rc == " << prefix 
               << "TCE::R32_"  << ri->first << "_"
               << rcpf << ") return TCE::" << intLoad << "rr;" << std::endl;
            
    }
    
    if (!littleEndian_) {
        if (opNames_.find("LDW2vr") != opNames_.end()) {
            os << "\tif (rc == " << prefix << "TCE::V2R32I" << rcpf << ") return TCE::LDW2vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V2R32FP" << rcpf << ") return TCE::LDW2mr;"
               << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::V2R32_L_0I" << rcpf << ") return TCE::LDW2vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V2R32_L_0FP" << rcpf << ") return TCE::LDW2mr;"
               << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::V2R32_L_2I" << rcpf << ") return TCE::LDW2vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V2R32_L_2FP" << rcpf << ") return TCE::LDW2mr;"
               << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::V2R32_L_4I" << rcpf << ") return TCE::LDW2vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V2R32_L_4FP" << rcpf << ") return TCE::LDW2mr;"
               << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::V2R32_L_6I" << rcpf << ") return TCE::LDW2vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V2R32_L_6FP" << rcpf << ") return TCE::LDW2mr;"
               << std::endl;
        }
        if (opNames_.find("LDW4vr") != opNames_.end()) {
            os << "\tif (rc == " << prefix << "TCE::V4R32I" << rcpf << ") return TCE::LDW4vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V4R32FP" << rcpf << ") return TCE::LDW4mr;"
               << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::V4R32_L_0I" << rcpf << ") return TCE::LDW4vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V4R32_L_0FP" << rcpf << ") return TCE::LDW4mr;"
               << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::V4R32_L_4I" << rcpf << ") return TCE::LDW4vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V4R32_L_4FP" << rcpf << ") return TCE::LDW4mr;"
               << std::endl;
        }
        if (opNames_.find("LDW8vr") != opNames_.end()) {
            os << "\tif (rc == " << prefix << "TCE::V8R32I" << rcpf << ") return TCE::LDW8vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V8R32FP" << rcpf << ") return TCE::LDW8mr;"
           << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::V8R32_L_0I" << rcpf << ") return TCE::LDW8vr;"
               << std::endl
               << "\tif (rc == " << prefix << "TCE::V8R32_L_0FP" << rcpf << ") return TCE::LDW8mr;"
               << std::endl;
        }
    }
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
        << "\tassert(0&&\"loading from stack to given regclass not supported."
        << " Bug in backend?\");"
        << std::endl
        << "} " << std::endl
        << std::endl;
}

void
TDGen::createMinMaxGenerator(std::ostream& os) {

    // MIN
    os  << "int GeneratedTCEPlugin::getMinOpcode(SDNode* n) const {" << std::endl
        << "\tEVT vt = n->getOperand(1).getValueType();" << std::endl;
    if (opNames_.find("MINrrr") != opNames_.end()) {
        os << "if (vt == MVT::i32) return TCE::MINrrr;" << std::endl;
    }
    if (opNames_.find("MINFfff") != opNames_.end()) {
        os << "if (vt == MVT::f32) return TCE::MINFfff;" << std::endl;
    }
    os << "\treturn -1; " << std::endl << "}" << std::endl;

    // MAX
    os  << "int GeneratedTCEPlugin::getMaxOpcode(SDNode* n) const {" << std::endl
        << "\tEVT vt = n->getOperand(1).getValueType();" << std::endl;
    if (opNames_.find("MAXrrr") != opNames_.end()) {
        os << "if (vt == MVT::i32) return TCE::MAXrrr;" << std::endl;
    }
    if (opNames_.find("MAXFfff") != opNames_.end()) {
        os << "if (vt == MVT::f32) return TCE::MAXFfff;" << std::endl;
    }
    os << "\treturn -1; " << std::endl << "}" << std::endl;
    
    // MINU
    os  << "int GeneratedTCEPlugin::getMinuOpcode(SDNode* n) const {" << std::endl;
    if (opNames_.find("MINUrrr") != opNames_.end()) {
        os << "\tEVT vt = n->getOperand(1).getValueType();" << std::endl;
        os << "if (vt == MVT::i32) return TCE::MINUrrr;" << std::endl;
    }
    os << "\treturn -1; " << std::endl << "}" << std::endl;

    // MAXU
    os  << "int GeneratedTCEPlugin::getMaxuOpcode(SDNode* n) const {" << std::endl;
    if (opNames_.find("MAXUrrr") != opNames_.end()) {
        os << "\tEVT vt = n->getOperand(1).getValueType();" << std::endl;
        os << "if (vt == MVT::i32) return TCE::MAXUrrr;" << std::endl;
    }
    os << "\treturn -1; " << std::endl << "}" << std::endl;
}

void TDGen::createEndiannesQuery(std::ostream& os) {
    os << "bool GeneratedTCEPlugin::isLittleEndian() const {" << std::endl;
    os << "return " << littleEndian_ << "; }" << std::endl;
}

void TDGen::createByteExtLoadPatterns(std::ostream& os) {
    TCEString load = littleEndian_ ? "LD8" : "LDQ";
    TCEString uload = littleEndian_ ? "LDU8" : "LDQU";

    if (mach_.hasOperation(load)) {
        if (!mach_.hasOperation(uload)) {

            // emulate zero ext with sing-ext and and
            os << "def : Pat<(i32 (zextloadi8 ADDRrr:$addr)), "
               << "(ANDrri (" << load << "rr ADDRrr:$addr), 255)>;"
               << std::endl;
            os << "def : Pat<(i32 (zextloadi8 ADDRri:$addr)), "
               << "(ANDrri (" << load << "ri ADDRri:$addr), 255)>;"
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
        load = uload;

        if (mach_.hasOperation("SXQW")) {
            // use zextload + sext for sextload
            os << "def : Pat<(i32 (sextloadi8 ADDRrr:$addr)), "
               << "(SXQWrr (" << load << "rr ADDRrr:$addr))>;" << std::endl;
            os << "def : Pat<(i32 (sextloadi8 ADDRri:$addr)), "
               << "(SXQWrr (" << load << "ri ADDRri:$addr))>;" << std::endl;

        } else {
            std::cerr << "Warning: no sign-extending 8-bit loads or"
                      << " 8-bit sign extension instruction!"
                      << " in the processor. All code may not compile!"
                      << std::endl;
        }
    }

    os << "def : Pat<(i32 (zextloadi1 ADDRrr:$addr)), ("
       << load << "rr ADDRrr:$addr)>;"
       << std::endl
       << "def : Pat<(i32 (zextloadi1 ADDRri:$addr)), ("
       << load << "ri ADDRri:$addr)>;"
       << std::endl;

    os << "def : Pat<(i32 (sextloadi1 ADDRrr:$addr)), "
       << "(SUBrir 0, "
       << "(ANDrrr (" << load << "rr ADDRrr:$addr), 1))>;"
       << std::endl
       << "def : Pat<(i32 (sextloadi1 ADDRri:$addr)), "
       << "(SUBrir 0, "
       <<  "(ANDrrr (" << load << "ri ADDRri:$addr), 1))>;"
       << std::endl
       << "// anyextloads" << std::endl;

    os << "def : Pat<(i32 (extloadi1 ADDRrr:$src)), (" << load << "rr ADDRrr:$src)>;" << std::endl
       << "def : Pat<(i32 (extloadi1 ADDRri:$src)), (" << load << "ri ADDRri:$src)>;" << std::endl
       << "def : Pat<(i32 (extloadi8 ADDRrr:$src)), (" << load << "rr ADDRrr:$src)>;" << std::endl
       << "def : Pat<(i32 (extloadi8 ADDRri:$src)), (" << load << "ri ADDRri:$src)>;" << std::endl
       << std::endl;
}

void TDGen::createShortExtLoadPatterns(std::ostream& os) {
    TCEString load = littleEndian_ ? "LD16" : "LDH";
    TCEString uload = littleEndian_ ? "LDU16" : "LDHU";

    if (mach_.hasOperation(load)) {
        if (!mach_.hasOperation(uload)) {
            // emulate zero ext with sing-ext and and
            os << "def : Pat<(i32 (zextloadi16 ADDRrr:$addr)), "
               << "(ANDrri (" << load << "rr ADDRrr:$addr), 65535)>;" << std::endl;
            os << "def : Pat<(i32 (zextloadi16 ADDRri:$addr)), "
               << "(ANDrri (" << load << "ri ADDRri:$addr), 65535)>;" << std::endl;
        }
    } else {
        if (!mach_.hasOperation(uload)) {
            std::cerr << "Warning: The architecture is missing any 16-bit loads."
                      << std::endl;
            return;
        }
        load = uload;

        if (mach_.hasOperation("SXHW")) {
            // use zextload + sext for sextload
            os << "def : Pat<(i32 (sextloadi16 ADDRrr:$addr)), "
               << "(SXHWrr (" << load << "rr ADDRrr:$addr))>;" << std::endl;
            os << "def : Pat<(i32 (sextloadi16 ADDRri:$addr)), "
               << "(SXHWrr (" << load << "ri ADDRri:$addr))>;" << std::endl;
        } else {
            std::cerr << "Warning: no sign-extending 16-bit loads or"
                      << " 16-bit sign extension instruction!"
                      << " in the processor. All code may not compile!" << std::endl;
        }

    }
    // anyext.
    os << "def : Pat<(i32 (extloadi16 ADDRrr:$src)), ("
       << load << "rr ADDRrr:$src)>;" << std::endl
       << "def : Pat<(i32 (extloadi16 ADDRri:$src)), ("
       << load << "ri ADDRri:$src)>;" << std::endl;
}

void
TDGen::writeCallingConv(std::ostream& os) {
    writeCallingConvLicenceText(os);

    os << "// Function return value types." << std::endl;
    os << "def RetCC_TCE : CallingConv<[" << std::endl
       << "  CCIfType<[i1], CCPromoteToType<i32>>," << std::endl
       << "  CCIfType<[i32], CCAssignToReg<[IRES0]>>," << std::endl
       << "  CCIfType<[f32], CCAssignToReg<[IRES0]>>," << std::endl
       << "  CCAssignToStack<4, 4>" << std::endl
       << "]>;" << std::endl << std::endl;

    os << "// Function argument value types." << std::endl;
    os << 
        "def CC_TCE : CallingConv<[" << std::endl <<
        "  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>," << std::endl <<
        "  CCIfType<[i32], CCAssignToReg<[IRES0]>>," << std::endl;

    os << 
        "  // Integer values get stored in stack slots that are 4 bytes in "
       << std::endl <<
        "  // size and 4-byte aligned." << std::endl << 
        "  CCIfType<[i32, f32], CCAssignToStack<4, 4>>," << std::endl <<
        "  // Integer values get stored in stack slots that are 8 bytes in"
       << std::endl <<
        "  // size and 8-byte aligned." << std::endl << 
        "  CCIfType<[f64], CCAssignToStack<8, 8>>" << std::endl << 

        "]>;" << std::endl;
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
    os << std::endl
       << "unsigned GeneratedTCEPlugin::getMaxMemoryAlignment() const {"
       << std::endl 
       << "\treturn 4;" 
       << std::endl << "}" << std::endl;
}

void TDGen::createSelectPatterns(std::ostream& os) {
    os << "// Creating select patterns. " << std::endl;
    if (!hasSelect_) {
        os << "// Does not have select instr. " << std::endl;
        if (!hasConditionalMoves_) {
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
            
            os << "def : Pat<(f32 (select R1Regs:$c, R32FPRegs:$t,R32FPRegs:$f)),"
               << "(IORfff (ANDfff R32FPRegs:$t, (SUBfir 0, (ANDext R1Regs:$c, 1))),"
               << "(ANDfff R32FPRegs:$f, (SUBfri (ANDext R1Regs:$c,1),1)))>;"
               << std::endl << std::endl
                
               << "def : Pat<(f16 (select R1Regs:$c,R32HFPRegs:$t,R32HFPRegs:$f)),"
               << "(IORhhh (ANDhhh R32HFPRegs:$t, (SUBhir 0, (ANDext R1Regs:$c, 1))),"
               << "(ANDhhh R32HFPRegs:$f, (SUBhri (ANDext R1Regs:$c,1),1)))>;"
               << std::endl << std::endl;
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
                
               << "def SELECT_F32 : InstTCE<(outs R32FPRegs:$dst),"
               << "(ins R1Regs:$c, R32FPRegs:$T, R32FPRegs:$F),"
               << "\"# SELECT_F32 PSEUDO!\","
               << "[(set R32FPRegs:$dst,"
               << "(select R1Regs:$c, R32FPRegs:$T, R32FPRegs:$F))]>;"
               << std::endl << std::endl
                
               << "def SELECT_F16 : InstTCE<(outs R32HFPRegs:$dst),"
               << "(ins R1Regs:$c, R32HFPRegs:$T, R32HFPRegs:$F),"
               << "\"# SELECT_F16 PSEUDO!\","
               << "[(set R32HFPRegs:$dst, "
               << "(select R1Regs:$c, R32HFPRegs:$T, R32HFPRegs:$F))]>;"
               << std::endl << std::endl;
        }            
    } else {
        os << "// Has select instr!. " << std::endl;
    }
}

void TDGen::writeCallSeqStart(std::ostream& os) {

#ifdef LLVM_OLDER_THAN_5_0

  os << "def SDT_TCECallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i32>]>;"
     << std::endl << std::endl
     << "def callseq_start : SDNode<\"ISD::CALLSEQ_START\", "
     << "SDT_TCECallSeqStart, [SDNPHasChain, SDNPOutGlue]>;" << std::endl
     << std::endl
     << "let Defs = [SP], Uses = [SP] in {" << std::endl
     << "def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt),"
     << "\"# ADJCALLSTACKDOWN $amt\","
     << "[(callseq_start timm:$amt)]>;}" << std::endl << std::endl;

#else

  os << "def SDT_TCECallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i32>,"
     << "SDTCisVT<1, i32> ]>;" << std::endl << std::endl
     << "def callseq_start : SDNode<\"ISD::CALLSEQ_START\", "
     << "SDT_TCECallSeqStart, [SDNPHasChain, SDNPOutGlue]>;" << std::endl
     << std::endl
     << "let Defs = [SP], Uses = [SP] in {" << std::endl
     << "def ADJCALLSTACKDOWN : Pseudo<(outs),"
     << "(ins i32imm:$amt1, i32imm:$amt2),"
     << "\"# ADJCALLSTACKDOWN $amt1, $amt2\","
     << "[(callseq_start timm:$amt1, timm:$amt2)]>;}"
     << std::endl << std::endl;

#endif
}



void TDGen::writeConstShiftPat(std::ostream& os,
                               const TCEString& nodeName,
                               const TCEString& opNameBase, int i) {

    TCEString opName = opNameBase; opName << i << "_32rr";
    if (opNames_.find(opName) != opNames_.end()) {
        os << "def : Pat<(i32 (" << nodeName
           << " R32IRegs:$val, (i32 " << i << "))), ("
           << opName << " R32IRegs:$val)>;" << std::endl;
    }
}


void TDGen::createConstShiftPatterns(std::ostream& os) {
    for (int i = 1; i < 32; i++) {
        writeConstShiftPat(os, "TCESRAConst", "SHR", i);
        writeConstShiftPat(os, "TCESRLConst", "SHRU", i);
        writeConstShiftPat(os, "TCESHLConst", "SHL", i);
    }
}

void TDGen::createBoolAndHalfLoadPatterns(std::ostream& os) {

    // TODO: what about true/false versions of these ops?

    TCEString load = littleEndian_ ? "LD8" : "LDQ";
    TCEString uload = littleEndian_ ? "LDU8" : "LDQU";
    TCEString wload = littleEndian_ ? "LD32" : "LDW";
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

    // if no 8-bit loads, create 32-bit loads for stack access but
    // no patterns for isel as only the stack is 32-bit aligned.
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

    os << "def " << halfLoad << "hr : InstTCE<(outs R32HFPRegs:$op2), "
       << "(ins MEMrr:$op1), \"\", [(set R32HFPRegs:$op2, "
       << "(load ADDRrr:$op1))]>;" << std::endl;

    os << "def " << halfLoad << "hi : InstTCE<(outs R32HFPRegs:$op2), "
       << "(ins MEMri:$op1), \"\", [(set R32HFPRegs:$op2, "
       << "(load ADDRri:$op1))]>;" << std::endl;

    opNames_[halfLoad + "hr"] = halfLoad;
    opNames_[halfLoad + "hi"] = halfLoad;
}
