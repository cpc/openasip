/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file tpef2pasm.cc
 *
 * Blocks parallel disassembler.
 *
 * @author Kanishkan Vadivel 2021 (k.vadivel-no.spam-tue.nl)
 * @author Barry de Bruin 2021 (e.d.bruin-no.spam-tue.nl)
 * @note rating: red
 */

#include <fstream>
#include <iostream>
#include <map>

#include "CompilerWarnings.hh"

IGNORE_COMPILER_WARNING("-Wunused-variable")

#include "ADFSerializer.hh"
#include "Binary.hh"
#include "BinaryReader.hh"
#include "BinaryStream.hh"
#include "CmdLineOptions.hh"
#include "ControlUnit.hh"
#include "DataDefinition.hh"
#include "DataMemory.hh"
#include "DisassemblyInstruction.hh"
#include "FUPort.hh"
#include "FileSystem.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "Immediate.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "Operation.hh"
#include "POMDisassembler.hh"
#include "Procedure.hh"
#include "Program.hh"
#include "RegisterFile.hh"
#include "TPEFProgramFactory.hh"
#include "Terminal.hh"
#include "TerminalImmediate.hh"
#include "UniversalMachine.hh"
#include "tce_config.h"

using std::cerr;
using std::cout;
using std::endl;

// ASM map: {FU: {operand: asm}}
typedef std::map<TCEString, std::map<int, TCEString>> PasmPacket;

/**
 * Commandline options.
 */
class DisasmCmdLineOptions : public CmdLineOptions {
public:
    DisasmCmdLineOptions()
        : CmdLineOptions("Usage: tpef2pasm [options] adffile tpeffile") {
        StringCmdLineOptionParser* outputFile = new StringCmdLineOptionParser(
            "outputfile", "Name of the output file.", "o");

        BoolCmdLineOptionParser* toStdout = new BoolCmdLineOptionParser(
            "stdout", "Print to standard output", "s");

        addOption(outputFile);
        addOption(toStdout);
    }

    std::string
    outputFile() {
        return findOption("outputfile")->String();
    }

    bool
    outputFileDefined() {
        return findOption("outputfile")->isDefined();
    }

    bool
    printToStdout() {
        return findOption("stdout")->isFlagOn();
    }

    void
    printVersion() const {
        std::cout << "tcedisasm - TCE parallel disassembler "
                  << Application::TCEVersionString() << std::endl;
    }

    virtual ~DisasmCmdLineOptions() {}
};

/**
 * Get FunctionUnit object from FU name
 *
 * @param machine Machine for the search
 * @param fuName Name of the FU to search
 * @return FunctionUnit object
 */
TTAMachine::FunctionUnit*
getFunctionUnit(TTAMachine::Machine* machine, TCEString fuName) {
    const TTAMachine::Machine::FunctionUnitNavigator fuNav =
        machine->functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        if (fuNav.item(i)->name().ciEqual(fuName)) return fuNav.item(i);
    }
    return nullptr;
}

/**
 * Get RegisterFile object from RF name
 *
 * @param machine Machine for the search
 * @param rfName Name of the RF to search
 * @return RegisterFile object
 */
TTAMachine::RegisterFile*
getRegisterFile(TTAMachine::Machine* machine, TCEString rfName) {
    const TTAMachine::Machine::RegisterFileNavigator rfNav =
        machine->registerFileNavigator();
    for (int i = 0; i < rfNav.count(); i++) {
        if (rfNav.item(i)->name().ciEqual(rfName)) return rfNav.item(i);
    }
    return nullptr;
}

/**
 * Get RegisterFile object from RF name
 *
 * @param pkt PASM instructions in map form
 * @param fuSet Pasm header, list of FU names
 * @param label Procedure start label
 * @param printHeader Name of the RF to search
 * @return TCEString PASM string
 */
