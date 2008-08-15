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
 * @file DictionaryTool.cc
 *
 * DictionaryTool main application.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva@tut.fi)
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

#include "MoveElement.hh"
#include "config.h"

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
        cout << "dictionary_tool - Dictionary Tool " << VERSION << endl;
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
