/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file TTAUnitTester.cc
 *
 * Implementation of TTAUnitTester utility program
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <cstdlib>
#include <string>
#include <vector>
#include "MachineImplementation.hh"
#include "Machine.hh"
#include "ADFSerializer.hh"
#include "IDFSerializer.hh"
#include "IDFValidator.hh"
#include "TTAUnitTester.hh"
#include "HDBTester.hh"
#include "Exception.hh"
#include "UnitImplementationLocation.hh"
#include "TTAUnitTesterCmdLineOptions.hh"
using std::string;

int main(int argc, char* argv[]) {

    TTAUnitTesterCmdLineOptions options;
    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& exception) {
        std::cerr << exception.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }

    string idfFile = "";
    if (options.numberOfArguments() == 0) {
        std::cerr << "IDF file was not given" << std::endl;
        options.printHelp();
        return EXIT_FAILURE;
    } else if (options.numberOfArguments() > 1) {
        std::cerr << "Illegal arguments" << std::endl;
        options.printHelp();
        return EXIT_FAILURE;
    } else {
        idfFile = options.idfFileName();
    }

    string adfFile = options.adfFileName();

    string sim = options.vhdlSim();
    VhdlSim simulator = SIM_GHDL;
    if (sim == "modelsim") {
        simulator = SIM_MODELSIM;
    } else {
        simulator = SIM_GHDL;
    }

    bool verbose = options.verbose();
    bool leaveDirty = options.leaveDirty();

    IDF::MachineImplementation* idf = readIdf(idfFile);
    if (!idf) {
        return EXIT_FAILURE;
    }

    // validate idf if adf was given
    if (!adfFile.empty()) {
         TTAMachine::Machine* mach = readAdf(adfFile);
        if (!mach) {
            return EXIT_FAILURE;
        }
        if (!validateIdf(*idf, *mach)) {
            delete(mach);
            delete(idf);
            return EXIT_FAILURE;
        }
        delete(mach);
    }

    if (!testUnits(*idf, simulator, verbose, leaveDirty)) {
        delete(idf);
        return EXIT_FAILURE;
    }
    
    delete(idf);
    return EXIT_SUCCESS;
}


TTAMachine::Machine*
readAdf(std::string adfName) {

    ADFSerializer reader;
    reader.setSourceFile(adfName);
    TTAMachine::Machine* mach = NULL;
    try {
        mach = reader.readMachine();
    } catch (Exception& e) {
        std::cerr << "Failed to load ADF file " << adfName << ": "
                  << e.errorMessage() << std::endl;
        return NULL;
    }
    return mach;
}


IDF::MachineImplementation*
readIdf(std::string idfName) {

    IDF::IDFSerializer reader;
    reader.setSourceFile(idfName);
    IDF::MachineImplementation* implementation = NULL;
    try {
        implementation = reader.readMachineImplementation();
    } catch (Exception& e) {
        std::cerr << "Failed to load IDF file " << idfName << ": "
                  << e.errorMessage() << std::endl;
        return NULL;
    }
    return implementation;
}


bool 
validateIdf(
    const IDF::MachineImplementation& idf,
    const TTAMachine::Machine& machine) {

    IDFValidator validator(idf, machine);

    bool success = validator.validate();
    if (!success) {
        for (int i = 0; i < validator.errorCount(); i++) {
            std::cerr << validator.errorMessage(i) << std::endl;
        }
    }
    return success;
}


bool 
testUnits(
    const IDF::MachineImplementation &idf, 
    VhdlSim simulator, bool verbose, bool leaveDirty) {

    HDBTester tester(std::cout, std::cerr, simulator, verbose, leaveDirty);

    bool allPassed = true;
    for (int i = 0; i < idf.fuImplementationCount(); i++) {
        IDF::UnitImplementationLocation* unit = &idf.fuImplementation(i);
        string hdb = unit->hdbFile();
        int entryId = unit->id();
        if (!tester.testOneFU(hdb, entryId)) {
            allPassed = false;
        }
    }
    
    for (int i = 0; i < idf.rfImplementationCount(); i++) {
        IDF::UnitImplementationLocation* unit = &idf.rfImplementation(i);
        string hdb = unit->hdbFile();
        int entryId = unit->id();
        if (!tester.testOneRF(hdb, entryId)) {
            allPassed = false;
        }
    }
    
    return allPassed;
}
