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
 * @file ImplementationTester.cc
 *
 * Implementation of ImplementationTester class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#include <iostream>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <stdint.h>

#include "ImplementationTester.hh"
#include "HDBManager.hh"
#include "CachedHDBManager.hh"
#include "FUEntry.hh"
#include "FUArchitecture.hh"
#include "FunctionUnit.hh"
#include "Machine.hh"
#include "MachineState.hh"
#include "MachineStateBuilder.hh"
#include "MemorySystem.hh"
#include "GlobalLock.hh"
#include "OutputPortState.hh"
#include "InputPortState.hh"
#include "FUImplementation.hh"
#include "FUTestbenchGenerator.hh"
#include "ImplementationSimulator.hh"
#include "GhdlSimulator.hh"
#include "ModelsimSimulator.hh"

using std::string;
using std::vector;
using std::ofstream;

ImplementationTester::ImplementationTester(): 
    hdbFile_(""),
    hdb_(NULL),
    simulator_(SIM_GHDL), verbose_(false), leaveDirty_(false), tempDir_("") {
    if (!createTempDir()) {
        IOException exp(__FILE__, __LINE__, "ImplementationTester",
                        "Couldn't create temp directory");
        throw exp;
    }
}

ImplementationTester::ImplementationTester(
    std::string hdbFile, VhdlSim simulator): 
    hdbFile_(hdbFile),
    hdb_(NULL),
    simulator_(simulator),
    verbose_(false), leaveDirty_(false), tempDir_("") {
    if (!createTempDir()) {
        IOException exp(__FILE__, __LINE__, "ImplementationTester",
                        "Couldn't create temp directory");
        throw exp;
    }
    hdbFile_ = FileSystem::absolutePathOf(hdbFile_);
}

ImplementationTester::ImplementationTester(
    string hdbFile, VhdlSim simulator, bool verbose, bool leaveDirty):
    hdbFile_(hdbFile),
    hdb_(NULL),
    simulator_(simulator),
    verbose_(verbose), leaveDirty_(leaveDirty), tempDir_("") {

    openHdb(hdbFile_);
    if (!createTempDir()) {
        IOException exp(__FILE__, __LINE__, "ImplementationTester",
                        "Couldn't create temp directory");
        throw exp;
    }
    hdbFile_ = FileSystem::absolutePathOf(hdbFile_);
}

ImplementationTester::~ImplementationTester() {
    if (hdb_) {
        delete(hdb_);
    }
    if (leaveDirty_) {
        // TODO: get correct stream from outside
        std::cout << "Testbench files are stored at " << tempDir_ 
                  << std::endl;
    } else {
        // clear created files
        if (!tempDir_.empty()) {
            FileSystem::removeFileOrDirectory(tempDir_);
        }
    }
}

void ImplementationTester::setVhdlSimulator(VhdlSim simulator) {
    simulator_ = simulator;
}

void ImplementationTester::openHdb(string hdbFile) {
    if (hdb_) {
        delete hdb_;
    }
    try {
        hdb_ = &HDB::CachedHDBManager::instance(hdbFile_);
    } catch (const Exception& e) {
        InvalidData excep(
            __FILE__, __LINE__, "ImplementationTester", 
            "Error while loading input data: " + e.errorMessage());
        throw excep;
    }
    hdbFile_ = hdbFile;
}

bool 
ImplementationTester::canTestFU(const int entryID, string& reason) {
    bool canTest = true;
    
    HDB::FUEntry* fuEntry = NULL;
    try {
        fuEntry = entryFromHdb(entryID);
    } catch (KeyNotFound& e) {
        std::ostringstream errorStream;
        errorStream << "ID " << entryID << " does not exist in HDB "
                    << hdbFile_;
        reason = errorStream.str();
        return false;
    }
    if (!fuEntry->hasArchitecture()) {
        reason = "FU entry does not have architecture";
        canTest = false;
    } else if (!fuEntry->hasImplementation()) {
        reason = "FU entry does not have implementation";
        canTest = false;
    } else if (fuHasMemoryAccess(fuEntry)) {
        reason = "Simulation of memory accessing FUs is not supported";
        canTest = false;
    } else if (!fuFullyPipelined(fuEntry)) {
        reason = "Simulation of non-pipelined FUs in not supported";
        canTest = false;
    } else if (fuHasExternalPorts(fuEntry)) {
        reason = "Simulation of FUs with external ports is not supported";
        canTest = false;
    }
    delete(fuEntry);
    return canTest;
}

bool 
ImplementationTester::canTestRF(const int entryID, string& reason) {
    return true;
}

