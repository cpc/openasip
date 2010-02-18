/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file TestHDB.cc
 *
 * HDB tester utility program
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "ImplementationTester.hh"
#include "TestHDB.hh"
#include "TestHDBCmdLineOptions.hh"
using std::string;
using std::vector;
using std::set;

const string GHDL_SIM = "ghdl";
const string VSIM_SIM = "modelsim";

int main(int argc, char* argv[]) {
    TestHDBCmdLineOptions options;
    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& exception) {
        std::cerr << exception.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }
    string hdbFile = "";
    if (options.numberOfArguments() < 1) {
        std::cerr << "HDB file was not given" << std::endl;
        options.printHelp();
        return EXIT_FAILURE;
    } else if (options.numberOfArguments() > 1) {
        std::cerr << "Illegal arguments" << std::endl;
        options.printHelp();
        return EXIT_FAILURE;
    } else {
        hdbFile = options.hdbFile();
    }
    
    string simulator = options.vhdlSim();
    VhdlSim sim = SIM_GHDL;
    if (simulator == VSIM_SIM) {
        sim = SIM_MODELSIM;
    } else if (simulator == GHDL_SIM || simulator.empty()) {
        sim = SIM_GHDL;
    } else {
        std::cerr << "Unknown HDL simulator" << simulator << std::endl;
        options.printHelp();
        return EXIT_FAILURE;
    }

    bool verbose = options.verbose();
    bool leaveDirty = options.leaveDirty();

    ImplementationTester* tester = NULL;
    try {
        tester = 
            new ImplementationTester(hdbFile, sim, verbose, leaveDirty);
    } catch (Exception& e) {
        std::cerr << "Failed to create implementation tester: "
                  << e.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }

    bool testAll = true;
    if (options.isFUEntryIDGiven()) {
        testAll = false;
        int entryID = options.fuEntryID();
        if (!testOneFU(entryID, tester)) {
            delete(tester);
            return EXIT_FAILURE;
        }
    }
    if (options.isRFEntryIDGiven()) {
        testAll = false;
        int entryID = options.rfEntryID();
        if (!testOneRF(entryID, tester)) {
            delete(tester);
            return EXIT_FAILURE;
        }
    }
    if (testAll) {
        if (!testAllEntries(tester)) {
            delete(tester);
            return EXIT_FAILURE;
        }   
    }

    delete(tester);
    return EXIT_SUCCESS;
}

bool testOneFU(int id, ImplementationTester* tester) {
    string reason = "";
    if (!tester->canTestFU(id, reason)) {
        std::cout << "Cannot test FU id " << id << " because: " << reason
                  << std::endl;
        // this is not failure
        return true;
    }
    vector<string> errors;
    bool success = false;
    try {
        success = tester->validateFU(id, errors);
    } catch (Exception& e) {
        std:: cerr << "Runtime error: " << e.errorMessage() << std::endl;
        return false;
    }
    
    if (!errors.empty()) {
        for (unsigned int i = 0; i < errors.size(); i++) {
            std::cerr << errors.at(i);
        }
        success = false;
    }
    return success;
}

bool testOneRF(int id, ImplementationTester* tester) {
    // write me
    id = id;
    tester = tester;
    return true;
}

bool testAllEntries(ImplementationTester* tester) {
    bool noFailures = true;
    set<int> fus = tester->fuEntryIDs();
    for (set<int>::iterator iter = fus.begin(); iter != fus.end(); iter++) {
        if (!testOneFU(*iter, tester)) {
            noFailures = false;
        }
    }
    set<int> rfs = tester->rfEntryIDs();
    for (set<int>::iterator iter = rfs.begin(); iter != rfs.end(); iter++) {
        if (!testOneRF(*iter, tester)) {
            noFailures = false;
        }
    }
    return noFailures;
}
