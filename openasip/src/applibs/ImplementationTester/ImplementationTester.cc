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
 * @file ImplementationTester.cc
 *
 * Implementation of ImplementationTester class.
 *
 * @author Pekka Jääskeläinen 2006,2010 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#include "ImplementationTester.hh"

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "CachedHDBManager.hh"
#include "FUArchitecture.hh"
#include "FUEntry.hh"
#include "FUImplementation.hh"
#include "FUTestbenchGenerator.hh"
#include "FunctionUnit.hh"
#include "GhdlSimulator.hh"
#include "HDBManager.hh"
#include "HWBlockImplementation.hh"
#include "ImplementationSimulator.hh"
#include "InputPortState.hh"
#include "Machine.hh"
#include "MachineState.hh"
#include "MachineStateBuilder.hh"
#include "MemorySystem.hh"
#include "ModelsimSimulator.hh"
#include "OutputPortState.hh"
#include "RFArchitecture.hh"
#include "RFEntry.hh"
#include "RFImplementation.hh"
#include "RFTestbenchGenerator.hh"

using std::string;
using std::vector;
using std::ofstream;

/**
 * Default constructor
 */
ImplementationTester::ImplementationTester(): 
    hdbFile_(""),
    hdb_(NULL),
    simulator_(SIM_GHDL), verbose_(false), leaveDirty_(false), tempDir_("") {
}

/**
 * Constructor
 *
 * @param hdbFile Name of the hdb file
 * @param simulator Name of the HDL simulator to be used
 */
ImplementationTester::ImplementationTester(
    std::string hdbFile, VhdlSim simulator): 
    hdbFile_(hdbFile),
    hdb_(NULL),
    simulator_(simulator),
    verbose_(false), leaveDirty_(false), tempDir_("") {

    hdbFile_ = FileSystem::absolutePathOf(hdbFile_);
    openHdb(hdbFile_);
}


/**
 * Constructor
 *
 * @param hdbFile Name of the hdb file
 * @param simulator Name of the HDL simulator to be used
 * @param verbose Enable verbose messages
 * @param leaveDirty Don't delete created testbench files
 */
ImplementationTester::ImplementationTester(
    std::string hdbFile, VhdlSim simulator, bool verbose, bool leaveDirty):
    hdbFile_(hdbFile),
    hdb_(NULL),
    simulator_(simulator),
    verbose_(verbose), leaveDirty_(leaveDirty), tempDir_("") {

    hdbFile_ = FileSystem::absolutePathOf(hdbFile_);
    openHdb(hdbFile_);
}

/**
 * Destructor
 */
ImplementationTester::~ImplementationTester() {
    if (leaveDirty_) {
        if (tempDir_.empty()) {
            std::cout << "No testbench files created" << std::endl;
        } else {
            std::cout << "Testbench files are stored at " << tempDir_ 
                      << std::endl;
        }
    } else {
        // clear created files
        if (!tempDir_.empty()) {
            FileSystem::removeFileOrDirectory(tempDir_);
        }
    }
}


/**
 * Set VHDL simulator to be used in simulation
 *
 * @param simulator Name of the HDL simulator to be used
 */
void 
ImplementationTester::setVhdlSimulator(VhdlSim simulator) {
    simulator_ = simulator;
}


/**
 * Tries to open hdb file
 *
 * @param hdbFile Name of the hdb file
 */
