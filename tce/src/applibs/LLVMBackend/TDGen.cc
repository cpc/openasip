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

#include "tce_config.h"
// SP, RES, KLUDGE, 2 GPRs?
unsigned const TDGen::REQUIRED_I32_REGS = 5;


/**
 * Constructor.
 *
 * @param mach Machine to generate plugin for.
 */
TDGen::TDGen(const TTAMachine::Machine& mach) :
    mach_(mach), dregNum_(0) {
    tempRegFiles_ = MachineConnectivityCheck::tempRegisterFiles(mach);
}

/**
 * Generates all files required to build a tce backend plugin
 * (excluding static plugin code included from include/llvm/TCE/).
 */
void
TDGen::generateBackend(std::string& path) throw (Exception) {    

    std::ofstream callTD;
    callTD.open((path + "/GenCallSeq.td").c_str());
    writeCallSeq(callTD);
    callTD.close();

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

    o << "def " << regName << " : " << regTemplate
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

    o << "class Ri1<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << std::endl;
    o << "}" << std::endl;

    o << "class Ri8<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << std::endl;
    o << "}" << std::endl;

    o << "class Ri16<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << std::endl;
    o << "}" << std::endl;

    o << "class Ri32<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << std::endl;
    o << "}" << std::endl;

    o << "class Rf32<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << std::endl;
    o << "}" << std::endl;

    o << "class Ri64<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << std::endl;
    o << "}" << std::endl;

    o << "class Rf64<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << std::endl;
    o << "}" << std::endl;

    o << std::endl;
   
    writeRARegisterInfo(o);
    write1bitRegisterInfo(o);
    write8bitRegisterInfo(o);
    write16bitRegisterInfo(o);
    write32bitRegisterInfo(o);
    write64bitRegisterInfo(o);

    return true;
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

    std::string i1regs;

    if (regs1bit_.size() < 1) {
        RegInfo reg = {"dummy1", 0};
        std::string name = "I1DUMMY";
        writeRegisterDef(o, reg, name, "Ri1", "", RESERVED);
        i1regs += name;
    } else {
        for (unsigned i = 0; i < regs1bit_.size(); i++) {
            std::string regName = "B" + Conversion::toString(i);
            if (i > 0) i1regs += ", ";
            i1regs += regName;
            writeRegisterDef(o, regs1bit_[i], regName, "Ri1", "", GPR);
        }
    }

/*    o << std::endl
      << "def I1Regs : RegisterClass<\"TCE\", [i1], 8, ["
      << i1regs << "]>;" << std::endl;

*/
#ifdef LLVM_2_9
    o << std::endl
      << "def I1Regs : RegisterClass<\"TCE\", [i1], 8, ["
      << i1regs << "]> {" << std::endl
      << " let Size=8;" << std::endl
      << "}" << std::endl;
#else
    o << std::endl
      << "def I1Regs : RegisterClass<\"TCE\", [i1], 8, (add "
      << i1regs << ")> {" << std::endl
      << " let Size=8;" << std::endl
      << "}" << std::endl;
#endif
}


/**
 * Writes 8-bit register definitions to the output stream.
 */
void
TDGen::write8bitRegisterInfo(std::ostream& o) {

    std::string i8regs;
    if (regs8bit_.size() < 1) {
        RegInfo reg = { "dummy8", 0 };
        std::string name = "I8DUMMY";
        writeRegisterDef(o, reg, name, "Ri8", "", RESERVED);
        i8regs += name;
    } else {
        for (unsigned i = 0; i < regs8bit_.size(); i++) {
            std::string regName = "Q" + Conversion::toString(i);
            if (i > 0) i8regs += ", ";
            i8regs += regName;
            writeRegisterDef(o, regs8bit_[i], regName, "Ri8", "", GPR);
        }
    }

#ifdef LLVM_2_9
    o << std::endl
       << "def I8Regs : RegisterClass<\"TCE\", [i8], 8, ["
       << i8regs << "]>;" << std::endl
       << std::endl << std::endl << std::endl;
#else
    o << std::endl
       << "def I8Regs : RegisterClass<\"TCE\", [i8], 8, (add "
       << i8regs << ")>;" << std::endl
       << std::endl << std::endl << std::endl;
#endif
}


