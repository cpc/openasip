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
 * @file tcedisasm.cc
 *
 * TCE parallel disassembler.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
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
#include "config.h"

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

        StringCmdLineOptionParser* outputFile = new StringCmdLineOptionParser(
                "outputfile", "Name of the output file.", "o");

        BoolCmdLineOptionParser* lineNumbers = new BoolCmdLineOptionParser(
            "linenumbers", "Print line numbers","n");

        addOption(lineNumbers);
        addOption(outputFile);
    }

    std::string outputFile() {
        return findOption("outputfile")->String();
    }

    bool lineNumbers() {
        return findOption("linenumbers")->isFlagOn();
    }

    void printVersion() const {
        std::cout << "tcedisasm - TCE parallel disassembler " 
                  << VERSION << std::endl;
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

    // Load TPEF.
    TPEF::BinaryStream stream(options.argument(2));
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
    // This might be needed if mixed code is inputted
    UniversalMachine umach;

    try {
        TTAProgram::TPEFProgramFactory factory(*tpef, *machine);
        program = factory.build();
    } catch (Exception& e) {
        
        // Try if mixed code
        try {
            TTAProgram::TPEFProgramFactory factory(*tpef, *machine, umach);
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
    if ((!FileSystem::fileExists(options.outputFile()) &&
         FileSystem::fileIsCreatable(options.outputFile()))) {
        FileSystem::createFile(options.outputFile());
        file.open(options.outputFile().c_str());
        output = &file;        
    } else if (FileSystem::fileIsWritable(options.outputFile())) {
        file.open(options.outputFile().c_str());
        if (file.is_open()) {
            output = &file;
        }
    }

    // Write code section disassembly.
    POMDisassembler disassembler(*program);
    Word first = disassembler.startAddress();
    *output<< "CODE " << first << " ;" << endl << endl;
    for (Word addr = first; addr < (first + disassembler.instructionCount());
         addr++) {

        // Write code labels.
        for (int i = 0; i < disassembler.labelCount(addr); i++) {
            *output << disassembler.label(addr, i) << ":" << endl;
        }

        DisassemblyInstruction* instr = NULL;
        try {
            // instr = disassembler.createInstruction(addr);
            // *output << instr->toString() << endl;
            if (options.lineNumbers()) {
                
                *output << std::left << std::setw(10) << Conversion::toString(addr) +":"
                        << std::left;
            }
            *output << POMDisassembler::disassemble(program->instructionAt(addr)) << endl;
        } catch (Exception& e) {
            cerr << e.fileName() << e.lineNum() << ": " << e.errorMessage()
                 << endl;
        }
        delete instr;
    }

    *output << endl << endl;

    // Write data memory initializations.
    for (int i = 0; i < program->dataMemoryCount(); i++) {

        const TTAProgram::DataMemory& mem = program->dataMemory(i);
        const TTAMachine::AddressSpace& aSpace = mem.addressSpace();

        *output << "DATA " << aSpace.name() << " "
               << aSpace.start() << " ;" << endl;

        // Definitions are put in a map to order them.
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

    return 0;
}

