/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
    mach_(mach), dregNum_(0), maxVectorSize_(1), highestLaneInt_(-1), highestLaneBool_(-1) {
    tempRegFiles_ = MachineConnectivityCheck::tempRegisterFiles(mach);
}

/**
 * Generates all files required to build a tce backend plugin
 * (excluding static plugin code included from include/llvm/TCE/).
 */
void
TDGen::generateBackend(std::string& path) throw (Exception) {    

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
TDGen::writeRegisterInfo(std::ostream& o) 
    throw (Exception) {

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
    o << "def lane0 : SubRegIndex;" << std::endl 
      << "def lane1 : SubRegIndex;" << std::endl
      << "def lane2 : SubRegIndex;" << std::endl
      << "def lane3 : SubRegIndex;" << std::endl
      << "def lane4 : SubRegIndex;" << std::endl
      << "def lane5 : SubRegIndex;" << std::endl
      << "def lane6 : SubRegIndex;" << std::endl
      << "def lane7 : SubRegIndex;" << std::endl << std::endl
        
      << "def subvector2_0 : SubRegIndex;" << std::endl
      << "def subvector2_2 : SubRegIndex;" << std::endl
      << "def subvector2_4 : SubRegIndex;" << std::endl
      << "def subvector2_6 : SubRegIndex;" << std::endl
      << "def subvector4_0 : SubRegIndex;" << std::endl
      << "def subvector4_4 : SubRegIndex;" << std::endl << std::endl;
    
    writeRegisterClasses(o);
   
    writeRARegisterInfo(o);
    write1bitRegisterInfo(o);
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
                    rg->registerFile()->name(), rg->registerIndex() };

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
            else if (width == 16) ri = &regs16bit_;
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
                RegInfo reg = {rf->name(), currentIdx};
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
              << "def " << ri->first << "Regs : RegisterClass<\"TCE\", [i1], 8, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")> {" << std::endl
              << " let Size=8;" << std::endl
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
    writeRegisterDef(
        o, regs32bit_[2], "KLUDGE_REGISTER", "R32", "", RESERVED);

    // -------------------------------------
    
    for (unsigned i = 3; i < regs32bit_.size(); i++) {
        std::string regName = "I" + Conversion::toString(i);
        writeRegisterDef(o, regs32bit_[i], regName, "R32", "", GPR);
    }

    o << std::endl;

    // Register classes for all 32-bit registers.
    // TODO: why are these needed? same as integer classes below?
    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 1-bit RF classes
        if (ri->first.find("R32") == 0) {
            
            o << "def " << ri->first << "Regs : RegisterClass<\"TCE\", [i32,f32], 32, (add ";
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
    std::ostream& o, int width) {

    std::string vectorRegsAll;
    std::vector<std::string> vectorRegs(8);

    if (width <= maxVectorSize_) {
        for (unsigned i = 3; i < regs32bit_.size(); i++) {
            if (regs32bit_[i].rf.find("L_") == 0) {
                bool ok = true;
                unsigned int regIndex = regs32bit_[i].idx;
                std::vector<RegInfo> subRegs;
                TCEString vecRegRfName = 
                  "_VECTOR_" + Conversion::toString(width) + "_" + 
                  regs32bit_[i].rf;
                int laneIndex = regs32bit_[i].rf[2] - 48;

                TCEString aliasName;
                if (width == 2) {
                    aliasName = "I" + Conversion::toString(i);
                } else {
                    aliasName = "_VEC32_";
                    aliasName << (width >> 1) << "_" << i; //Conversion::toString(i);
                }
                for (int j = 1; j < width; j++) {
                    if (i+j >= regs32bit_.size()) {
                        ok = false;
                        break;
                    }                        
                    RegInfo& gprRegInfo = regs32bit_[i+j];
                    if (gprRegInfo.rf.find("L_") != 0 ||
                        gprRegInfo.idx != regIndex) {
                        ok = false;
                        break;
                    } else {
                        if (width == 2) {
                            TCEString aliasIndex = Conversion::toString(i+j);
                            aliasName+= ", I";
                            aliasName+= aliasIndex;
                        } else {
                            if (j == width >>1) {
                                aliasName <<" , _VEC32_" << (width >>1) << "_" << i+j;
                            }
                        }
                        vecRegRfName += "+" + gprRegInfo.rf;
                    }
                }
                if (ok) {
                    TCEString regName = "_VEC32_";
                    regName << width << "_" << Conversion::toString(i);
                    if (vectorRegsAll != "") {
                        vectorRegsAll += ", " + regName;
                    } else {
                        vectorRegsAll = regName;
                    }

                    if (vectorRegs[laneIndex] == "") {
                        vectorRegs[laneIndex] = regName;
                    } else {
                        vectorRegs[laneIndex] += ", " +regName;
                    }

                    RegInfo vecRegInfo = { vecRegRfName, regIndex };
                    o << "let SubRegIndices = [";
                    if (width == 2) {
                        o << "lane" << laneIndex << ", lane" << (laneIndex+1);
                    } else {
                        o << "subvector" << (width >> 1) << "_" << laneIndex << " , "
                          << "subvector" << (width >> 1) << "_" << laneIndex + (width>>1);
                    }
#ifdef LLVM_3_0
                    o << "] in {" << std::endl;
#else
                    o << "], CoveredBySubRegs = 1 in {" << std::endl;
#endif
                    writeRegisterDef(
                        o, vecRegInfo, regName, 
                        TCEString("V") + Conversion::toString(width) + "R32",
                        aliasName, GPR);

                    o << "}" << std::endl;

                    i+= (width-1);
                }
            }
        }
    }
    TCEString regClassBase("V"); regClassBase << width << "R32";

    bool hasAllVectorRegs = true;
    for (int i = 0; i < 8; i+=width) {
        if (vectorRegs[i] == "") {
            hasAllVectorRegs = false;
        }
    }

    // only low lane ones are really used for data(ie. first element in lane 0)
    if (!hasAllVectorRegs) {
        RegInfo reg = {TCEString("dummyvec") + Conversion::toString(width), 0};
        TCEString nameI("V"); nameI << width << "R32DUMMY" ;
        writeRegisterDef(o, reg, nameI, regClassBase, "", RESERVED);
    }

    for (int i = 0; i < 8; i+=width) {
        if (vectorRegs[i] == "") {
            o << "def " << regClassBase << "_L_" << i
              << "Regs : RegisterClass<\"TCE\", [v" << width
              << "i32, v" << width << "f32], " << 32 * width
              << ", (add V" << width << "R32DUMMY)> ;" << std::endl;
            
            o << "def " << regClassBase << "_L_" << i
              << "IRegs : RegisterClass<\"TCE\", [v" << width
              << "i32], " << 32 * width  << ", (add V" << width << "R32DUMMY)> ;"
              << std::endl;

            o << "def " << regClassBase << "_L_" << i
              << "FPRegs : RegisterClass<\"TCE\", [v" << width
              << "f32], " << 32 * width  << ", (add V" << width << "R32DUMMY)> ;"
              << std::endl << std::endl;
        } else {
            o << "def " << regClassBase << "_L_" << i
              << "Regs : RegisterClass<\"TCE\", [v" << width
              << "i32, v" << width << "f32], " << 32 * width
              << ", (add " << vectorRegs[i] << ")>;" << std::endl;

            o << "def " << regClassBase << "_L_" << i
              << "IRegs : RegisterClass<\"TCE\", [v" << width
              << "i32], " << 32 * width << ", (add " << vectorRegs[i] << ")>;"
              << std::endl;

            o << "def " << regClassBase << "_L_" << i
              << "FPRegs : RegisterClass<\"TCE\", [v" << width
              << "f32], " << 32 * width << ", (add " << vectorRegs[i] << ")>;"
              << std::endl << std::endl;
        }
    }

    // add class of all vector lanes, also ones whose lanes don't match
    // for example 2-wide vector consisting of lanes 2 and 3.
    if (vectorRegsAll == "") {
        // dummy already written, no need to write again.
        o << "def " << regClassBase << "Regs : RegisterClass<\"TCE\", [v" << width
          << "i32, v" << width << "f32], " << 32 * width
          << ", (add V" << width << "R32DUMMY)> ;" << std::endl;

        o << "def " << regClassBase << "IRegs : RegisterClass<\"TCE\", [v" << width
          << "i32], " << 32 * width << ", (add V" << width
          << "R32DUMMY)> ;" << std::endl;

        o << "def " << regClassBase << "FPRegs : RegisterClass<\"TCE\", [v" << width
          << "f32], " << 32 * width << ", (add V" << width << "R32DUMMY)> ;"
          << std::endl << std::endl;
    } 
    else {
        o << "def " << regClassBase << "Regs : RegisterClass<\"TCE\", [v" << width
          << "i32, v" << width << "f32], " << 32 * width << ", (add "
          << vectorRegsAll << ")> ;" << std::endl;

        o << "def " << regClassBase << "IRegs : RegisterClass<\"TCE\", [v" << width
          << "i32], " << 32 * width << ", (add "
          << vectorRegsAll << ")> ;" << std::endl;

        o << "def " << regClassBase << "FPRegs : RegisterClass<\"TCE\", [v" << width
          << "f32], " << 32 * width << ", (add "
          << vectorRegsAll << ")> ;" << std::endl << std::endl;
    }
}

/**
 * Writes register definitions for vector registers consisting from
 * multiple ordinar register files to the output stream.
 * @TODO: currently only support 2-sized registers.
 */
void
TDGen::writeVectorRegisterInfo(std::ostream& o) {
    std::vector<const TTAMachine::RegisterFile*> vectorRFs;

    const TTAMachine::Machine::RegisterFileNavigator nav =
        mach_.registerFileNavigator();

    for (int i = 0; i < nav.count(); i++) {
        const TTAMachine::RegisterFile* rf = nav.item(i);
        
        if (rf->name().find("L_") == 0 && rf->width() == 32) {
            vectorRFs.push_back(rf);
        }
    }

    maxVectorSize_ = MathTools::roundDownToPowerTwo(vectorRFs.size());
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
TDGen::checkRequiredRegisters() 
    throw (Exception) {

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

    OperationDAGSelector::OperationSet opNames;
    OperationDAGSelector::OperationSet requiredOps =
        LLVMBackend::llvmRequiredOpset();

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

    // add the floating point load and store patterns first so they will
    // be selected instead of i32 ldw/stw to avoid dummy castings
    os << "def LDWfr : InstTCE<(outs R32FPRegs:$op2), " 
       << "(ins MEMrr:$op1), \"\", [(set R32FPRegs:$op2, " 
       << "(load ADDRrr:$op1))]>;" << std::endl;

    os << "def LDWfi : InstTCE<(outs R32FPRegs:$op2), " 
       << "(ins MEMri:$op1), \"\", [(set R32FPRegs:$op2, " 
       << "(load ADDRri:$op1))]>;" << std::endl;

    os << "def STWfr : InstTCE<(outs), " 
       << "(ins MEMrr:$op1, R32FPRegs:$op2), \"\"," 
       << "[(store R32FPRegs:$op2, ADDRrr:$op1)]>;" << std::endl;

    os << "def STWfi : InstTCE<(outs), " 
       << "(ins MEMri:$op1, R32FPRegs:$op2), \"\"," 
       << "[(store R32FPRegs:$op2, ADDRri:$op1)]>;" << std::endl;

    opNames_["LDWfr"] = "LDW";
    opNames_["LDWfi"] = "LDW";
    opNames_["STWfr"] = "STW";
    opNames_["STWfi"] = "STW";


    OperationDAGSelector::OperationSet::const_iterator iter = opNames.begin();
    for (; iter != opNames.end(); iter++) {
        OperationDAGSelector::OperationSet::iterator r = 
            requiredOps.find(*iter);
        if (r != requiredOps.end()) {
            requiredOps.erase(r);
        }
        Operation& op = opPool.operation((*iter).c_str());

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

        if (op.name() == "LDW2" || op.name() == "LDW4" || op.name() == "LDW8") {
            int vectorWidth = Conversion::toInt(op.name().substr(3));
            // vector store
            writeVectorLoadDefs(os, op, vectorWidth);
            continue;
        }
        
        // TODO: Allow multioutput (remove last or)
        if (!operationCanBeMatched(op)) {
            
            if (&op != &NullOperation::instance()) {
                if (Application::verboseLevel() > 0) {
                    Application::logStream() 
                        << "Skipped writing operation definition for " 
                        << op.name() << std::endl;
                }
            }
            continue;
        }
        
        // TODO: remove this. For now MIMO operation patterns are not 
        // supported by tablegen.
        if (op.numberOfOutputs() > 1) {
            continue;
        }

        writeOperationDefs(os, op);
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
	    if (opName == "sqrtf") hasSQRTF = true;
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
      << "int GeneratedTCEPlugin::maxVectorSize() const { return "
      << maxVectorSize_ << "; }" << std::endl;

    generateLoadStoreCopyGenerator(o);
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
   o << "include \"TCECallingConv.td\"" << std::endl;
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
 * h
 * i = Immediate integer
 * j = immediate boolean
 * k = immediate float?
 * l
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
    Operation& op) {

    std::string attrs;

    // These white listed operations have mayLoad/mayStore flag
    // inferred from the llvm pattern and declaring it
    // explicitly will display warning in tablegen.
    if (op.name() != "LDQ" && op.name() != "LDQU" &&
        op.name() != "LDH" && op.name() != "LDHU" &&
        op.name() != "LDW" && op.name() != "LDD" &&
        op.name() != "STQ" && op.name() != "STH" &&
        op.name() != "STW" && op.name() != "STD" &&
        op.name() != "ALDQ" && op.name() != "ALDQU" &&
        op.name() != "ALDH" && op.name() != "ALDHU" &&
        op.name() != "ALDW" && op.name() != "ALDD" &&
        op.name() != "ASTQ" && op.name() != "ASTH" &&
        op.name() != "ASTW" && op.name() != "ASTD") {

        if (op.readsMemory()) attrs += " mayLoad = 1";
        if (op.writesMemory()) attrs += " mayStore = 1";
    }

    // no bool outs for some operatios
    if (op.name() == "CFI" || op.name() == "CFIU") {
        writeOperationDef(o, op, "rf", attrs);
        writeOperationDef(o, op, "vm", attrs, "_VECTOR_2_");
        writeOperationDef(o, op, "wn", attrs, "_VECTOR_4_");
        writeOperationDef(o, op, "xo", attrs, "_VECTOR_8_");

        return;
    }
        
    // rotations are allways n x n -> n bits.
    if (op.name() == "ROTL" || op.name() == "ROTR" ||
        op.name() == "SHL" || op.name() == "SHR" || op.name() == "SHRU") {
        writeOperationDefs(o, op, "rrr", attrs);

        writeOperationDef(o, op, "vvv", attrs, "_VECTOR_2_");
        writeOperationDef(o, op, "www", attrs, "_VECTOR_4_");
        writeOperationDef(o, op, "xxx", attrs, "_VECTOR_8_");
        return;
    }

    if (op.name() == "SXHW" || op.name() == "SXQW") {
        writeOperationDef(o, op, "rr", attrs);
        writeOperationDef(o, op, "vv", attrs, "_VECTOR_2_");
        writeOperationDef(o, op, "ww", attrs, "_VECTOR_4_");
        writeOperationDef(o, op, "xx", attrs, "_VECTOR_8_");
        return;
    }

    // these can have 1-bit inputs
    if (op.name() == "XOR" || op.name() == "IOR" || op.name() == "AND" ||
        op.name() == "ANDN" || op.name() == "ADD" || op.name() == "SUB") {
        
        writeOperationDefs(o, op, "bbb", attrs);
    }

    // store likes this. store immediate to immediate address
    if (op.numberOfInputs() == 2 && op.numberOfOutputs() == 0) {
        Operand& operand1 = op.operand(1);
        Operand& operand2 = op.operand(2);
        // TODO: add an else branch here for float immediates
        if ((operand1.type() == Operand::UINT_WORD || 
             operand1.type() == Operand::SINT_WORD) &&
           (operand2.type() == Operand::UINT_WORD || 
            operand2.type() == Operand::SINT_WORD)) {
            writeOperationDef(o, op, "ii", attrs);
        }
    }

    std::string operandTypes = createDefaultOperandTypeString(op);
    // this the ordinary def

    // then try with immediates.
    // TODO: this should be 2^n loop instead of n loop, to get
    // all permutations.

    writeOperationDefs(o, op, operandTypes, attrs);

    // then with boolean outs, and vector versions.
    if (op.numberOfOutputs() == 1 && !op.readsMemory()) {
        Operand& outOperand = op.operand(op.numberOfInputs()+1);
        if (outOperand.type() == Operand::UINT_WORD || 
            outOperand.type() == Operand::SINT_WORD) {

            // 32  to 1-bit operations
            operandTypes[0] = 'b';
            writeOperationDefs(o, op, operandTypes, attrs);
        } 
        
        if (createDefaultOperandTypeString(op) == "rrr") {
            writeOperationDef(o, op, "vvv", attrs, "_VECTOR_2_");
            writeOperationDef(o, op, "www", attrs, "_VECTOR_4_");
            writeOperationDef(o, op, "xxx", attrs, "_VECTOR_8_");
        }
        // TODO.. this may be problematic, same reg class for vec and normal?
        if (createDefaultOperandTypeString(op) == "fff") {
            writeOperationDef(o, op, "mmm", attrs, "_VECTOR_2_");            
            writeOperationDef(o, op, "nnn", attrs, "_VECTOR_4_");
            writeOperationDef(o, op, "ooo", attrs, "_VECTOR_8_");
        }

        // int-to-float conversions. also vector versions of those
        if (createDefaultOperandTypeString(op) == "fr") {
            writeOperationDef(o, op, "mv", attrs, "_VECTOR_2_");            
            writeOperationDef(o, op, "nw", attrs, "_VECTOR_4_");
            writeOperationDef(o, op, "ox", attrs, "_VECTOR_8_");
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
    const std::string& attrs, std::string backendPrefix) {

    // first without imms.
    writeOperationDef(o, op, operandTypes, attrs, backendPrefix);

    for (int i = 0; i < op.numberOfInputs(); i++) {
        bool canSwap = false;
        for (int j = i+1 ; j < op.numberOfInputs(); j++) {
            if (op.canSwap(i+1, j+1)) {
                canSwap = true;
                break;
            }
        }
        if (!canSwap) {
            std::string opTypes = operandTypes;
            char& c = opTypes[i+op.numberOfOutputs()];
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
            default:
                continue;
            }
            writeOperationDef(o, op, opTypes, attrs, backendPrefix);
        }
    }
}

void 
TDGen::writeVectorStoreDefs(
    std::ostream& o,
    Operation& op, int vectorLen) {

    o << "def STW" << vectorLen << "vr : InstTCE<(outs), (ins MEMrr:$addr, V" << vectorLen << "R32IRegs:$data),"
      << "\"\", [(store V" << vectorLen << "R32IRegs:$data, ADDRrr:$addr)]>;" << std::endl;

    o << "def STW" << vectorLen << "vi : InstTCE<(outs), (ins MEMri:$addr, V" << vectorLen << "R32IRegs:$data),"
      << "\"\", [(store V" << vectorLen << "R32IRegs:$data, ADDRri:$addr)]>;" << std::endl;

    o << "def STW" << vectorLen << "mr : InstTCE<(outs), (ins MEMrr:$addr, V" << vectorLen << "R32FPRegs:$data),"
      << "\"\", [(store V" << vectorLen << "R32FPRegs:$data, ADDRrr:$addr)]>;" << std::endl;

    o << "def STW" << vectorLen << "mi : InstTCE<(outs), (ins MEMri:$addr, V" << vectorLen << "R32FPRegs:$data),"
      << "\"\", [(store V" << vectorLen << "R32FPRegs:$data, ADDRri:$addr)]>;" << std::endl;

    opNames_[op.name() + "vr"] = op.name();
    opNames_[op.name() + "vi"] = op.name();
    opNames_[op.name() + "mr"] = op.name();
    opNames_[op.name() + "mi"] = op.name();

}

void 
TDGen::writeVectorLoadDefs(
    std::ostream& o,
    Operation& op, int vectorLen) {

    o << "def LDW" << vectorLen << "vr : InstTCE<(outs V" << vectorLen << "R32IRegs:$data), (ins MEMrr:$addr),"
      << "\"\", [(set V" << vectorLen << "R32IRegs:$data, (load ADDRrr:$addr))]>;" << std::endl;

    o << "def LDW" << vectorLen << "vi : InstTCE<(outs V" << vectorLen << "R32IRegs:$data), (ins MEMri:$addr),"
      << "\"\", [(set V" << vectorLen << "R32IRegs:$data, (load ADDRri:$addr))]>;" << std::endl;

    o << "def LDW" << vectorLen << "mr : InstTCE<(outs V" << vectorLen << "R32FPRegs:$data), (ins MEMrr:$addr),"
      << "\"\", [(set V" << vectorLen << "R32FPRegs:$data, (load ADDRrr:$addr))]>;" << std::endl;

    o << "def LDW" << vectorLen << "mi : InstTCE<(outs V" << vectorLen << "R32FPRegs:$data), (ins MEMri:$addr),"
      << "\"\", [(set V" << vectorLen << "R32FPRegs:$data, (load ADDRri:$addr))]>;" << std::endl;

    opNames_[op.name() + "vr"] = op.name();
    opNames_[op.name() + "vi"] = op.name();
    opNames_[op.name() + "mr"] = op.name();
    opNames_[op.name() + "mi"] = op.name();


//def LDW2r : InstTCE<(outs V2Regs:$op2), (ins MEMrr:$op1), "", [(set V2Regs:$op2, (load ADDRrr:$op1))]>;

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
    std::string backendPrefix) {
    assert (operandTypes.size() > 0);

    
    std::string outputs, inputs, asmstr, pattern;
    outputs = "(outs" + patOutputs(op, operandTypes) + ")";
    inputs = "(ins " + patInputs(op, operandTypes) + ")";

    asmstr = "\"\"";
    
    if (llvmOperationPattern(op.name(),'r') != "" || 
        op.dagCount() == 0) {
        OperationDAG* trivial = createTrivialDAG(op);
        pattern = operationPattern(op, *trivial, operandTypes);
        delete trivial;
    } else {
        pattern = operationPattern(op, op.dag(0), operandTypes);
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
    
    if (attrs != "") {
        o << "}" << std::endl;
    }        
    opNames_[opcEnum] = backendPrefix + op.name();
}

/**
 * Checks whether operand is integer or float type.
 *
 * @return 'r' for integer, 'f' for float
 */
char 
TDGen::operandChar(Operand& operand) {
    if (operand.type() != Operand::UINT_WORD &&
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
        // commutative op imms only once
        bool canSwap = false;
        if (immInput > 0) {
            for (int j = immInput + 1 ; j <= op.numberOfInputs(); j++) {
                if (op.canSwap(immInput, j)) {
                    canSwap = true;
                    break;
                }
            }
        }
        if (canSwap) {
            continue;
        }

        bool ok = true;
        std::string llvmPat = llvmOperationPattern(op.name(), 'r');
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
                if (inputType == 'f') {
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
 * @param osalOperationName Base-operation name in OSAL.
 * @return Boost::format string of the operation node in llvm.
 */
std::string
TDGen::llvmOperationPattern(const std::string& osalOperationName,
    char operandType) {

    const std::string opName = StringTools::stringToLower(osalOperationName);

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
    if (opName == "cfh") return "fp32_to_fp16 %1%";
    if (opName == "chf") return "fp16_to_fp32 %1%";

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

    if (opName == "neg") return "ineg %1%";
    if (opName == "not") return "not %1%";

    // Unknown operation name.
    return "";
}

/**
 * Returns llvm operation name for the given OSAL operation name,
 * if any.
 */
std::string
TDGen::llvmOperationName(const std::string& osalOperationName) {

    const std::string opName = StringTools::stringToLower(osalOperationName);

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
    if (opName == "cfh") return "fp32_to_fp16";
    if (opName == "chf") return "fp16_to_fp32";


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

    if (opName == "sxhw") return "sext_inreg";
    if (opName == "sxqw") return "sext_inreg";

    if (opName == "neg") return "ineg";
    if (opName == "not") return "not";

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
    if (llvmOperationPattern(op.name(),'r') != "") {
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
    const Operation& op,
    const OperationDAG& dag,
    const OperationDAGNode& node,
    bool emulationPattern, 
    const std::string& operandTypes)
    throw (InvalidData) {

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
        assert(dag.inDegree(*cNode) == 0);
        return Conversion::toString(cNode->value());
    }

    abortWithError("Unknown OperationDAG node type.");
    return "";
}

/**
 * Returns an llvm name for an operation node in a emulation dag.
 *
 * @param op the operation being emulated.
 * @param dag dag of the emulated operation
 * @param node node whose name is being asked
 * @param operandTypes string containing oeprand types for the emulated op.
 */
std::string
TDGen::emulatingOpNodeLLVMName(
    const Operation& op,
    const OperationDAG& dag, 
    const OperationNode& node,
    const std::string& operandTypes)
    throw (InvalidData) {
    
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
                    assert(operandTypes.length() > strIndex &&
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
                        assert(operandTypes.length() > strIndex &&
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
    const Operation& op,
    const OperationDAG& dag,
    const OperationNode& node,
    bool emulationPattern,
    const std::string& operandTypes)
    throw (InvalidData) {

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
        operationPat = llvmOperationPattern(operation.name(), operandTypes[0]);
        
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
            throw (InvalidData(__FILE__, __LINE__, __func__, msg));
        }
    } else if (operand.type() == Operand::SINT_WORD ||
               operand.type() == Operand::UINT_WORD) {

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
        default:
            std::string msg = 
                "invalid operation type for integer operand:";
            msg += operandType;
            throw (InvalidData(__FILE__, __LINE__, __func__, msg));
        }
    } else if (operand.type() == Operand::FLOAT_WORD ||
              operand.type() == Operand::HALF_FLOAT_WORD) {

        // TODO: half float should have it's own branch here and 
        // own registers?

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
            throw (InvalidData(__FILE__, __LINE__, __func__, msg));
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
 * @param immOp Index for an operand that should be defined as an immediate.
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

    OperationDAG* dag = new OperationDAG(op.name());
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


    os << "#include <stdio.h>" << std::endl 
       << "int GeneratedTCEPlugin::getStore(const TargetRegisterClass *rc)"
       << " const {" << std::endl;
    
    os << "\tif (rc == TCE::RARegRegisterClass) return TCE::STWRArr;"
       << std::endl;

    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        if (ri->first.find("R1") == 0) {
            os << "\tif (rc == TCE::" << ri->first
               << "RegsRegisterClass) return TCE::STQBrb;" << std::endl;
        }
        if (ri->first.find("R32") == 0) {
            os << "\tif (rc == TCE::" << ri->first
               << "RegsRegisterClass) return TCE::STWrr;" << std::endl;
            
            os << "\tif (rc == TCE::"  << ri->first
               << "IRegsRegisterClass) return TCE::STWrr;" << std::endl;
            
            os << "\tif (rc == TCE::"  << ri->first
               << "FPRegsRegisterClass) return TCE::STWfr;" << std::endl;
        }
    }
    
    if (opNames_.find("STW2vr") != opNames_.end()) {
        os << "\tif (rc == TCE::V2R32IRegsRegisterClass) return TCE::STW2vr;"
           << std::endl
           << "\tif (rc == TCE::V2R32FPRegsRegisterClass) return TCE::STW2mr;"
           << std::endl;

        os << "\tif (rc == TCE::V2R32_L_0IRegsRegisterClass) return TCE::STW2vr;"
           << std::endl
           << "\tif (rc == TCE::V2R32_L_0FPRegsRegisterClass) return TCE::STW2mr;"
           << std::endl;

        os << "\tif (rc == TCE::V2R32_L_2IRegsRegisterClass) return TCE::STW2vr;"
           << std::endl
           << "\tif (rc == TCE::V2R32_L_2FPRegsRegisterClass) return TCE::STW2mr;"
           << std::endl;

        os << "\tif (rc == TCE::V2R32_L_4IRegsRegisterClass) return TCE::STW2vr;"
           << std::endl
           << "\tif (rc == TCE::V2R32_L_4FPRegsRegisterClass) return TCE::STW2mr;"
           << std::endl;

        os << "\tif (rc == TCE::V2R32_L_6IRegsRegisterClass) return TCE::STW2vr;"
           << std::endl
           << "\tif (rc == TCE::V2R32_L_6FPRegsRegisterClass) return TCE::STW2mr;"
           << std::endl;
    }
    if (opNames_.find("STW4vr") != opNames_.end()) {
        os << "\tif (rc == TCE::V4R32IRegsRegisterClass) return TCE::STW4vr;"
           << std::endl
           << "\tif (rc == TCE::V4R32FPRegsRegisterClass) return TCE::STW4mr;"
           << std::endl;

        os << "\tif (rc == TCE::V4R32_L_0IRegsRegisterClass) return TCE::STW4vr;"
           << std::endl
           << "\tif (rc == TCE::V4R32_L_0FPRegsRegisterClass) return TCE::STW4mr;"
           << std::endl;

        os << "\tif (rc == TCE::V4R32_L_4IRegsRegisterClass) return TCE::STW4vr;"
           << std::endl
           << "\tif (rc == TCE::V4R32_L_4FPRegsRegisterClass) return TCE::STW4mr;"
           << std::endl;
    }
    if (opNames_.find("STW8vr") != opNames_.end()) {
        os << "\tif (rc == TCE::V8R32IRegsRegisterClass) return TCE::STW8vr;"
           << std::endl
           << "\tif (rc == TCE::V8R32FPRegsRegisterClass) return TCE::STW8mr;"
           << std::endl;

        os << "\tif (rc == TCE::V8R32_L_0IRegsRegisterClass) return TCE::STW8vr;"
           << std::endl
           << "\tif (rc == TCE::V8R32_L_0FPRegsRegisterClass) return TCE::STW8mr;"
           << std::endl;
    }
    os  << "\tprintf(\"regclass: %s\\n\", rc->getName());" << std::endl
        << "\tassert(0&&\"Storing given regclass to stack not supported. "
        << "Bug in backend?\");"
        << std::endl
        << "} " << std::endl
        << std::endl
              
        << "int GeneratedTCEPlugin::getLoad(const TargetRegisterClass *rc)"
        << " const {" << std::endl;

    os << "\tif (rc == TCE::RARegRegisterClass) return TCE::LDWRAr;"
       << std::endl;

    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        if (ri->first.find("R1") == 0) {
            os << "\tif (rc == TCE::" << ri->first
               << "RegsRegisterClass) return TCE::LDQBr;" << std::endl;
        }
        if (ri->first.find("R32") == 0) {
            os << "\tif (rc == TCE::" << ri->first
               << "RegsRegisterClass) return TCE::LDWrr;" << std::endl;
            
            os << "\tif (rc == TCE::" << ri->first
               << "IRegsRegisterClass) return TCE::LDWrr;" << std::endl;
            
            os << "\tif (rc == TCE::" << ri->first
               << "FPRegsRegisterClass) return TCE::LDWfr;" << std::endl;
        }
    }

    if (opNames_.find("LDW2vr") != opNames_.end()) {
        os << "\tif (rc == TCE::V2R32IRegsRegisterClass) return TCE::LDW2vr;"
           << std::endl
           << "\tif (rc == TCE::V2R32FPRegsRegisterClass) return TCE::LDW2mr;"
           << std::endl;

        os << "\tif (rc == TCE::V2R32_L_0IRegsRegisterClass) return TCE::LDW2vr;"
           << std::endl
           << "\tif (rc == TCE::V2R32_L_0FPRegsRegisterClass) return TCE::LDW2mr;"
           << std::endl;

        os << "\tif (rc == TCE::V2R32_L_2IRegsRegisterClass) return TCE::LDW2vr;"
           << std::endl
           << "\tif (rc == TCE::V2R32_L_2FPRegsRegisterClass) return TCE::LDW2mr;"
           << std::endl;

        os << "\tif (rc == TCE::V2R32_L_4IRegsRegisterClass) return TCE::LDW2vr;"
           << std::endl
           << "\tif (rc == TCE::V2R32_L_4FPRegsRegisterClass) return TCE::LDW2mr;"
           << std::endl;

        os << "\tif (rc == TCE::V2R32_L_6IRegsRegisterClass) return TCE::LDW2vr;"
           << std::endl
           << "\tif (rc == TCE::V2R32_L_6FPRegsRegisterClass) return TCE::LDW2mr;"
           << std::endl;
    }
    if (opNames_.find("LDW4vr") != opNames_.end()) {
        os << "\tif (rc == TCE::V4R32IRegsRegisterClass) return TCE::LDW4vr;"
           << std::endl
           << "\tif (rc == TCE::V4R32FPRegsRegisterClass) return TCE::LDW4mr;"
           << std::endl;

        os << "\tif (rc == TCE::V4R32_L_0IRegsRegisterClass) return TCE::LDW4vr;"
           << std::endl
           << "\tif (rc == TCE::V4R32_L_0FPRegsRegisterClass) return TCE::LDW4mr;"
           << std::endl;

        os << "\tif (rc == TCE::V4R32_L_4IRegsRegisterClass) return TCE::LDW4vr;"
           << std::endl
           << "\tif (rc == TCE::V4R32_L_4FPRegsRegisterClass) return TCE::LDW4mr;"
           << std::endl;
    }
    if (opNames_.find("LDW8vr") != opNames_.end()) {
        os << "\tif (rc == TCE::V8R32IRegsRegisterClass) return TCE::LDW8vr;"
           << std::endl
           << "\tif (rc == TCE::V8R32FPRegsRegisterClass) return TCE::LDW8mr;"
           << std::endl;

        os << "\tif (rc == TCE::V8R32_L_0IRegsRegisterClass) return TCE::LDW8vr;"
           << std::endl
           << "\tif (rc == TCE::V8R32_L_0FPRegsRegisterClass) return TCE::LDW8mr;"
           << std::endl;
    }
    os  << "\tprintf(\"regclass: %s\\n\", rc->getName());" << std::endl
        << "\tassert(0&&\"loading from stack to given regclass not supported."
        << " Bug in backend?\");"
       << std::endl
       << "} " << std::endl
       << std::endl;
}
