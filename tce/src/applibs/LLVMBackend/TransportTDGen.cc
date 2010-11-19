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
 * @author Pekka Jääskeläinen 2010
 */

#include <boost/format.hpp>

#include "TransportTDGen.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "Machine.hh"
#include "OperationPool.hh"
#include "HWOperation.hh"
#include "BaseFUPort.hh"
#include "FUPort.hh"
#include "Operation.hh"
#include "Operand.hh"
#include "DisassemblyFUPort.hh"
#include "DisassemblyFUOpcodePort.hh"
#include "TCEString.hh"
#include "OperationDAGSelector.hh"
#include "LLVMBackend.hh"
#include "Guard.hh"

const std::size_t FIXED_REGISTERS = 2; // SP + FP

TransportTDGen::TransportTDGen(const TTAMachine::Machine& mach) :
    TDGen(mach), funcArgRegs_(4) {
}

void 
TransportTDGen::generateBackend(std::string& path) 
    throw (Exception) {

    if (!FileSystem::fileExists(path)) {
        FileSystem::createDirectory(path);
    } else if (FileSystem::fileExists(path) && 
               !FileSystem::fileIsDirectory(path)) {
        throw Exception(__FILE__, __LINE__, __func__,
                        "The given path is not a directory.");
    }
    directoryRoot_ = path;

    analyzeRegisters();
    checkRequiredRegisters();

    writeCallingConv();
    writeInstrFormats();
    writeInstrInfo();
    writeRegisterInfo();
}

void
TransportTDGen::writeCallingConv() {
    std::ofstream f(
        (directoryRoot_ + DIR_SEPARATOR + "TTACallingConv.td").c_str(),
        std::ios::trunc);
    
    TCEString parameterRegList = "";
    // reg 0 = SP
    // reg 1 = FP
    for (std::size_t i = FIXED_REGISTERS; 
         i < regs32bit_.size() && i < FIXED_REGISTERS + funcArgRegs_; 
         ++i) {
        RegInfo reg = regs32bit_.at(i);
        if (i > FIXED_REGISTERS)
            parameterRegList << ", ";
        parameterRegList << reg.rf << "_" << reg.idx;
    }
    
    TCEString returnValueRegister = "";
    RegInfo reg = regs32bit_.at(FIXED_REGISTERS);
    returnValueRegister << reg.rf << "_" << reg.idx;

    // the function argument register list
    f << "def CC_TTA : CallingConv<[" << std::endl
      << "\tCCAssignToReg<[" << parameterRegList << "]>" << std::endl
      << "]>;" << std::endl << std::endl;

    f << "def RetCC_TTA : CallingConv<[" << std::endl
      << "\tCCAssignToReg<[" << returnValueRegister << "]>" << std::endl
      << "]>;" << std::endl;

    f.close();
}

void
TransportTDGen::writeInstrFormats() {
    std::ofstream f(
        (directoryRoot_ + DIR_SEPARATOR + "TTAInstrFormats.td").c_str(),
        std::ios::trunc);
    f << "class InstTTA<dag outs, dag ins, string asmstr, list<dag> pattern> : Instruction {" << std::endl
      << "\tlet Namespace = \"TTA\";" << std::endl
      << "\tlet OutOperandList = outs;" << std::endl
      << "\tlet InOperandList = ins;" << std::endl
      << "\tlet AsmString = asmstr;" << std::endl
      << "\tlet Pattern = pattern;" << std::endl
      << "}" << std::endl;
    f.close();
}