TCEString
getPasmPacket(
    PasmPacket pkt, std::set<TCEString> fuSet, TCEString label = "",
    bool printHeader = false) {
    const int width = 32;  // column width in spaces
    TCEString pasm = "";
    if (printHeader) {
        // first row with FU names
        pasm += "|";
        for (auto& fu : fuSet) {
            TCEString pasm_col = " " + fu;
            int ntabs = width/4 - (pasm_col.size()+1)/4; // 1 tab = 4 spaces
            for (int i = 0; i < ntabs; i++) pasm_col += "\t";
            pasm += pasm_col + "|";
        }
        pasm += "\n";

        // empty row
        pasm += "|";
        for (auto& fu : fuSet) {
            int ntabs = width/4;
            for (int i = 0; i < ntabs; i++) pasm += "\t";
            pasm += "|";
        }
        pasm += "\n";

        // .text row
        pasm += "|";
        for (auto& fu : fuSet) {
            TCEString pasm_col = " .text";
            int ntabs = width/4 - (pasm_col.size()+1)/4;
            for (int i = 0; i < ntabs; i++) pasm_col += "\t";
            pasm += pasm_col + "|";
        }
        pasm += "\n";

        // empty row
        pasm += "|";
        for (auto& fu : fuSet) {
            int ntabs = width/4;
            for (int i = 0; i < ntabs; i++) pasm += "\t";
            pasm += "|";
        }
        pasm += "\n";

    } else {
        pasm += "|";

        int col_idx = 0;
        for (auto& fu : fuSet) {
            TCEString pasm_col = "";

            if (!pkt.count(fu))
                pasm_col += " nop";
            else {
                pkt[fu][0].replaceString("copy", "pass");
                if (pkt[fu][0] == "jump" && pkt[fu][1] == fu+".ra") {
                    pkt[fu][0].replaceString("jump", "ret");
                    pkt[fu][1] = "";
                }
                TCEString instr = " " + pkt[fu][0] + " ";
                for (unsigned op = 1; op < pkt[fu].size(); op++) {
                    instr += pkt[fu][op].replaceString(".ra", ".0"); // .ra register is mapped to out0 (non-registered)
                    if (pkt[fu][op] != "" && op < pkt[fu].size() - 1) instr += ", ";
                }
                pasm_col += instr;
            }

            // Add label to first column, when available
            if (!label.empty() && col_idx++ == 0) {
                pasm_col += " ; " + label;
            }

            // Fix length of column
            int ntabs = width/4 - (pasm_col.size()+1)/4;
            for (int i = 0; i < ntabs; i++) pasm_col += "\t";  // 1 tab = 4 spaces
            pasm += pasm_col + "|";
        }
        pasm += "\n";
    }
    return pasm;
}

/**
 * Get FU output Port name for operation
 *
 * @param fu FunctionUnit to search for instruction
 * @param op Instruction name to search
 * @return TCEString Output port name
 */
TCEString
getOutputPort(TTAMachine::FunctionUnit* fu, TCEString op) {
    TTAMachine::HWOperation* hwOp = fu->operation(op);
    for (int i = hwOp->operandCount(); i > 0; i--) {
        TTAMachine::FUPort* p = hwOp->port(i);
        if (p->isOutput()) return p->name();
    }
    return "";
}

