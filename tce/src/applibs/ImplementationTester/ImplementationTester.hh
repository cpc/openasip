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
 * @file ImplementationTester.hh
 *
 * Declaration of ImplementationTester
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMPLEMENTATION_TESTER_HH
#define TTA_IMPLEMENTATION_TESTER_HH

#include <string>
#include <vector>
#include <set>
#include "HDBManager.hh"
#include "FUEntry.hh"
#include "RFEntry.hh"
#include "MachineState.hh"
#include "HWBlockImplementation.hh"

enum VhdlSim {
    SIM_GHDL, 
    SIM_MODELSIM
};

class ImplementationTester {
public:
    ImplementationTester();

    ImplementationTester(std::string hdbFile, VhdlSim simulator);

    ImplementationTester(
        std::string hdbFile,
        VhdlSim simulator, bool verbose, bool leaveDirty);

    ~ImplementationTester();

    void setVhdlSimulator(VhdlSim simulator);

    void openHdb(std::string hdbFile);

    bool canTestFU(const int entryID, std::string& reason);

    bool canTestRF(const int entryID, std::string& reason);

    bool validateFU(const int entryID, std::vector<std::string>& errors);

    bool validateRF(const int entryID, std::vector<std::string>& errors);

    std::set<int> fuEntryIDs() const;

    std::set<int> rfEntryIDs() const;

private:

    bool fuHasMemoryAccess(HDB::FUEntry* fuEntry) const;

    bool fuFullyPipelined(HDB::FUEntry* fuEntry) const;

    bool fuHasExternalPorts(HDB::FUEntry* fuEntry) const;

    HDB::FUEntry* fuEntryFromHdb(int entryID) const;

    HDB::RFEntry* rfEntryFromHdb(int entryID) const;

    bool createTempDir();

    std::string fuTbName(int id);
    
    std::string rfTbName(int id);

    void openTbFile(std::ofstream& fileStream, std::string fileName);

    void
    createListOfSimulationFiles(
        HDB::HWBlockImplementation* impl,
        std::vector<std::string>& files) const;

    std::string hdbFile_;
    HDB::HDBManager* hdb_;

    VhdlSim simulator_;
    bool verbose_;
    bool leaveDirty_;

    std::string tempDir_;

};

#endif