void
TransportTDGen::writeInstrInfo() {
    std::ofstream f(
        (directoryRoot_ + DIR_SEPARATOR + "TTAInstrInfo.td").c_str(),
        std::ios::trunc);
    
    f << "include \"TTAInstrFormats.td\"" << std::endl 
      << "include \"TTAInstrInfo.inc\"" << std::endl 
      << std::endl;

    OperationDAGSelector::OperationSet opNames = 
        LLVMBackend::llvmRequiredOpset(false);

    const TTAMachine::Machine::FunctionUnitNavigator fuNav =
        mach_.functionUnitNavigator();

    OperationPool opPool;
    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit* fu = fuNav.item(i);
        fu->operationNames(opNames);
    }

    TCETools::CIStringSet::const_iterator iter = opNames.begin();
    for (; iter != opNames.end(); iter++) {
        Operation& op = opPool.operation((*iter).c_str());
        if (&op == &NullOperation::instance()) {
            throw Exception(
                __FILE__, __LINE__, __func__,
                TCEString("Cannot find operation definition for ") +
                *iter);
        }
        
        TCEString llvmOpName = llvmOperationName(op.name());
        if (op.numberOfOutputs() > 1 || op.numberOfInputs() == 0 ||
            llvmOpName == "")
            continue;

        TCEString opname = op.name();
        
        if (op.readsMemory() || op.writesMemory()) {
            f << "let ";
            if (op.readsMemory()) {
                f << "mayLoad = 1";
                if (op.writesMemory())
                    f << ", ";
            }
            if (op.writesMemory()) 
                f << "mayStore = 1";
            f << " in " << std::endl;
        }
        
        f << "def " << opname.upper() << " : InstTTA<(outs";
        if (op.numberOfOutputs() > 0) {
            f << " " << opname.capitalize() << "Regs:$o";
        }
        f << "), (ins ";
        bool inputOnly = op.numberOfOutputs() == 0;

        for (int i = 0; i < op.numberOfInputs(); ++i) {
            if (i > 0) f << ", ";            

            if (inputOnly && i == 0) {
                // stores use the first operand reg as the "FU reg"
                f << opname.capitalize() << "Regs:$o";
            } else {
                f << "ReadableRegs:$" << char('a' + i);
            }
        }
        f << "), " << std::endl;
        f << "\t\t\"" << llvmOpName << " ";
        if (!inputOnly)
            f << "%o, ";
        
        for (int i = 0; i < op.numberOfInputs(); ++i) {
            if (i > 0) f << ", ";
            TCEString operandName(char('a' + i));
            if (inputOnly && i == 0) operandName = "o";

            if (op.operand(i + 1).isAddress()) {
                f << "[$" << operandName << "]";
            } else {
                f << "%" << operandName;
            }
        }
        if (llvmOpName == "sext_inreg") {
            f << ", ";
            if (opname.lower() == "sxhw") {
                f << "i16";
            } else if (opname.lower() == "sxqw") {
                f << "i8";
            } else {
                abortWithError(
                    TCEString("Unknown sign extension width (operation ") +
                    opname + ").");
            }
        }


/*
-  def STW : InstTTA<(outs), (ins StwRegs:$i, ReadableRegs:$addr),
-            "store $i, [$addr]",
-           [(store StwRegs:$i, ReadableRegs:$addr)]>;
-}



+def STW : InstTTA<(outs), (ins ReadableRegs:$a, ReadableRegs:$b),
+               "store %o, %a, %b",
+       [(set StwRegs:$o, (store ReadableRegs:$a, ReadableRegs:$b))]>;

*/

        f << "\"," << std::endl
          << "\t["; 
        if (!inputOnly) {
            f << "(set " << opname.capitalize() << "Regs:$o, ";
        }
        f << "(" << llvmOperationName(opname.lower()) << " ";
        for (int i = 0; i < op.numberOfInputs(); ++i) {
            if (i > 0) f << ", ";

            if (inputOnly && i == 0) {
                // stores use the first operand reg as the "FU reg"
                f << opname.capitalize() << "Regs:$o";
            } else {
                f << "ReadableRegs:$" << char('a' + i);
            }
        }
        if (llvmOpName == "sext_inreg") {
            f << ", ";
            if (opname.lower() == "sxhw") {
                f << "i16";
            } else if (opname.lower() == "sxqw") {
                f << "i8";
            } else {
                abortWithError(
                    TCEString("Unknown sign extension width (operation ") +
                    opname + ").");
            }
        }

        if (!inputOnly) {
            f << ")";
        }
        f << ")]>;" << std::endl << std::endl;
    }
    f.close();
}

