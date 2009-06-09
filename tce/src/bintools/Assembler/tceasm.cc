/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file tceasm.cc
 *
 * TCE paraller assembler commandline client.
 *
 * @author Mikael Lepist� 2005 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "tce_config.h"

#include "CmdLineOptions.hh"
#include "Assembler.hh"
#include "ADFSerializer.hh"
#include "Machine.hh"
#include "Binary.hh"
#include "TPEFWriter.hh"
#include "FileSystem.hh"

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

    if (!FileSystem::fileIsReadable(asmFileName)) {
        std::cerr << "Error: cannot read " << asmFileName << std::endl;
        return 2;
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

