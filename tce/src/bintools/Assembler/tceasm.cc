/** 
 * @file tceasm.cc
 *
 * TCE paraller assembler commandline client.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
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
        std::cout << "tceasm - TCE parallel assembler " << VERSION << std::endl;
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