/**
 * Writes 16-bit register definitions to the output stream.
 */
void
TDGen::write16bitRegisterInfo(std::ostream& o) {

    std::string i16regs;
    if (regs16bit_.size() < 1) {
        RegInfo reg = { "dummy16", 0 };
        std::string name = "I16DUMMY";
        writeRegisterDef(o, reg, name, "Ri16", "", RESERVED);
        i16regs += name;
    } else {
        for (unsigned i = 0; i < regs16bit_.size(); i++) {
            std::string regName = "H" + Conversion::toString(i);
            if (i > 0) i16regs += ", ";
            i16regs += regName;
            writeRegisterDef(o, regs16bit_[i], regName, "Ri16", "", GPR);
        }
    }

#ifdef LLVM_2_9
    o << std::endl
       << "def I16Regs : RegisterClass<\"TCE\", [i16], 16, ["
       << i16regs << "]>;" << std::endl
       << std::endl;
#else
    o << std::endl
       << "def I16Regs : RegisterClass<\"TCE\", [i16], 16, (add "
       << i16regs << ")>;" << std::endl
       << std::endl;
#endif
}


/**
 * Writes 32-bit register definitions to the output stream.
 */
void
TDGen::write32bitRegisterInfo(std::ostream& o) {

    // --- Hardcoded reserved registers. ---
    writeRegisterDef(o, regs32bit_[0], "SP", "Ri32", "", RESERVED);
    writeRegisterDef(o, regs32bit_[1], "IRES0", "Ri32", "", RESULT);
    writeRegisterDef(
        o, regs32bit_[2], "KLUDGE_REGISTER", "Ri32", "", RESERVED);

    // -------------------------------------
    
    std::string i32regs;
    for (unsigned i = 3; i < regs32bit_.size(); i++) {
        std::string regName = "I" + Conversion::toString(i);
        i32regs += regName + ", ";
        writeRegisterDef(o, regs32bit_[i], regName, "Ri32", "", GPR);
    }

#ifdef LLVM_2_9
    o << std::endl
      << "def I32Regs : RegisterClass<\"TCE\", [i32], 32, ["
      << i32regs << std::endl
      << "SP, IRES0, KLUDGE_REGISTER]> {"
      << std::endl;
    
    o << " let MethodProtos = [{" << std::endl
      << "  iterator allocation_order_end(const MachineFunction& mf) const;"
      << std::endl
      << " }];" << std::endl
      << " let MethodBodies = [{" << std::endl
      << "  I32RegsClass::iterator" << std::endl
      << "  I32RegsClass::allocation_order_end("
      << "   const MachineFunction& mf) const {" << std::endl
      << "   return end()-3; // Don't allocate special regs." << std::endl
      << "  }" << std::endl
      << " }];" << std::endl
      << "}" << std::endl;

#else    
    o << std::endl
      << "def I32Regs : RegisterClass<\"TCE\", [i32], 32, (add "
      << i32regs << std::endl
      << "SP, IRES0, KLUDGE_REGISTER)> ;"
      << std::endl;
#endif
    
    // --- Hardcoded reserved registers. ---
    writeRegisterDef(o, regs32bit_[0], "FSP", "Rf32", "SP", RESERVED);
    writeRegisterDef(o, regs32bit_[1], "FRES0", "Rf32", "IRES0", RESULT);
    writeRegisterDef(o, regs32bit_[2], "FKLUDGE", "Rf32", "KLUDGE_REGISTER",
                     RESERVED);
    // -------------------------------------
    std::string f32regs;
    for (unsigned i = 3; i < regs32bit_.size(); i++) {
        std::string regName = "F" + Conversion::toString(i);
        std::string aliasName = "I" + Conversion::toString(i);
        f32regs += regName + ", ";
        writeRegisterDef(o, regs32bit_[i], regName, "Rf32", aliasName, GPR);
    }

#ifdef LLVM_2_9
    o << std::endl
      << "def F32Regs : RegisterClass<\"TCE\", [f32], 32, ["
      << f32regs << std::endl
      << "FSP, FRES0, FKLUDGE]> {" << std::endl;
    
    o << " let MethodProtos = [{" << std::endl
      << "  iterator allocation_order_end(const MachineFunction& mf) const;"
      << std::endl
      << " }];" << std::endl
      << " let MethodBodies = [{" << std::endl
      << "  F32RegsClass::iterator" << std::endl
      << "  F32RegsClass::allocation_order_end("
      << "  const MachineFunction& mf) const {" << std::endl
      << "   return end()-3; // Don't allocate special registers." << std::endl
      << "  }"<< std::endl
      << " }];" << std::endl
      << "}" << std::endl;

#else
    o << std::endl
      << "def F32Regs : RegisterClass<\"TCE\", [f32], 32, (add "
      << f32regs << std::endl
      << "FSP, FRES0, FKLUDGE)> ;" << std::endl;
#endif

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

#ifdef LLVM_2_9
    o << std::endl
      << "def I64Regs : RegisterClass<\"TCE\", [i64], 32, [" // DIRES
      << i64regs << "]> {"
      << std::endl;

    o << " let MethodProtos = [{" << std::endl
      << "  iterator allocation_order_end(const MachineFunction& mf) const;"
      << std::endl
      << " }];" << std::endl
      << " let MethodBodies = [{" << std::endl
      << "  I64RegsClass::iterator" << std::endl
      << "  I64RegsClass::allocation_order_end("
      << "  const MachineFunction& mf) const {" << std::endl
      << "   return end()-1; // Don't allocate special registers." << std::endl
      << "  }"<< std::endl
      << " }];" << std::endl
      << "}" << std::endl;
#else // LLVM-3.x
    o << std::endl
      << "def I64Regs : RegisterClass<\"TCE\", [i64], 32, (add " // DIRES
      << i64regs << ")> ;"
      << std::endl;
#endif
    std::string f64regs;

    if (regs64bit_.size() < 1) {
        RegInfo reg = { "dummy64", 0 };
        writeRegisterDef(o, reg, "DRES0", "Rf64", "", RESERVED);
        f64regs = "DRES0";
    } else {
        writeRegisterDef(o, regs64bit_[0], "DRES0", "Ri64", "DIRES0", RESERVED);
        for (unsigned i = 1; i < regs64bit_.size(); i++) {
            std::string regName = "D" + Conversion::toString(i);
            std::string aliasName = "DI" + Conversion::toString(i);
            f64regs += regName;
            f64regs += ", ";
            writeRegisterDef(o, regs64bit_[i], regName, "Rf64", aliasName, GPR);
        }
        f64regs += "DRES0";
    }
#ifdef LLVM_2_9
    o << std::endl
      << "def F64Regs : RegisterClass<\"TCE\", [f64], 32, [" // DRES
      << f64regs << "]>{" << std::endl;

    o << " let MethodProtos = [{" << std::endl
      << "  iterator allocation_order_end(const MachineFunction& mf) const;"
      << std::endl
      << " }];" << std::endl
      << " let MethodBodies = [{" << std::endl
      << "  F64RegsClass::iterator" << std::endl
      << "  F64RegsClass::allocation_order_end("
      << "  const MachineFunction& mf) const {" << std::endl
      << "   return end()-1; // Don't allocate special registers." << std::endl
      << "  }"<< std::endl
      << " }];" << std::endl
      << "}" << std::endl;
#else
    o << std::endl
      << "def F64Regs : RegisterClass<\"TCE\", [f64], 32, (add " // DRES
      << f64regs << ")>;" << std::endl;
#endif
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
#ifdef LLVM_2_9
    o << "def RAReg : RegisterClass<\"TCE\", [i32], 32, [RA]>;" << std::endl;
#else
    o << "def RAReg : RegisterClass<\"TCE\", [i32], 32, (add RA)>;" << 
	std::endl;
#endif
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
    os << "def LDWfr : InstTCE<(outs F32Regs:$op2), " 
       << "(ins MEMrr:$op1), \"\", [(set F32Regs:$op2, " 
       << "(load ADDRrr:$op1))]>;" << std::endl;

    os << "def LDWfi : InstTCE<(outs F32Regs:$op2), " 
       << "(ins MEMri:$op1), \"\", [(set F32Regs:$op2, " 
       << "(load ADDRri:$op1))]>;" << std::endl;

    os << "def STWfrr : InstTCE<(outs), " 
       << "(ins MEMrr:$op1, F32Regs:$op2), \"\"," 
       << "[(store F32Regs:$op2, ADDRrr:$op1)]>;" << std::endl;

    os << "def STWfir : InstTCE<(outs), " 
       << "(ins MEMri:$op1, F32Regs:$op2), \"\"," 
       << "[(store F32Regs:$op2, ADDRri:$op1)]>;" << std::endl;

    opNames_["LDWfr"] = "LDW";
    opNames_["LDWfi"] = "LDW";
    opNames_["STWfrr"] = "STW";
    opNames_["STWfir"] = "STW";


    OperationDAGSelector::OperationSet::const_iterator iter = opNames.begin();
    for (; iter != opNames.end(); iter++) {
        OperationDAGSelector::OperationSet::iterator r = 
            requiredOps.find(*iter);
        if (r != requiredOps.end()) {
            requiredOps.erase(r);
        }
        Operation& op = opPool.operation((*iter).c_str());
        
        
        // TODO: Allow multioutput (remove last or)
        if (&op == &NullOperation::instance() || 
            !operationCanBeMatched(op)) {
            
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

        writeOperationDef(os, op);
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
TDGen::writeCallSeq(std::ostream& o) {
#if (defined(LLVM_2_7) || defined(LLVM_2_8))
    o << 
"def callseq_start : SDNode<\"ISD::CALLSEQ_START\", SDT_TCECallSeqStart," <<
"                             [SDNPHasChain, SDNPOutFlag]>;" <<
"def callseq_end   : SDNode<\"ISD::CALLSEQ_END\",   SDT_TCECallSeqEnd," <<
"                           [SDNPHasChain, SDNPOptInFlag, SDNPOutFlag]>;" <<
"def SDT_TCECall    : SDTypeProfile<0, 1, [SDTCisVT<0, i32>]>;" <<
"def call          : SDNode<\"TCEISD::CALL\", SDT_TCECall," <<
"                  [SDNPHasChain, SDNPOptInFlag, SDNPOutFlag]>;" <<
"def retflag       : SDNode<\"TCEISD::RET_FLAG\", SDTNone," <<
"                           [SDNPHasChain, SDNPOptInFlag]>; " <<
 std::endl;
#else
    o << 
"def callseq_start : SDNode<\"ISD::CALLSEQ_START\", SDT_TCECallSeqStart," <<
"                           [SDNPHasChain, SDNPOutGlue]>;" <<
"def callseq_end   : SDNode<\"ISD::CALLSEQ_END\",   SDT_TCECallSeqEnd," <<
"                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;" <<
"def SDT_TCECall    : SDTypeProfile<0, 1, [SDTCisVT<0, i32>]>;" <<
"def call          : SDNode<\"TCEISD::CALL\", SDT_TCECall," <<
"                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;" <<
"def retflag       : SDNode<\"TCEISD::RET_FLAG\", SDTNone," <<
"                           [SDNPHasChain, SDNPOptInGlue]>;" << std::endl;
#endif
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
      << hasSQRTF << "; }" << std::endl;
    
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


/**
 * Writes operation definition in .td format to an output stream.
 *
 * @param o Output stream to write the definition to.
 * @param op Operation to write definition for.
 */
void
TDGen::writeOperationDef(
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

    // now works for 1 and 0 outputs. Need changes when multiple
    // output become supported.
    int intOutCount = 0;
    if (op.numberOfOutputs() == 1 && !op.readsMemory() ) {
        
        // These are a mess in Operation class.
        Operand& operand = op.operand(op.numberOfInputs()+1);
        if (operand.type() == Operand::UINT_WORD || 
            operand.type() == Operand::SINT_WORD) {
            intOutCount = 1;
        }
        // no bool outs for some operatios
        if (op.name() == "CFI" || op.name() == "CFIU") {
            intOutCount = 0;
        }
        
        // rotations are olways n x n -> n bits.
        if (op.name() == "ROTL" || op.name() == "ROTR" ||
            op.name() == "SHL" || op.name() == "SHR" || op.name() == "SHRU" ||
            op.name() == "SXHW" || op.name() == "SXQW") {
            intOutCount = 0;
        }
        
        // these can have 1-bit inputs
        if (op.name() == "XOR" || op.name() == "IOR" || op.name() == "AND" ||
            op.name() == "ANDN" || op.name() == "ADD" || op.name() == "SUB") {
            intOutCount = 2;
        }
        if (op.readsMemory() && intOutCount == 2 ) {
            intOutCount = 1; // 1bit addresses not reasonable
        }
    }
    
    // boolout = 0 means n x n -> n bits operation defs
    // boolout = 1 means n x n -> 1 bit operation defs
    // boolout = 2 means 1 x 1 -> 1 bit operation defs
    for (int boolOut = 0; boolOut <= intOutCount; boolOut++) {
        // for fully 1-bit ops use postfix letter b instead of r
        // meaning registers
        char regInputChar = boolOut != 2 ? 'r' : 'b';
        std::string suffix(op.numberOfInputs(), regInputChar);

        // for n x n -> 1 bit ops postfix another b letter to the name
        if (boolOut == 1) {
            suffix += 'b';
        }
        
        // Start iteration from -1 so first pattern is w/o immediates.
        int inputCount = op.numberOfInputs();
        for (int immInput = 0;
             immInput <= inputCount; immInput++) {
	   
            // check here if nodes connected to inputs are commutative
            if (immInput > 0) {
                if (immInput == 1) {
                    // Don't create op(imm, reg) patterns for commutative
                    // binary operations. tablegen wants only
                    // op(reg, imm) pattern for them.
                    if (inputCount == 2 && op.canSwap(1, 2)) {
                        // LLVM does not understand that setne and seteq
                        // are commutative. so make both versions of them.
                        if (op.name() != "EQ" && op.name() != "NE") {
                            continue;
                        }
                    }
                    if (inputCount == 1 && !op.input(0).isAddress()) {
                        continue;
                    }
                }
                
                // TODO: fancier check if operation dag made operation really 
                // should have a commutative pattern defined.
                // That would need analyzing nodes which are connected to 
                // inputs of DAG and check if they are comutative or not.
                if (llvmOperationPattern(op.name()) == "" && 
                    op.dagCount() != 0) {
                    continue;
                }

                if (!(op.operand(immInput).type() == Operand::SINT_WORD ||
                      op.operand(immInput).type() == Operand::UINT_WORD)) {

                    // Only integer immediates.
                    continue;
                }
            }

            std::string outputs, inputs, asmstr, pattern, patSuffix;
            outputs = "(outs" + patOutputs(op, boolOut!=0) + ")";
            inputs = "(ins " + patInputs(op, immInput, boolOut==2) + ")";
            asmstr = "\"\"";
            patSuffix = suffix;
            if (immInput > 0) {
                if (boolOut != 2) {
                    patSuffix[immInput - 1] = 'i';
                } else {
                    // for 1 bit immediates use j instead of i
                    patSuffix[immInput -1] = 'j';
                }
            }

            if (llvmOperationPattern(op.name()) != "" || 
                op.dagCount() == 0) {
                OperationDAG* trivial = createTrivialDAG(op);
                pattern = operationPattern(
                    op, *trivial, immInput, boolOut);
                delete trivial;
            } else {
                pattern = operationPattern(
                    op, op.dag(0), immInput, boolOut);
            }
            
            if (attrs != "") {
                o << "let" << attrs << " in { " << std::endl;
            }
            
            std::string opcEnum = 
                StringTools::stringToUpper(op.name()) + patSuffix;
            
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
            opNames_[opcEnum] = op.name();
        }
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
        std::cerr << "end nodes of dag for operation: " << op.name() << " is empty!" << std::endl;
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
    for (int immInput = 0;
	 immInput <= inputCount; immInput++) {

	std::string llvmPat = llvmOperationPattern(op.name());
	assert(llvmPat != "" && "Unknown operation to emulate.");

	boost::format match1(llvmPat);
	boost::format match2(llvmPat);

	// check here if nodes connected to inputs are commutative
	if (immInput > 0) {
	    if (immInput == 1) {
		// Don't create op(imm, reg) patterns for commutative
		// binary operations. tablegen wants only
		// op(reg, imm) pattern for them.
		if (inputCount == 2 && op.canSwap(1, 2)) {
		    // LLVM does not understand that setne and seteq
		    // are commutative. so make both versions of them.
		    if (op.name() != "EQ" && op.name() != "NE") {
			continue;
		    }
		}
		if (inputCount == 1 && !op.input(0).isAddress()) {
		    continue;
		}
	    }
	    
	    if (!(op.operand(immInput).type() == Operand::SINT_WORD ||
		  op.operand(immInput).type() == Operand::UINT_WORD)) {
		
		// Only integer immediates.
		continue;
	    }
	}

	for (int i = 0; i < op.numberOfInputs(); i++) {
	    match1 % operandToString(op.operand(i + 1), false, (i+1 == immInput), false);
	    match2 % operandToString(op.operand(i + 1), false, (i+1 == immInput), true);
	}

	o << "def : Pat<(" << match1.str() << "), "
	  << dagNodeToString(op, dag, *res, immInput, true, false)
	  << ">;" << std::endl;
	
	if (op.name() == "EQ" ||
	    op.name() == "EQF" || op.name() == "EQUF" ||
	    op.name() == "GE" ||op.name() == "GEU" ||
	    op.name() == "GEF" || op.name() == "GEUF" || 
	    op.name() == "GT" || op.name() == "GTU" ||
	    op.name() == "GTF" || op.name() == "GTUF" ||
	    op.name() == "LE" || op.name() == "LEU" ||
	    op.name() == "LEF" || op.name() == "LEUF" ||
	    op.name() == "LT" || op.name() == "LTU" ||
	    op.name() == "LTF" || op.name() == "LTUF" ||
	    op.name() == "NE" ||
	    op.name() == "NEF" || op.name() == "NEUF") {

	    // todo: b versions of those. should this be match2?
	    o << "def : Pat<(" << match1.str() << "), "
	      << dagNodeToString(op, dag, *res, immInput, true, 1)
	      << ">;" << std::endl;

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
TDGen::llvmOperationPattern(const std::string& osalOperationName) {

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

    if (opName == "sxhw") return "sext_inreg %1%, i16";
    if (opName == "sxqw") return "sext_inreg %1%, i8";

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
    const Operation& op, std::set<std::string>* recursionCycleCheck) {
    
    // if operation has llvm pattern
    if (llvmOperationPattern(op.name()) != "") {
        return true;
    }

    // just to avoid new delete, always init set and select if we use 
    // local or the one which was given as parameter
    std::set<std::string> checkSet;
    std::set<std::string>* useSet;
    if (recursionCycleCheck == NULL) {
        useSet = &checkSet;
    } else {
        useSet = recursionCycleCheck;
    }    
    
    // check if one of dags of operation is ok
    for (int i = 0; i < op.dagCount(); i++) {
        OperationDAG& dag = op.dag(i);
        if (op.dagError(i) != "") {
            std::cerr << "Broken dag in operation " << op.name() <<
                op.dagCode(i) << std::endl;
            assert(0);
        }

        bool dagIsGood = true;
      
        for (int j = 0; j < dag.nodeCount(); j++) {
            OperationNode* opNode = dynamic_cast<OperationNode*>(&dag.node(j));
            if (opNode != NULL) {
                Operation& refOp = opNode->referencedOperation();

                // check that the same operation is not used recursively
                if (useSet->count(refOp.name()) != 0) {
                    dagIsGood = false;
                    break;
                }
                    
                // check if referenced op can be matched
                useSet->insert(refOp.name());                
                if (!operationCanBeMatched(refOp, useSet)) {
                    dagIsGood =  false;
                    useSet->erase(refOp.name());
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
    int immOp, int intToBool) {

    std::string retVal;
    for (OperationDAG::NodeSet::iterator i = dag.endNodes().begin(); 
         i != dag.endNodes().end(); ++i) {
        if (i != dag.endNodes().begin()) {
            retVal += ",";
        }
        const OperationDAGNode& res = **(i);
        retVal += dagNodeToString(op, dag, res, immOp, false, intToBool);
    }
    return retVal;
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
    int immOp,
    bool emulationPattern, int intToBool) throw (InvalidData) {

    const OperationNode* oNode = dynamic_cast<const OperationNode*>(&node);
    if (oNode != NULL) {
        assert(
            dag.inDegree(*oNode) ==
            oNode->referencedOperation().numberOfInputs());

        return operationNodeToString(
            op, dag, *oNode, immOp, emulationPattern, intToBool);
    }

    const TerminalNode* tNode = dynamic_cast<const TerminalNode*>(&node);
    if (tNode != NULL) {
        const Operand& operand = op.operand(tNode->operandIndex());
        bool imm = (operand.index() == immOp);
        if (dag.inDegree(*tNode) == 0) {
            // Input operand for the whole operation.
            assert(operand.isInput());

            if (imm && !canBeImmediate(dag, *tNode)) {
                std::string msg = 
                    "Invalid immediate operand for " + op.name() +
                    " operand #" + Conversion::toString(tNode->operandIndex());
                throw InvalidData(__FILE__, __LINE__, __func__, msg);
            }

            return operandToString(operand, false, imm, intToBool == 2);
        } else {

            // Output operand for the whole operation.
            assert(dag.inDegree(*tNode) == 1);
            assert(op.operand(tNode->operandIndex()).isOutput());
            assert(operand.isOutput());

            
            const OperationDAGEdge& edge = dag.inEdge(node, 0);
            const OperationDAGNode& srcNode = dag.tailNode(edge);

            // Multiple-output operation nodes not supported in the middle
            // of dag:
            assert(dag.outDegree(srcNode) == 1);
            
            std::string dnString = 
                (intToBool != 2) ? 
                dagNodeToString(
                    op, dag, srcNode, immOp, emulationPattern, 0) :
                dagNodeToString(
                    op, dag, srcNode, immOp, emulationPattern, 2);
            
            bool needTrunc = (intToBool == 1);

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
                        operand, emulationPattern, imm, intToBool!=0)
                    + ", (trunc " + dnString + "))";
                return pattern;
            } else {
                std::string pattern =
                    "(set " + operandToString(
                        operand, emulationPattern, imm, intToBool !=0) 
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
    int immOp,
    bool emulationPattern,
    int intToBool) throw (InvalidData) {

    const Operation& operation = node.referencedOperation();

    std::string operationPat;

    if (emulationPattern) {

	// Look at incoming nodes. If operand comes from another op,
	// the value is in register. If operand comes from constant,
	// it's immediate.
	// if it's terminal, check if it's an immediate operand
	// of the emulated operation.
        operationPat = StringTools::stringToUpper(operation.name());
	int inputs = operation.numberOfInputs();

	for (int i = 1; i < inputs + 1; i++) {
	    for (int e = 0; e < dag.inDegree(node); e++) {
		const OperationDAGEdge& edge = dag.inEdge(node, e);
		int dst = edge.dstOperand();
		if (dst == i) {
		    if (dynamic_cast<OperationNode*>(&(dag.tailNode(edge)))) {
			operationPat += 'r';
		    } else {
			if (dynamic_cast<ConstantNode*>(&(dag.tailNode(edge)))) {
			    operationPat += 'i';
			} else {
			    TerminalNode* t = dynamic_cast<TerminalNode*>(&(dag.tailNode(edge)));
			    assert (t != NULL);
			    int terminalIndex = t->operandIndex();
			    if (immOp == terminalIndex) {
				operationPat += 'i';
			    } else {
				operationPat += 'r';
			    }
			}
		    }
		    
		}
	    }
	}

        if (intToBool == 1) {
            operationPat += 'b';
        }

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
        operationPat = llvmOperationPattern(operation.name());
        
        // generate pattern for operation if not llvmOperation (can match 
        // custom op patterns)
        if (operationPat == "") {
            operationPat = tceOperationPattern(operation);
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
                pattern % dagNodeToString(
                    op, dag, in, immOp, emulationPattern, intToBool==2?2:0);
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
    bool immediate, int intToBool) {

    int idx = operand.index();

    if (operand.isAddress()) {
        if (immediate) {
            if (match) {
                return "MEMri:$op" + Conversion::toString(idx);
            } else {
                return "ADDRri:$op" + Conversion::toString(idx);
            }
        } else {
            if (match) {
                return  "MEMrr:$op" + Conversion::toString(idx);
            } else {
                return  "ADDRrr:$op" + Conversion::toString(idx);
            }
        }
    } else if (operand.type() == Operand::SINT_WORD ||
               operand.type() == Operand::UINT_WORD) {

        if (immediate) {
            if (match) {
                return (intToBool != 0 ? "i1imm:$op" : "i32imm:$op") + 
                    Conversion::toString(idx);
            } else {
                std::string s = (intToBool != 0 ? "(i1" : "(i32");
                return s + " imm:$op" + Conversion::toString(idx) + ")";
            }
        } else {
            return (intToBool != 0 ? "I1Regs:$op" : "I32Regs:$op") + 
                Conversion::toString(idx);
        }
    } else if (operand.type() == Operand::FLOAT_WORD) {
        if (immediate) {
            // f32 immediates not implemetned
            std::string msg = "f32 immediate operands not supported";
            throw (InvalidData(__FILE__, __LINE__, __func__, msg));
        } else {
            return "F32Regs:$op" + Conversion::toString(idx);
        }
    } else if (operand.type() == Operand::DOUBLE_WORD) {
        if (immediate) {
            // f64 immediates not implemetned
            std::string msg = "f64 immediate operands not supported";
            throw (InvalidData(__FILE__, __LINE__, __func__, msg));
        } else {
            return "F64Regs:$op" + Conversion::toString(idx);
        }
    } else {
        assert(false && "Unknown operand type.");
    }
    abortWithError("Should not get here.");
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
TDGen::patInputs(const Operation& op, int immOp, bool intToBool) {
    std::string ins;
    for (int i = 0; i < op.numberOfInputs(); i++) {
        if (i > 0) {
            ins += ",";
        }
        bool imm = (op.operand(i+1).index() == immOp);
       ins += operandToString(op.operand(i + 1), true, imm, intToBool);
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
TDGen::patOutputs(const Operation& op, bool intToBool) {
    std::string outs;

    for (int i = 0; i < op.numberOfOutputs(); i++) {
        assert(op.operand(op.numberOfInputs() + 1 + i).isOutput());
        outs += (i > 0) ? (",") : (" ");
        outs += operandToString(
            op.operand(op.numberOfInputs() + 1 + i), true, false, intToBool);
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
