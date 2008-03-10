/**
 * @file tcedisasm.cc
 *
 * TCE parallel disassembler.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
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
            cerr << "Error: " << e.errorMessage() << " and " << e1.errorMessage() << endl;
            delete tpef;
            delete machine;
            return 1;
        }
    }

    std::ostream* output = &std::cout;
    std::fstream file;
    if (FileSystem::fileIsCreatable(options.outputFile())) {
        FileSystem::createFile(options.outputFile());
        file.open(options.outputFile().c_str());
        output = &file;
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
            *output << endl << "DA " << def->size();
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