bool 
ImplementationTester::validateFU(const int entryID, vector<string>& errors) {
    string reason;
    if (!canTestFU(entryID, reason)) {
        InvalidData e(
            __FILE__, __LINE__, "ImplementationTester",
            "Cannot test FU: " + reason);
        throw e;
    }
    
    HDB::FUEntry* fuEntry = entryFromHdb(entryID);

    try {
        fuEntry = entryFromHdb(entryID);
    } catch (KeyNotFound& e) {
        std::ostringstream errorStream;
        errorStream << "ID " << entryID << " does not exist in HDB "
                    << hdbFile_;
        string errorMsg = errorStream.str();
        errors.push_back(errorMsg);
        return false;
    }
     
    FUTestbenchGenerator tbGen(fuEntry);
    
    ofstream fileStream;
    string fileName = fuTbName(entryID);
    openTbFile(fileStream, fileName);
    tbGen.generateTestbench(fileStream);
    fileStream.close();

    vector<string> hdlFiles;
    createListOfSimulationFiles(fuEntry, hdlFiles);

    // call testbench runner here
    ImplementationSimulator* sim = NULL;
    if (simulator_ == SIM_GHDL) {
        sim = new GhdlSimulator(fileName, hdlFiles, verbose_);
    } else if (simulator_ = SIM_MODELSIM) {
        sim = new ModelsimSimulator(fileName, hdlFiles, verbose_);
    }
    
    bool success = false;
    if (sim->compile(errors)) {
        if (sim->simulate(errors)) {
            success = true;
        }
    }
    
    if (!leaveDirty_) {
        FileSystem::removeFileOrDirectory(fileName);
    }
    delete(sim);
    return success;
}

bool 
ImplementationTester::validateRF(const int entryID, vector<string>& errors) {
    return true;
}

std::set<int> ImplementationTester::fuEntryIDs() const {
    return hdb_->fuEntryIDs();
}

std::set<int> ImplementationTester::rfEntryIDs() const {
    return hdb_->rfEntryIDs();
}

bool ImplementationTester::fuHasMemoryAccess(HDB::FUEntry* fuEntry) const {
    HDB::FUArchitecture arch = fuEntry->architecture();
    return arch.architecture().hasAddressSpace();
}

bool ImplementationTester::fuFullyPipelined(HDB::FUEntry* fuEntry) const  {
    HDB::FUArchitecture arch = fuEntry->architecture();
    return arch.architecture().pipelineElementCount() == 0;
}

bool ImplementationTester::fuHasExternalPorts(HDB::FUEntry* fuEntry) const {
    HDB::FUImplementation* fuImpl = &fuEntry->implementation();
    return fuImpl->externalPortCount() != 0;
}

HDB::FUEntry* ImplementationTester::entryFromHdb(int entryID) const {
    if (!hdb_) {
      InvalidData e(
            __FILE__, __LINE__, "ImplementationTester",
            "HDB is not defined");
        throw e;
    }
    return hdb_->fuByEntryID(entryID);
}

bool ImplementationTester::createTempDir() {
    tempDir_ = FileSystem::createTempDirectory();
    return !tempDir_.empty();
}

string ImplementationTester::fuTbName(int id) {
    std::ostringstream name;
    name << tempDir_ << FileSystem::DIRECTORY_SEPARATOR
         << "tb_fu_" << id << ".vhdl";
    return name.str();
}

string ImplementationTester::rfTbName(int id) {
    std::ostringstream name;
    name << tempDir_ << FileSystem::DIRECTORY_SEPARATOR
         << "tb_rf_" << id << ".vhdl";
    return name.str();
}

void ImplementationTester::openTbFile(ofstream& fileStream, string fileName) {
    fileStream.open(fileName.c_str());
    if (!fileStream) {
        IOException(__FILE__, __LINE__, "ImplementationTester",
                    "Failed to open file " + fileName + " for writing!");
    }
}

void
ImplementationTester::createListOfSimulationFiles(
    HDB::FUEntry* fuEntry, vector<string>& files) const {
    HDB::FUImplementation* fuImpl = &fuEntry->implementation();
    for (int i = 0; i < fuImpl->implementationFileCount(); i++) {
        string fullPath = "";
        string filename = fuImpl->file(i).pathToFile();
        if (FileSystem::isAbsolutePath(filename)) {
            fullPath = filename;
        } else {
            string hdbPath = FileSystem::directoryOfPath(hdbFile_);
            fullPath = hdbPath + FileSystem::DIRECTORY_SEPARATOR + filename;
        }
        files.push_back(fullPath);
    }
}


    
