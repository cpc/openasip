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
 * @file HDBTester.cc
 *
 * Implementation of HDBTester
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include "HDBTester.hh"
#include "ImplementationTester.hh"
#include "HDBRegistry.hh"
using std::string;
using std::vector;
using std::set;

HDBTester::HDBTester(): infoStream_(NULL), errorStream_(NULL), sim_(SIM_GHDL),
                        verbose_(false), leaveDirty_(false) {
}

HDBTester::HDBTester(
    std::ostream& infoStream, 
    std::ostream& errorStream,
    VhdlSim simulator, bool verbose, bool leaveDirty):
    infoStream_(&infoStream), errorStream_(&errorStream), sim_(simulator),
    verbose_(verbose), leaveDirty_(leaveDirty) {
}

HDBTester::~HDBTester() {
    // clear hdb cache
    HDB::HDBRegistry* registry = &HDB::HDBRegistry::instance();
    delete registry;
}

bool
HDBTester::testAllEntries(std::string hdbFile) {

    ImplementationTester* implTester = initializeTester(hdbFile);
    if (implTester == NULL) {
        return false;
    }

    bool noFailures = true;
    set<int> fus = implTester->fuEntryIDs();
    for (set<int>::iterator iter = fus.begin(); iter != fus.end(); iter++) {
        if (!testFU(*iter, implTester)) {
            noFailures = false;
            std::cerr << "FU Entry " << *iter << " from " << hdbFile
                      << " failed." << std::endl;
        }
    }

    set<int> rfs = implTester->rfEntryIDs();
    for (set<int>::iterator iter = rfs.begin(); iter != rfs.end(); iter++) {
        if (!testRF(*iter, implTester)) {
            noFailures = false;
            std::cerr << "RF Entry " << *iter << " from " << hdbFile
                      << " failed." << std::endl;
        }
    }
    delete implTester;
    return noFailures;
}

bool
HDBTester::testOneRF(std::string hdbFile, int entryId) {

    ImplementationTester* implTester = initializeTester(hdbFile);
    if (implTester == NULL) {
        return false;
    }
    bool success = testRF(entryId, implTester);
    delete implTester;

    if (!success) {
        std::cerr << "RF Entry " << entryId << " from " << hdbFile
                  << " failed." << std::endl;
    }

    return success;
}

bool
HDBTester::testOneFU(std::string hdbFile, int entryId) {
    
    ImplementationTester* implTester = initializeTester(hdbFile);
    if (implTester == NULL) {
        return false;
    }
    bool success = testFU(entryId, implTester);
    delete implTester;

    if (!success) {
        std::cerr << "FU Entry " << entryId << " from " << hdbFile
                  << " failed." << std::endl;
    }
    
    return success;
}

ImplementationTester*
HDBTester::initializeTester(std::string hdbFile) {
    
    ImplementationTester* tester = NULL;
    try {
        tester = 
            new ImplementationTester(hdbFile, sim_, verbose_, leaveDirty_);
    } catch (Exception& e) {
        if (errorStream_ != NULL) {
            *errorStream_ << "Failed to create implementation tester: "
                          << e.errorMessage() << std::endl;
        }
        return NULL;
    }
    return tester;
}

bool
HDBTester::testFU(int id, ImplementationTester* tester) {

    string reason = "";
    if (!tester->canTestFU(id, reason)) {
        if (infoStream_ != NULL) {
            *infoStream_ << "Cannot test FU id " << id << " because: " 
                         << reason << std::endl;
        }
        // this is not failure
        return true;
    }
    vector<string> errors;
    bool success = false;
    try {
        success = tester->validateFU(id, errors);
    } catch (Exception& e) {
        if (errorStream_ != NULL) {
            *errorStream_ 
                << "Runtime error: " << e.errorMessage() << std::endl;
        }
        return false;
    }
    if (!errors.empty()) {
        if (errorStream_ != NULL) {
            for (unsigned int i = 0; i < errors.size(); i++) {
                 *errorStream_ << errors.at(i);
            }
        }
        success = false;
    }
    return success;
}

bool
HDBTester::testRF(int id, ImplementationTester* tester) {

    string reason = "";
    if (!tester->canTestRF(id, reason)) {
        if (infoStream_ != NULL) {
            *infoStream_ << "Cannot test RF id " << id << " because: " 
                         << reason << std::endl;
        }
        // this is not failure
        return true;
    }
    vector<string> errors;
    bool success = false;
    try {
        success = tester->validateRF(id, errors);
    } catch (Exception& e) {
        if (errorStream_ != NULL) {
            *errorStream_ 
                << "Runtime error: " << e.errorMessage() << std::endl;
        }
        return false;
    }
    if (!errors.empty()) {
        if (errorStream_ != NULL) {
            for (unsigned int i = 0; i < errors.size(); i++) {
                 *errorStream_ << errors.at(i);
            }
        }
        success = false;
    }
    return success;
}
