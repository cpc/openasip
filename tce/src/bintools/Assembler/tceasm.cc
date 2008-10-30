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
 * @file tceasm.cc
 *
 * TCE paraller assembler commandline client.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "CmdLineOptions.hh"

#include "Assembler.hh"

#include "ADFSerializer.hh"
#include "Machine.hh"

#include "Binary.hh"
#include "TPEFWriter.hh"

#include "config.h"

using namespace TTAMachine;
using namespace TPEF;

/**
 * Commandline options.
 */
class AsmCmdLineOptions : public CmdLineOptions {
public:
    AsmCmdLineOptions() : 
        CmdLineOptions("Usage: tceasm [options] adffile assemblerfile") {
        
        StringCmdLineOptionParser* outputFile = 
            new StringCmdLineOptionParser("outputfile", "Name of the output file.", "o");
        
        addOption(outputFile);

        BoolCmdLineOptionParser* quietMode = 
            new BoolCmdLineOptionParser("quiet", "Don't print warnings.", "q");
        
        addOption(quietMode);
    }
    
    std::string outputFile() {
        return findOption("outputfile")->String();
    }
    
    bool printWarnings() {
        return findOption("quiet")->isFlagOff();
    }
    
    void printVersion() const {
        std::cout << "tceasm - TCE parallel assembler " 
                  << Application::TCEVersionString() 
                  << std::endl;
    }
};

int main(int argc, char *argv[]) {
    
    AsmCmdLineOptions options;
    Machine* machine = NULL;
    
    try {
        options.parse(argv, argc);
    } catch (const ParserStopRequest& e) {
        return EXIT_SUCCESS;
    } catch (IllegalCommandLine &e) {
        std::cerr << "Error: Illegal commandline: " 
                  << e.errorMessage() << "\n\n";
        options.printHelp();
        return 1;
    }
    
    if (options.numberOfArguments() != 2) {
        std::cerr << "Error: Illegal number of parameters.\n\n";
        options.printHelp();
        return 1;
    }
    
    // find out which parameter was adf file
    ADFSerializer machineReader;
    machineReader.setSourceFile(options.argument(1));
    
    std::string adfFileName = options.argument(1);
    std::string asmFileName = options.argument(2);
    
    try {
        machine = machineReader.readMachine();
        
    } catch ( ... ) {
        std::cerr << "Error: " << adfFileName << " is not valid ADF file.\n\n";
        options.printHelp();
        return 1;
    }
        
    // generate default output file name:
    // file.name.ending -> file.name.tpef
    // justafile -> justafile.tpef
    std::string::size_type dotPosition = asmFileName.rfind('.');
    std::string genOutputFileName;
    
    std::string outputFileName;
    
    if (dotPosition != std::string::npos) {
        genOutputFileName = asmFileName.substr(0, dotPosition);
    } else {
        genOutputFileName = asmFileName;
    }
    
    genOutputFileName += ".tpef";
    
    outputFileName = options.outputFile();
    
    if (outputFileName == "") {
        outputFileName = genOutputFileName;
    };
    
    BinaryStream asmFile(asmFileName);
    
    Assembler assembler(asmFile, *machine);

    // if there was failure during compilation
    bool failure = false;

    try {
        Binary* compiledTPEF = assembler.compile();
        
        if (options.printWarnings()) {
            for (unsigned int i = 0; i < assembler.warningCount(); i++) {
                const Assembler::CompilerMessage& message = assembler.warning(i);
                std::cerr << "Warning in line " << message.lineNumber
                          << ": " << message.assemblerLine 
                          << "\nreason: " << message.message 
                          << std::endl;       
            }
        }
        
        try {
            BinaryStream tpefStream(outputFileName);
            
            TPEFWriter::instance().writeBinary(tpefStream, compiledTPEF);    

        } catch (Exception& e) {            
            // if error during the TPEF writing to file.
            delete compiledTPEF;
            compiledTPEF = NULL;

            CompileError error(
                __FILE__, __LINE__, __func__,
                "Problems while writing Binary to file: " +
                outputFileName);

            error.setCause(e);

            throw error;
        }

    } catch (CompileError& e) {
        std::cerr << "Error while compiling file: " << asmFileName << std::endl
                  << e.errorMessage() << std::endl;
        failure = true;
        
    } catch (Exception& e) {
        std::cerr << "Strange exception while compiling file: " << asmFileName << std::endl
                  << e.errorMessage() << std::endl;
        failure = true;
        
    } catch ( ... ) {
        std::cerr << "Unknown exception!\n";
        failure = true;
    } 
    
    // back to the nature!
    delete machine;
    machine = NULL;
    
    if (failure) {
        return 1;
    } else {
        return 0;
    }
}

