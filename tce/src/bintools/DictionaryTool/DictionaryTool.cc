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
 * @file DictionaryTool.cc
 *
 * DictionaryTool main application.
 *
 * @author Jari M�ntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "ADFSerializer.hh"
#include "Application.hh"
#include "Machine.hh"
#include "ProgrammabilityValidator.hh"
#include "ProgrammabilityValidatorResults.hh"
#include "CmdLineOptions.hh"
#include "Exception.hh"
#include "Binary.hh"
#include "TPEFWriter.hh"
#include "TPEFProgramFactory.hh"
#include "ProgramWriter.hh"
#include "BinaryStream.hh"
#include "MoveElement.hh"
#include "tce_config.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::ofstream;
using std::vector;
using namespace TTAMachine;
using namespace TTAProgram;
using namespace TPEF;

/**
 * DictionaryTool commandline options.
 */
class DictionaryToolCmdLineOptions : public CmdLineOptions {
public:
    DictionaryToolCmdLineOptions() :
        CmdLineOptions("Usage: dictionary_tool [options] adffile") {

        BoolCmdLineOptionParser* printPrimitive =
            new BoolCmdLineOptionParser(
                "primitive",
                "Print missing operations from primitive operation set",
                "p");

        addOption(printPrimitive);

        BoolCmdLineOptionParser* printConnections =
            new BoolCmdLineOptionParser("connections",
                                        "Print all connections in adf.",
                                        "c");

        addOption(printConnections);

        BoolCmdLineOptionParser* printGlobalRegister =
            new BoolCmdLineOptionParser(
                "global_register",
                "Print the name of global connection register", "g");

        addOption(printGlobalRegister);

        BoolCmdLineOptionParser* printMissingConnections =
            new BoolCmdLineOptionParser(
                "missing",
                "Print missing connections to connection register",
                "m");

        addOption(printMissingConnections);

        BoolCmdLineOptionParser* printRFConnections =
            new BoolCmdLineOptionParser(
                "reg_connections",
                "Print all connections to the connection register.", "r");

        addOption(printRFConnections);

        BoolCmdLineOptionParser* doNotPrintErrors =
            new BoolCmdLineOptionParser(
                "no_errors",
                "Don't print any error messages.", "e");
        
        addOption(doNotPrintErrors);

        BoolCmdLineOptionParser* writeBinary =
            new BoolCmdLineOptionParser(
                "binary",
                "Write binary program as output.", "w");

        addOption(writeBinary);
    }

    bool printPrimitive() {
        return findOption("primitive")->isFlagOn();
    }

    bool printConnections() {
        return findOption("connections")->isFlagOn();
    }

    bool printGlobalRegister() {
        return findOption("global_register")->isFlagOn();
    }

    bool printMissingConnections() {
        return findOption("missing")->isFlagOn();
    }

    bool printRFConnections() {
        return findOption("reg_connections")->isFlagOn();
    }

    bool doNotPrintErrors() {
        return findOption("no_errors")->isFlagOn();
    }

    bool writeBinary() {
        return findOption("binary")->isFlagOn();
    }

    void printVersion() const {
        cout << "dictionary_tool - Dictionary Tool " 
             << Application::TCEVersionString() << endl;
    }
};


/**
 * The dictionary_tool main function.
 */
int main(int argc, char* argv[]) {

    DictionaryToolCmdLineOptions options;

    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& e) {
        cerr << e.errorMessage() << endl;
        return EXIT_FAILURE;
    }
    
    if (options.numberOfArguments() == 0) {
        options.printHelp();
        return EXIT_SUCCESS;
    } else if (options.numberOfArguments() > 1) {
        cerr << "Illegal number of arguments" << endl;
        return EXIT_FAILURE;
    }

    string adfFileName = options.argument(1);

    ADFSerializer adfSerializer;
    adfSerializer.setSourceFile(adfFileName);

    TTAMachine::Machine* mach;

    try {
        mach = adfSerializer.readMachine();
    } catch (...) {
        cout.flush();
        cerr << "Error opening ADF file:" << endl
             << "- Is the filename correct?" << endl
             << "- Is the machine legal?" << endl;
        return EXIT_FAILURE;
    }

    ProgrammabilityValidator* validator;
   
    try {	
	validator = new ProgrammabilityValidator(*mach);
    } catch (IllegalMachine& e) {	
	cerr << e.errorMessage() << endl;
	return EXIT_FAILURE;
    }

    ProgrammabilityValidatorResults booleanResults;
    validator->checkBooleanRegister(booleanResults);
    if (booleanResults.errorCount() != 0) {
        for (int i = 0; i < booleanResults.errorCount(); i++) {
            ProgrammabilityValidatorResults::Error error = 
                booleanResults.error(i);
            
            cerr << error.second << endl;
        }
        cerr << "Execution stopped." << endl;
        return EXIT_SUCCESS;
    }

    bool gcrError = false;

    ProgrammabilityValidatorResults results;
    TPEF::Binary* binaryProgram = NULL;
    try {
        binaryProgram = validator->profile(results);
    } catch (NotAvailable& e) {
        cerr << e.errorMessage() << endl;
        cerr << "Check that all operations are found in OSAL." << endl;
        cerr << "Create new ones with OSEd." << endl;
        cerr << "Execution stopped." << endl;
        return EXIT_FAILURE;
    } catch (InstanceNotFound& e) {
        cerr << e.errorMessage() << endl;
    }

    if (options.writeBinary()) {
        string outputFile = adfFileName.append("_profiled.tpef");
        BinaryStream tpefOut(outputFile);
        TPEFWriter::instance().writeBinary(tpefOut, binaryProgram);
    }

    if (!options.doNotPrintErrors()) {
        for (int i = 0; i < results.errorCount(); i++) {
            ProgrammabilityValidatorResults::Error error = results.error(i);
            if (error.first == 
                ProgrammabilityValidator::
                OPERATION_MISSING_FROM_THE_PRIMITIVE_OPERATION_SET) {
                if (options.printPrimitive()) {
                    cerr << error.second << endl;
                }
            }
            else if (error.first == 
                     ProgrammabilityValidator::
                     GLOBAL_CONNECTION_REGISTER_NOT_FOUND) {
                if (options.printGlobalRegister()) {
                    gcrError = true;
                    cerr << error.second << endl;
                }
            }
            else if (error.first ==
                     ProgrammabilityValidator::
                     MISSING_CONNECTION) {
                if (options.printMissingConnections()) {
                    cerr << error.second << endl;
                }
            }
            else {
                cerr << error.second << endl;
            }
        }
    }

    if (options.printGlobalRegister() && !gcrError) {        
        cout << "* The global connection register in the machine is: " 
             << validator->findGlobalConnectionRegister()->name() 
             << endl;
    }
    
    if (options.printConnections()) {
        validator->printConnections();
    }

    if (options.printRFConnections()) {
        validator->printRegisterConnections();
    }
    
    return EXIT_SUCCESS;
}