void
TransportTDGen::writeRegisterInfo() {
    std::ofstream f(
        (directoryRoot_ + DIR_SEPARATOR + "TTARegisterInfo.td").c_str(),
        std::ios::trunc);

    f << "include \"TTARegisterInfo.inc\"" << std::endl << std::endl;
    
    const TTAMachine::Machine::FunctionUnitNavigator fuNav =
        mach_.functionUnitNavigator();

    OperationDAGSelector::OperationSet opsToPrint =
        LLVMBackend::llvmRequiredOpset(false);

    std::set<TCEString> outputPortNames;
    std::set<TCEString> readableRegs;
    readableRegs.insert("FP");
    readableRegs.insert("RA");

    std::set<TCEString> writableRegs;
    writableRegs.insert("FP");
    writableRegs.insert("RA");

    std::set<TCEString> guardableRegs;
    std::set<TCEString> triggerRegs;
    OperationPool opPool;
    int maxVirtualTriggers = 0;
    // print the port reg defs
    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit& fu = *fuNav.item(i);
        for (int p = 0; p < fu.portCount(); ++p) {
            TTAMachine::BaseFUPort& port = *fu.port(p);
            if (port.isTriggering()) {
                // for triggering ports we create "virtual registers" for
                // each opcode
                for (int op = 0; op < fu.operationCount(); ++op) {
                    TCEString operationName = fu.operation(op)->name();
                    TCEString underscoredPortNameStr = 
                        underScoredPortName(port, operationName);
                    TCEString asmPortNameStr = 
                        assemblyPortName(port, operationName);
                    f << "def " << underscoredPortNameStr << " : TTAReg<\""
                      << asmPortNameStr << "\">;" << std::endl;
                    triggerRegs.insert(underscoredPortNameStr);
                }
            } else {
                TCEString underscoredPortNameStr = 
                    underScoredPortName(port);
                TCEString asmPortNameStr = assemblyPortName(port);
                f << "def " << underscoredPortNameStr << " : TTAReg<\""
                  << asmPortNameStr << "\">;" << std::endl;
            }
        }
        maxVirtualTriggers = std::max(maxVirtualTriggers, fu.operationCount());
        
        f << std::endl;

        fu.operationNames(opsToPrint);
    }  

    for (OperationDAGSelector::OperationSet::const_iterator op = 
             opsToPrint.begin(); op != opsToPrint.end(); ++op) {
        TCEString operationName = *op;
        Operation& osalOp = opPool.operation(operationName.c_str());
        if (&osalOp == &NullOperation::instance()) {
            throw Exception(
                __FILE__, __LINE__, __func__,
                TCEString("Cannot find operation definition for  ") +
                operationName);
        }

        int portWidth = 32;
        TCEString portNameStr = "";

        const TTAMachine::Machine::FunctionUnitNavigator fuNav =
            mach_.functionUnitNavigator();

        for (int i = 0; i < fuNav.count(); i++) {
            const TTAMachine::FunctionUnit& fu = *fuNav.item(i);
            
            if (!fu.hasOperation(operationName))
                continue;

            TTAMachine::FUPort* thePort = NULL;
            if (osalOp.numberOfOutputs() == 0) {
                // stores do not have outputs, use input reg as the handle
                // try to find the non-triggering port name
                for (int operandId = 1; 
                     operandId <= osalOp.numberOfInputs(); ++operandId) {
                    thePort = fu.operation(operationName)->port(operandId);
                    if (!thePort->isTriggering())
                        break;
                }
            } else if (osalOp.numberOfOutputs() > 1) {
                continue;
            } else {
                int outputOperandId = osalOp.numberOfInputs() + 1;
                thePort = fu.operation(operationName)->port(
                    outputOperandId);
                outputPortNames.insert(thePort->name());
            }
            portWidth = thePort->width();
            if (portNameStr != "") portNameStr += ", ";
            portNameStr += underScoredPortName(*thePort);
        } 

        if (portNameStr == "") portNameStr = "NONE";
        f << "def " << operationName.capitalize() 
          << "Regs : RegisterClass<\"TTA\", [i" << portWidth
          << "], " << portWidth << ", [" << portNameStr
          << "]>;" << std::endl;        
    }

    f << std::endl << std::endl;

    bool widthPrinted = false;
    bool first = true;
 
    f << "def OutputRegs : RegisterClass<\"TTA\", [i";
    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit& fu = *fuNav.item(i);
        for (int p = 0; p < fu.portCount(); ++p) {
            TTAMachine::BaseFUPort& port = *fu.port(p);
            if (!AssocTools::containsKey(outputPortNames, port.name()))
                continue;
            // assume for now that all outputs are of the same width
            if (!widthPrinted) {
                f << port.width() << "], " << port.width() << ", [";
                widthPrinted = true;
            }
            if (!first) {
                f << ", ";
            }
            first = false;
            TCEString pn = underScoredPortName(port);
            f << pn;
            readableRegs.insert(pn);
        }
    }   
    f << "]>;" << std::endl;

    int maxInputs = 0;

    widthPrinted = false;
    first = true;
    f << "def InputRegs : RegisterClass<\"TTA\", [i";
    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit& fu = *fuNav.item(i);
        int inputs = 0;
        for (int p = 0; p < fu.portCount(); ++p) {
            TTAMachine::BaseFUPort& port = *fu.port(p);
            if (AssocTools::containsKey(outputPortNames, port.name()) ||
                port.isTriggering())
                continue;
            ++inputs;
            // assume for now that all inputs are of the same width
            if (!widthPrinted) {
                f << port.width() << "], " << port.width() << ", [";
                widthPrinted = true;
            }
            if (!first) {
                f << ", ";
            }
            first = false;
            TCEString pn = underScoredPortName(port);
            f << pn;
            writableRegs.insert(pn);
        }
        maxInputs = std::max(maxInputs, inputs);
    }   
    f << "]>;" << std::endl;

    widthPrinted = false;
    first = true;
    f << "def TriggerRegs : RegisterClass<\"TTA\", [i32], 32, [";

    for (std::set<TCEString>::const_iterator i = triggerRegs.begin();
         i != triggerRegs.end(); ++i) {
        if (i != triggerRegs.begin()) 
            f << ", ";
        f << *i;
    }
    f << "]>;" << std::endl << std::endl;

    const TTAMachine::Machine::RegisterFileNavigator rfNav =
        mach_.registerFileNavigator();
    for (int i = 0; i < rfNav.count(); i++) {
        const TTAMachine::RegisterFile& rf = *rfNav.item(i);
        for (int r = 0; r < rf.numberOfRegisters(); ++r) {
            TCEString regName = rf.name();
            regName << "_";
            regName << r;
            
            f << "def " << regName << " : TTAReg<\""
              << rf.name() << "." << r << "\">;" << std::endl;
        }
    }
    f << std::endl;

    f << "def IntRegs : RegisterClass<\"TTA\", [i32], 32, [";
    for (int i = 0; i < rfNav.count(); i++) {
        const TTAMachine::RegisterFile& rf = *rfNav.item(i);
        if (rf.width() != 32) continue;
        for (int r = 0; r < rf.numberOfRegisters(); ++r) {
            if (r > 0) f << ", ";

            TCEString regName = rf.name();
            regName << "_";
            regName << r;
            f << regName;
            writableRegs.insert(regName);
            readableRegs.insert(regName);
        }
    }
    f << "]>;" << std::endl << std::endl;

    f << "def BoolRegs : RegisterClass<\"TTA\", [i1], 1, [";
    for (int i = 0; i < rfNav.count(); i++) {
        const TTAMachine::RegisterFile& rf = *rfNav.item(i);
        if (rf.width() != 1) continue;
        for (int r = 0; r < rf.numberOfRegisters(); ++r) {
            if (r > 0) f << ", ";
            f << rf.name() << "_" << r;
        }
    }
    f << "]>;" << std::endl << std::endl;

    f << "def ReadableRegs : RegisterClass<\"TTA\", [i32], 32, [";
    for (std::set<TCEString>::const_iterator i = readableRegs.begin();
         i != readableRegs.end(); ++i) {
        if (i != readableRegs.begin()) 
            f << ", ";
        f << *i;
    }
    f << "]>;" << std::endl;
    
    f << "def WritableRegs : RegisterClass<\"TTA\", [i32], 32, [";
    for (std::set<TCEString>::const_iterator i = writableRegs.begin();
         i != writableRegs.end(); ++i) {
        if (i != writableRegs.begin()) 
            f << ", ";
        f << *i;
    }
    f << "]>;" << std::endl;

    const TTAMachine::Machine::BusNavigator busNav =
        mach_.busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        const TTAMachine::Bus& bus = *busNav.item(i);
        for (int g = 0; g < bus.guardCount(); ++g) {
            const TTAMachine::Guard* guard = bus.guard(g);
            // inverted guards not yet supported
            if (guard->isInverted()) continue; 

            const TTAMachine::PortGuard* portGuard = 
                dynamic_cast<TTAMachine::PortGuard*>(bus.guard(g));
            const TTAMachine::RegisterGuard* registerGuard = 
                dynamic_cast<TTAMachine::RegisterGuard*>(bus.guard(g));
            if (portGuard != NULL) {
                if (portGuard->port()->width() > 1) continue;
                guardableRegs.insert(
                    portGuard->port()->parentUnit()->name() + "_" + 
                    portGuard->port()->name());
            } else if (registerGuard != NULL) {
                if (registerGuard->registerFile()->width() > 1) continue;
                TCEString name = registerGuard->registerFile()->name() + "_";
                name << registerGuard->registerIndex();
                guardableRegs.insert(name);
            }            
        }
    }

    f << "def GuardableRegs : RegisterClass<\"TTA\", [i1], 1, [";
    for (std::set<TCEString>::const_iterator i = guardableRegs.begin();
         i != guardableRegs.end(); ++i) {
        if (i != guardableRegs.begin()) 
            f << ", ";
        f << *i;
    }
    f << "]>;" << std::endl << std::endl;

    f << "let Namespace = \"TTA\" in {" << std::endl
      << "\tdef output\t: SubRegIndex;" << std::endl;
    for (int input = 1; input <= maxInputs; ++input) {
        f << "\tdef input" << input << "\t: SubRegIndex;" << std::endl;
    }
    for (int trig = 1; trig <= maxVirtualTriggers; ++trig) {
        f << "\tdef trigger" << trig << "\t: SubRegIndex;" << std::endl;
    }
    f << "}" << std::endl << std::endl;

    std::map<std::string, TTAMachine::BaseFUPort*> triggerPorts;
    int maxOperandCount = 0;
    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit& fu = *fuNav.item(i);
        int inputs = 0;
        int triggers = fu.operationCount();
        int outputs = 0;
        f << "def " << fu.name() << " : TTARegWithSubRegs<\"" << fu.name()
          << "\", [";
        for (int p = 0; p < fu.portCount(); ++p) {
            TTAMachine::BaseFUPort& port = *fu.port(p);
            if (p > 0) f << ", ";
            if (AssocTools::containsKey(outputPortNames, port.name())) {
                ++outputs;
                f << underScoredPortName(port);
            } else if (port.isTriggering()) {
                for (int op = 0; op < fu.operationCount(); ++op) {
                    if (op > 0) f << ", ";            
                    TCEString operationName = fu.operation(op)->name();
                    Operation& op = opPool.operation(operationName.c_str());
                    f << underScoredPortName(port, operationName);
                    maxOperandCount = std::max(
                        maxOperandCount, 
                        op.numberOfInputs() + op.numberOfOutputs());
                    triggerPorts[fu.name()] = &port;
                }
            } else {
                ++inputs;
                f << underScoredPortName(port);
            }
        }

        f << "]> {" << std::endl;
        f << "\tlet SubRegIndices = [";
        bool first = true;
        while (outputs--) {
            f << "output";
            first = false;
        }
        for (int i = 1; i <= inputs; ++i) {
            if (!first) f << ", ";
            f << "input" << i;
            first = false;
        }

        for (int i = 1; i <= triggers; ++i) {
            if (!first) f << ", ";
            f << "trigger" << i;
            first = false;
        }
        f << "];" << std::endl
          << "}" << std::endl << std::endl;
    }   

    f << "def FunctionalUnits : RegisterClass<\"TTA\", [i32], 32, [";
    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit& fu = *fuNav.item(i);
        if (i > 0) f << ", ";
        f << fu.name();
    }
    f << "]>;" << std::endl << std::endl;

    f << "let Namespace = \"TTA\" in {" << std::endl;
    for (int i = 0; i < maxOperandCount; ++i) {
        f << "\tdef operand" << i << " : " << "SubRegIndex;" << std::endl;
    }
    f << "}" << std::endl << std::endl;

    std::set<TCEString> bindings;
    for (int operandCount = 1; operandCount <= maxOperandCount; 
         ++operandCount) {
        bool headerPrinted = false;
        for (int i = 0; i < fuNav.count(); i++) {
            const TTAMachine::FunctionUnit& fu = *fuNav.item(i);
            for (int op = 0; op < fu.operationCount(); ++op) {
                Operation& osalOp = 
                    opPool.operation(fu.operation(op)->name().c_str());
                if (osalOp.operandCount() != operandCount) 
                    continue;
                if (!headerPrinted) {
                    f << "let SubRegIndices = [";
                    for (int opr = 0; opr < operandCount; ++opr) {
                        if (opr > 0) f << ", ";
                        f << "operand" << opr;
                    }
                    f << "] in {" << std::endl;
                    headerPrinted = true;
                }
                TCEString bindStr = "BIND_";
                bindStr << fu.name() << "_" << osalOp.name();
                f << "\tdef BIND_" << fu.name() << "_" << osalOp.name() 
                  << " : TTARegWithSubRegs<\"" << bindStr << "\", [";
                bindings.insert(bindStr);

                bool firstPort = true;
                // first print the output port bindings
                for (int out = 0; out < osalOp.numberOfOutputs(); ++out) {
                    if (!firstPort) f << ", ";
                    TTAMachine::FUPort* port = 
                        fu.operation(op)->port(
                            osalOp.numberOfInputs() + out + 1);
                    f << underScoredPortName(*port);
                    firstPort = false;
                }

                // first print the input port bindings
                for (int in = 0; in < osalOp.numberOfInputs(); ++in) {
                    if (!firstPort) f << ", ";
                    TTAMachine::FUPort* port = fu.operation(op)->port(in + 1);
                    TCEString opname = "";
                    if (port->isTriggering())
                        opname = osalOp.name();
                    f << underScoredPortName(*port, opname);
                    firstPort = false;
                }

                f << "]>;" << std::endl;
            }
        }
        if (headerPrinted)
            f << "}" << std::endl << std::endl;
    }

    f << "def Bindings : RegisterClass<\"TTA\", [i32], 32," << std::endl
      << "\t[";
    for (std::set<TCEString>::const_iterator i = bindings.begin();
         i != bindings.end(); ++i) {
        if (i != bindings.begin())
            f << ", ";
        f << *i;
    }    
    f << "]>;" << std::endl << std::endl;

    // the latency regs for non-pipelined FUs
    int maxLatency = 0;
    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit& fu = *fuNav.item(i);
        maxLatency = std::max(maxLatency, fu.maxLatency());
    }

    // TODO: the backend currently doesn't compile in case there are
    // no operations with latency in the ADF
    if (maxLatency > 1) {

        f << "let Namespace = \"TTA\" in {" << std::endl;
        for (int i = 0; i < maxLatency; ++i) {
            f << "\tdef latency" << i << " : SubRegIndex;" << std::endl;
        }
        f << "}" << std::endl << std::endl;
    
        std::set<TCEString> latencyRegs;
        for (int i = 0; i < fuNav.count(); i++) {
            const TTAMachine::FunctionUnit& fu = *fuNav.item(i);
            for (int lat = 1; lat < fu.maxLatency(); ++lat) {
                TCEString latencyReg = fu.name() + "_lat";
                latencyReg << lat;
                f << "def " << latencyReg << " : TTAReg<\"" 
                  << latencyReg << "\">;"
                  << std::endl;
                latencyRegs.insert(latencyReg);
            }
        }
        f << std::endl;
    
        f << "def LatencyRegs : RegisterClass<\"TTA\", [i32], 32, [";
        for (std::set<TCEString>::const_iterator i = latencyRegs.begin();
             i != latencyRegs.end(); ++i) {
            if (i != latencyRegs.begin())
                f << ", ";
            f << *i;
        }
        f << "]>;" << std::endl << std::endl;
        
        std::set<TCEString> latencies;
        for (int latency = 2; latency <= maxLatency; ++latency) {
            bool headerPrinted = false;
            for (int i = 0; i < fuNav.count(); i++) {
                const TTAMachine::FunctionUnit& fu = *fuNav.item(i);
                for (int op = 0; op < fu.operationCount(); ++op) {
                    TTAMachine::HWOperation& hwop = *fu.operation(op);
                    Operation& osalOp = 
                        opPool.operation(fu.operation(op)->name().c_str());
                    if (hwop.latency() != latency)
                        continue;
                    if (!headerPrinted) {
                        f << "let SubRegIndices = [";
                        for (int lat = 0; lat < latency; ++lat) {
                            if (lat > 0) f << ", ";
                            f << "latency" << lat;
                        }
                        f << "] in {" << std::endl;
                        headerPrinted = true;
                    }
                    TCEString latencyReg = "LAT_";
                    latencyReg << fu.name() + "_" + osalOp.name();
                    f << "\tdef " << latencyReg << " : TTARegWithSubRegs<\""
                      << latencyReg << "\", [" 
                      << underScoredPortName(
                          *triggerPorts[fu.name()], osalOp.name());
                    for (int lat = 1; lat < latency; ++lat) {
                        f << ", " << fu.name() << "_lat" << lat;
                    }
                    f << "]>;" << std::endl;
                    latencies.insert(latencyReg);
                }
            }
            if (headerPrinted) {
                f << "}" << std::endl << std::endl;
            }
        }

        f << "def Latencies : RegisterClass<\"TTA\", [i32], 32, [";
        for (std::set<TCEString>::const_iterator i = latencies.begin();
             i != latencies.end(); ++i) {
            if (i != latencies.begin())
                f << ", ";
            f << *i;
        }
        f << "]>;" << std::endl << std::endl;
    }
    
    // TODO: fully pipelined FUs
    f << "def PipelineRegs : RegisterClass<\"TTA\", [i32], 32, [NONE]>;"
      << std::endl;

    f.close();
}

TCEString
TransportTDGen::underScoredPortName(
    const TTAMachine::BaseFUPort& port, 
    TCEString opName) {
    return assemblyPortName(port, opName).replaceString(".", "_");
}

TCEString
TransportTDGen::assemblyPortName(
    const TTAMachine::BaseFUPort& port, 
    TCEString opName) {
    if (opName != "") {
        DisassemblyFUOpcodePort dis(
            port.parentUnit()->name(), port.name(), opName);
        return dis.toString();
    } else {
        DisassemblyFUPort dis(
            port.parentUnit()->name(), port.name());
        return dis.toString();
    }
}
