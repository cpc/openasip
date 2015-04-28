/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file tcedisasm.cc
 *
 * TCE parallel disassembler.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2009,2011 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <fstream>
#include <map>
#include "CmdLineOptions.hh"
#include "BinaryReader.hh"
#include "Binary.hh"
#include "TPEFProgramFactory.hh"
#include "ADFSerializer.hh"
#include "POMDisassembler.hh"
#include "DisassemblyInstruction.hh"
#include "BinaryStream.hh"
#include "BinaryReader.hh"
#include "DataMemory.hh"
#include "DataDefinition.hh"
#include "FileSystem.hh"
#include "UniversalMachine.hh"
#include "tce_config.h"
#include "Program.hh"

using std::cout;
using std::cerr;
using std::endl;

/**
 * Commandline options.
 */
class DisasmCmdLineOptions : public CmdLineOptions {
public:
    DisasmCmdLineOptions() :
        CmdLineOptions("Usage: tcedisasm [options] adffile tpeffile") {

        StringCmdLineOptionParser* outputFile = 
            new StringCmdLineOptionParser(
                "outputfile", "Name of the output file.", "o");

        BoolCmdLineOptionParser* lineNumbers = 
            new BoolCmdLineOptionParser(
                "linenumbers", "Print line numbers","n");

        BoolCmdLineOptionParser* flatFile = 
            new BoolCmdLineOptionParser(
                "flat", "The instructions only with file line numbers "
                "matching instruction indices","F");

        BoolCmdLineOptionParser* toStdout = 
            new BoolCmdLineOptionParser(
                "stdout", "Print to standard output","s");

        addOption(lineNumbers);
        addOption(outputFile);
        addOption(flatFile);
        addOption(toStdout);
    }

    std::string outputFile() {
        return findOption("outputfile")->String();
    }

    bool outputFileDefined() {
        return findOption("outputfile")->isDefined();
    }

    bool lineNumbers() {
        return findOption("linenumbers")->isFlagOn();
    }

    bool flatFile() {
        return findOption("flat")->isFlagOn();
    }

    bool printToStdout() {
        return findOption("stdout")->isFlagOn();
    }

    void printVersion() const {
        std::cout << "tcedisasm - TCE parallel disassembler " 
                  << Application::TCEVersionString() << std::endl;
    }

    virtual ~DisasmCmdLineOptions() {
    }
};


int main(int argc, char *argv[]) {

    DisasmCmdLineOptions options;

    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
        return 0;
    } catch (IllegalCommandLine& e) {
        std::cerr << "Error: Illegal commandline: "
                  << e.errorMessage() << endl << endl;

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
             << endl << e.errorMessage() << endl;

        delete tpef;
        return 1;
    }
    
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
    std::string outputFileName = 
        options.outputFileDefined() ? 
        options.outputFile() : inputFileName + ".S";
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
            if (file.is_open()) {
                output = &file;
            }
        }
    }

    // Write code section disassembly.
    POMDisassembler disassembler(*program);
    Word first = disassembler.startAddress();
    if (!options.flatFile()) {
        *output << "CODE " << first << " ;" << endl << endl;
        try {
            *output << POMDisassembler::disassemble(
                *program, options.lineNumbers())
                    << endl << endl;

        } catch (Exception& e) {
            std::cerr << "Disassebly failed because of exception: " <<
                e.errorMessage() << std::endl;
        }
        
        // Write data memory initializations.
        for (int i = 0; i < program->dataMemoryCount(); i++) {

            const TTAProgram::DataMemory& mem = program->dataMemory(i);
            const TTAMachine::AddressSpace& aSpace = mem.addressSpace();

            if (mem.dataDefinitionCount() == 0) continue;

            *output << "DATA " << aSpace.name() << " "
                    << mem.dataDefinition(0).startAddress().location()
                    << " ;" << endl;

            // Definitions are put in a map to order them.
            // TODO: the indexing API of DataMemory could be used for this?
            // TODO: does this handle "holes" correctly or assume fully
            // "connected regions" always?
            // TODO: this does not handle UDATA at all!
            std::map<Word, const TTAProgram::DataDefinition*> definitions;
            for (int d = 0; d < mem.dataDefinitionCount(); d++) {
                const TTAProgram::DataDefinition& def = mem.dataDefinition(d);
                definitions[def.startAddress().location()] = &def;
            }

            std::map<Word, const TTAProgram::DataDefinition*>::iterator iter =
                definitions.begin();

            for (; iter != definitions.end(); iter++) {
                const TTAProgram::DataDefinition* def = (*iter).second;
                *output << endl << "DA " << std::dec << def->size();
                if (def->isInitialized())  {
                    for (int mau = 0; mau < def->size(); mau++) {
                        *output << endl << "1:0x" << std::hex << def->MAU(mau);
                    }
                }
                *output << " ;" << endl;
            }
        }
        *output << endl;
    } else {
        // assumes instruction addresses always start from 0
        TTAProgram::Program::InstructionVector instr =
            program->instructionVector();
        for (TTAProgram::Program::InstructionVector::const_iterator i =
                 instr.begin(); i != instr.end(); ++i) {
            *output 
                << POMDisassembler::disassemble(**i, options.lineNumbers())
                << std::endl;
        }
    }  

    return 0;
}