int
main(int argc, char* argv[]) {
    DisasmCmdLineOptions options;

    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
        return 0;
    } catch (IllegalCommandLine& e) {
        std::cerr << "Error: Illegal commandline: " << e.errorMessage()
                  << endl
                  << endl;

        options.printHelp();
        return 1;
    }

    if (options.numberOfArguments() != 2) {
        std::cerr << "Error: Illegal number of parameters." << endl << endl;
        options.printHelp();
        return 1;
    }

    std::string inputFileName = options.argument(2);
    // Load TPEF.
    TPEF::BinaryStream stream(inputFileName);
    TPEF::Binary* tpef = NULL;
    try {
        tpef = TPEF::BinaryReader::readBinary(stream);
    } catch (Exception& e) {
        cerr << "Can't read TPEF binary file: " << options.argument(2) << endl
             << e.errorMessage() << endl;
        return 1;
    }

    // Load machine.
    TTAMachine::Machine* machine = NULL;
    ADFSerializer machineReader;
    machineReader.setSourceFile(options.argument(1));
    try {
        machine = machineReader.readMachine();
    } catch (Exception& e) {
        cerr << "Error: " << options.argument(1) << " is not a valid ADF File"
             << endl
             << e.errorMessage() << endl;

        delete tpef;
        return 1;
    }

    // Create blocks instruction packet header
    std::set<TCEString> blocksFU;
    const TTAMachine::Machine::FunctionUnitNavigator fuNav =
        machine->functionUnitNavigator();
    const TTAMachine::Machine::RegisterFileNavigator rfNav =
        machine->registerFileNavigator();
    const TTAMachine::Machine::ImmediateUnitNavigator immNav =
        machine->immediateUnitNavigator();
    TTAMachine::ControlUnit* abu = machine->controlUnit();
    for (int i = 0; i < fuNav.count(); i++) {
        TCEString fuName = fuNav.item(i)->name();
        blocksFU.insert(fuName.split("_out")[0]);
    }
    for (int i = 0; i < rfNav.count(); i++)
        blocksFU.insert(rfNav.item(i)->name());
    for (int i = 0; i < immNav.count(); i++)
        blocksFU.insert(immNav.item(i)->name());
    blocksFU.insert(abu->name());

    // Create POM.
    TTAProgram::Program* program = NULL;

    try {
        TTAProgram::TPEFProgramFactory factory(*tpef, *machine);
        program = factory.build();
    } catch (Exception& e) {
        // Try if mixed code
        try {
            TTAProgram::TPEFProgramFactory factory(
                *tpef, *machine, &UniversalMachine::instance());
            program = factory.build();
        } catch (Exception& e1) {
            cerr << "Error: " << e.errorMessage() << " and "
                 << e1.errorMessage() << endl;
            delete tpef;
            delete machine;
            return 1;
        }
    }

    std::ostream* output = &std::cout;
    std::fstream file;
    std::string outputFileName = options.outputFileDefined()
                                     ? options.outputFile()
                                     : inputFileName + ".pasm";
    if (!options.printToStdout()) {
        if ((!FileSystem::fileExists(outputFileName) &&
             FileSystem::fileIsCreatable(outputFileName))) {
            FileSystem::createFile(outputFileName);
            file.open(
                outputFileName.c_str(),
                std::fstream::trunc | std::fstream::out);
            output = &file;
        } else if (FileSystem::fileIsWritable(outputFileName)) {
            file.open(
                outputFileName.c_str(),
                std::fstream::trunc | std::fstream::out);
            if (file.is_open()) output = &file;
        }
    }

    // Write code section disassembly.
    POMDisassembler disassembler(*program);

    // assumes instruction addresses always start from 0
    PasmPacket pasmHeader;
    TCEString pasm = getPasmPacket(pasmHeader, blocksFU, "", true);

    // Write code section disassembly.
    for (int p = 0; p < program->procedureCount(); p++) {
        const TTAProgram::Procedure& proc = program->procedureAtIndex(p);
        TCEString label = proc.name();

        // Process each instruction
        for (int i = 0; i < proc.instructionCount(); i++) {
            TTAProgram::Instruction* instr = &proc.instructionAtIndex(i);
            // ASM map: {FU: {operand: asm}}
            PasmPacket asmMap;

            // Generate immediates
            for (int j = 0; j < instr->immediateCount(); j++) {
                TTAProgram::Immediate& imm = instr->immediate(j);
                asmMap[imm.destination().immediateUnit().name()][0] = "imm";
                asmMap[imm.destination().immediateUnit().name()][1] =
                    std::to_string(imm.value().value().intValue()); // signed value
            }

            // Generate instructions
            for (int j = 0; j < instr->moveCount(); j++) {
                TTAProgram::Move& m = instr->move(j);
                TTAProgram::Terminal& src = m.source();
                TTAProgram::Terminal& dst = m.destination();

                // Input source (bypass) of an operation
                TCEString srcFUName =
                    src.port().parentUnit()->name().split("_out")[0];
                TCEString srcPortName = src.port().name();
                srcPortName = srcPortName.replaceString("out", "");
                TCEString srcPort = srcFUName + "." + srcPortName;

                // Operand port of FU
                TTAMachine::FunctionUnit* dstFU =
                    getFunctionUnit(machine, dst.port().parentUnit()->name());
                TTAMachine::RegisterFile* dstRF =
                    getRegisterFile(machine, dst.port().parentUnit()->name());
                TTAMachine::RegisterFile* srcRF =
                    getRegisterFile(machine, src.port().parentUnit()->name());

                if (srcRF) { // found a load
                    if (!asmMap.count(srcRF->name())) {
                        asmMap[srcRF->name()][0] = "lrm";
                    } else if(asmMap[srcRF->name()][0] == "srm") { // if there is already a store
                        asmMap[srcRF->name()][0] = "lrm_srm";
                    }
                    asmMap[srcRF->name()][1] =
                        "r" + src.toString().split(".")[1];
                }

                if (dstRF) { // found a store
                    if (!asmMap.count(dstRF->name())) {
                        asmMap[dstRF->name()][0] = "srm";
                    } else if (asmMap[dstRF->name()][0] == "lrm") {  // if there is already a load
                        asmMap[dstRF->name()][0] = "lrm_srm";
                    }
                    asmMap[dstRF->name()][2] =
                        "r" + dst.toString().split(".")[1];
                    asmMap[dstRF->name()][3] = srcPort;
                } else if (dstFU) {
                    TCEString dstPort = dst.port().name();
                    dstPort = dstPort.replaceString("in", "");
                    TCEString dstFUName = dstFU->name().split("_out")[0];
                    if (dst.isTriggering()) {
                        TCEString opName = dst.toString().split(".")[2];
                        asmMap[dstFUName][0] = opName;
                        TCEString outPort = getOutputPort(dstFU, opName);
                        if (!outPort.empty())
                            asmMap[dstFUName][0] += " " + outPort + ",";
                    }
                    int inputPortID = std::stoi(dstPort.split("t")[0]);
                    asmMap[dstFUName][inputPortID] = srcPort;
                } else {
                    // ABU move. Handle it based on Blocks assumptions
                    std::vector<TCEString> abuParm =
                        dst.toString().split(".");
                    TCEString dstFUName = abu->name().split("_out")[0];
                    if (dst.isRA()) {
                        // Convert to store to RF
                        asmMap[dstFUName][0] = "srm";
                        asmMap[dstFUName][1] = "r1"; // $RA
                        asmMap[dstFUName][2] = srcPort;
                    } else if (dst.isTriggering()) {
                        // Handle control flow calls
                        asmMap[dstFUName][0] = abuParm[2];
                        asmMap[dstFUName][1] = srcPort;
                    } else if (abuParm[1].ciEqual("value")) {
                        // Handle additional param
                        asmMap[dstFUName][2] = srcPort;
                    } else {
                        std::cout << "Parsing failed for: " + dst.toString()
                                  << std::endl;
                        assert(false && "unknown format for abu op\n");
                    }
                }
            }
            pasm += getPasmPacket(asmMap, blocksFU, label);
            label.clear();
        }
    }
    *output << pasm;
    return 0;
}