void 
ImplementationTester::openHdb(std::string hdbFile) {

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

/**
 * Tests whether a FU can be tested
 *
 * @param entryID Entry ID of the FU
 * @param reason If FU can't be tested this string holds the reason why
 * @return True if can be tested
 */
bool 
ImplementationTester::canTestFU(const int entryID, std::string& reason) {

    bool canTest = true;
    HDB::FUEntry* fuEntry = NULL;
    try {
        fuEntry = fuEntryFromHdb(entryID);
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
    } else if (fuHasOnePort(fuEntry)) {
        reason = "FU has only one port";
        canTest = false;
    }
    delete fuEntry;
    return canTest;
}

/**
 * Tests whether a RF can be tested
 *
 * @param entryID Entry ID of the RF
 * @param reason If RF can't be tested this string holds the reason why
 * @return True if can be tested
 */
bool 
ImplementationTester::canTestRF(const int entryID, std::string& reason) {

    bool canTest = true;
    HDB::RFEntry* rfEntry = NULL;
    try {
        rfEntry = rfEntryFromHdb(entryID);
    } catch (KeyNotFound& e) {
        std::ostringstream errorStream;
        errorStream << "ID " << entryID << " does not exist in HDB "
                    << hdbFile_;
        reason = errorStream.str();
        return false;
    }
    if (!rfEntry->hasArchitecture()) {
        reason = "RF entry does not have architecture";
        canTest = false;
    } else if (!rfEntry->hasImplementation()) {
        reason = "RF entry does not have implementation";
        canTest = false;
    } else if (rfEntry->architecture().readPortCount() == 0) {
        reason = "RF does not have a read port";
        canTest = false;
    } else if (rfEntry->architecture().writePortCount() == 0) {
        reason = "RF does not have a write port";
        canTest = false;
    } else if (rfEntry->architecture().bidirPortCount() != 0) {
        reason = "RF has bidirectional ports";
        canTest = false;
    } else if (rfEntry->architecture().latency() != 1) {
        reason = "RF does not have latency of 1 cycle";
        canTest = false;
    }
    delete rfEntry;
    return canTest;
}

/**
 * Validates that FU behaviour model and implementation are equal
 *
 * @param entryID Entry ID of the FU
 * @param errors Error messages from the validation process
 * @return True if there were no errors
 */
bool 
ImplementationTester::validateFU(
    const int entryID, std::vector<std::string>& errors) {

    if (tempDir_.empty()) {
        if (!createTempDir()) {
            IOException exp(__FILE__, __LINE__, "ImplementationTester",
                            "Couldn't create temp directory");
            throw exp;
        }
    }

    string reason;
    if (!canTestFU(entryID, reason)) {
        InvalidData e(
            __FILE__, __LINE__, "ImplementationTester",
            "Cannot test FU: " + reason);
        throw e;
    }
    
    HDB::FUEntry* fuEntry = NULL;
    try {
        fuEntry = fuEntryFromHdb(entryID);
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
    string tbName = fuTbName(entryID);

    createTestbench(&tbGen, tbName);

    bool success = 
        simulateTestbench(tbName, &fuEntry->implementation(), errors);
    
    if (!leaveDirty_) {
        FileSystem::removeFileOrDirectory(tbName);
    }

    delete fuEntry;
    return success;
}


/**
 * Validates that RF behaviour model and implementation are equal
 *
 * @param entryID Entry ID of the RF
 * @param errors Error messages from the validation process
 * @return True if there were no errors
 */
bool 
ImplementationTester::validateRF(
    const int entryID, std::vector<std::string>& errors) {

    if (tempDir_.empty()) {
        if (!createTempDir()) {
            IOException exp(__FILE__, __LINE__, "ImplementationTester",
                            "Couldn't create temp directory");
            throw exp;
        }
    }

    string reason;
    if (!canTestRF(entryID, reason)) {
        InvalidData e(
            __FILE__, __LINE__, "ImplementationTester",
            "Cannot test RF: " + reason);
        throw e;
    }
    
    HDB::RFEntry* rfEntry = NULL;
    try {
        rfEntry = rfEntryFromHdb(entryID);
    } catch (KeyNotFound& e) {
        std::ostringstream errorStream;
        errorStream << "ID " << entryID << " does not exist in HDB "
                    << hdbFile_;
        string errorMsg = errorStream.str();
        errors.push_back(errorMsg);
        return false;
    }

    RFTestbenchGenerator tbGen(rfEntry);
    string tbName = rfTbName(entryID);

    createTestbench(&tbGen, tbName);

    bool success = 
        simulateTestbench(tbName, &rfEntry->implementation(), errors);
    
    if (!leaveDirty_) {
        FileSystem::removeFileOrDirectory(tbName);
    }
    delete rfEntry;
    return success;
}


std::set<int> 
ImplementationTester::fuEntryIDs() const {

    if (!hdb_) {
      InvalidData e(
            __FILE__, __LINE__, "ImplementationTester",
            "HDB is not defined");
        throw e;
    }
    return hdb_->fuEntryIDs();
}


std::set<int> 
ImplementationTester::rfEntryIDs() const {

    if (!hdb_) {
      InvalidData e(
            __FILE__, __LINE__, "ImplementationTester",
            "HDB is not defined");
        throw e;
    }
    return hdb_->rfEntryIDs();
}


bool 
ImplementationTester::fuHasMemoryAccess(HDB::FUEntry* fuEntry) const {

    HDB::FUArchitecture arch = fuEntry->architecture();
    return arch.architecture().hasAddressSpace();
}


bool 
ImplementationTester::fuFullyPipelined(HDB::FUEntry* fuEntry) const  {

    HDB::FUArchitecture arch = fuEntry->architecture();
    return arch.architecture().pipelineElementCount() == 0;
}


bool 
ImplementationTester::fuHasExternalPorts(HDB::FUEntry* fuEntry) const {

    HDB::FUImplementation* fuImpl = &fuEntry->implementation();
    return fuImpl->externalPortCount() != 0;
}


bool 
ImplementationTester::fuHasOnePort(HDB::FUEntry* fuEntry) const {

    HDB::FUImplementation* fuImpl = &fuEntry->implementation();
    return fuImpl->architecturePortCount() == 1;
}


HDB::FUEntry* 
ImplementationTester::fuEntryFromHdb(int entryID) const {

    if (!hdb_) {
      InvalidData e(
            __FILE__, __LINE__, "ImplementationTester",
            "HDB is not defined");
        throw e;
    }
    return hdb_->fuByEntryID(entryID);
}


HDB::RFEntry* 
ImplementationTester::rfEntryFromHdb(int entryID) const {

    if (!hdb_) {
      InvalidData e(
            __FILE__, __LINE__, "ImplementationTester",
            "HDB is not defined");
        throw e;
    }
    return hdb_->rfByEntryID(entryID);
}


bool 
ImplementationTester::createTempDir() {

    tempDir_ = FileSystem::createTempDirectory();
    return !tempDir_.empty();
}


/**
 * Creates name for the testbench file
 *
 * @param id ID number of the FU
 * @return testbench name
 */
std::string 
ImplementationTester::fuTbName(int id) const {

    std::ostringstream name;
    name << tempDir_ << FileSystem::DIRECTORY_SEPARATOR
         << "tb_fu_" << id << ".vhdl";
    return name.str();
}

/**
 * Creates name for the testbench file
 *
 * @param id ID number of the RF
 * @return testbench name
 */
std::string 
ImplementationTester::rfTbName(int id) const {

    std::ostringstream name;
    name << tempDir_ << FileSystem::DIRECTORY_SEPARATOR
         << "tb_rf_" << id << ".vhdl";
    return name.str();
}


void 
ImplementationTester::openTbFile(
    std::ofstream& fileStream, std::string fileName) const {

    fileStream.open(fileName.c_str());
    if (!fileStream) {
        IOException(__FILE__, __LINE__, "ImplementationTester",
                    "Failed to open file " + fileName + " for writing!");
    }
}


/**
 * Creates a list of HDL files of a FU/RF implementation
 *
 * @param impl FU/RF implementation
 * @param files Vector containing the HDL files
 */
void
ImplementationTester::createListOfSimulationFiles(
    const HDB::HWBlockImplementation* impl,
    std::vector<std::string>& files) const {

    for (int i = 0; i < impl->implementationFileCount(); i++) {
        string fullPath = "";
        string filename = impl->file(i).pathToFile();
        if (FileSystem::isAbsolutePath(filename)) {
            fullPath = filename;
        } else {
            string hdbPath = FileSystem::directoryOfPath(hdbFile_);
            fullPath = hdbPath + FileSystem::DIRECTORY_SEPARATOR + filename;
        }
        files.push_back(fullPath);
    }
}

/**
 * Compiles and simulates the testbech
 *
 * @param testbench Name of the testbench file
 * @param implementation Implementation of the FU/RF
 * @param errors Error messages from the compilation/simulation
 * @return True if compilation and simulation were successfull
 */
bool 
ImplementationTester::simulateTestbench(
    std::string testbench,
    const HDB::HWBlockImplementation* implementation,
    std::vector<std::string>& errors) const {

    vector<string> hdlFiles;
    createListOfSimulationFiles(implementation, hdlFiles);

    ImplementationSimulator* sim = NULL;
    if (simulator_ == SIM_GHDL) {
        sim = new GhdlSimulator(testbench, hdlFiles, verbose_, leaveDirty_);
    } else if (simulator_ == SIM_MODELSIM) {
        sim = new ModelsimSimulator(
            testbench, hdlFiles, verbose_, leaveDirty_);
    }

    if (!sim->compile(errors)) {
        delete sim;
        return false;
    }
    
    bool success = true;
    if (!sim->simulate(errors)) {
        success = false;
    }
    delete sim;
    return success;
}

void
ImplementationTester::createTestbench(
    TestbenchGenerator* tbGen, std::string tbName) const {
    
    ofstream fileStream;
    openTbFile(fileStream, tbName);
    tbGen->generateTestbench(fileStream);
    fileStream.close();
}
