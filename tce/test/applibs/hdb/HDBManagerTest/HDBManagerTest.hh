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
 * @file HDBManagerTest.hh 
 *
 * A test suite for HDBManager.
 * 
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_MANAGER_TEST_HH
#define TTA_HDB_MANAGER_TEST_HH

#include <string>
#include <TestSuite.h>

#include "FileSystem.hh"
#include "CachedHDBManager.hh"
#include "HDBRegistry.hh"
#include "FUEntry.hh"
#include "FUArchitecture.hh"
#include "FUImplementation.hh"
#include "FUPortImplementation.hh"
#include "RFEntry.hh"
#include "RFArchitecture.hh"
#include "RFImplementation.hh"
#include "RFPortImplementation.hh"
#include "FUExternalPort.hh"
#include "Machine.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "PipelineElement.hh"
#include "FUPort.hh"
#include "ADFSerializer.hh"

#include "DataObject.hh"
#include "AssocTools.hh"

using std::string;
using namespace HDB;
using namespace TTAMachine;

const string DS = FileSystem::DIRECTORY_SEPARATOR;
const string HDB_TO_CREATE_1 = "data" + DS + "newHDB1.hdb";
const string HDB_TO_CREATE_2 = "data" + DS + "newHDB2.hdb";
const string HDB_TO_CREATE_3 = "data" + DS + "newHDB3.hdb";
const string HDB_TO_CREATE_4 = "data" + DS + "newHDB4.hdb";
const string HDB_TO_CREATE_5 = "data" + DS + "newHDB5.hdb";
const string HDB_TO_CREATE_6 = "data" + DS + "newHDB6.hdb";
const string HDB_TO_CREATE_7 = "data" + DS + "newHDB7.hdb";
const string HDB_TO_CREATE_8 = "data" + DS + "newHDB8.hdb";
const string TEST_ADF = "data" + DS + "testadf.adf";

/**
 * Class that tests BEMGenerator class.
 */
class HDBManagerTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreatingHDB();
    void testInsertingFUArchitecture();
    void testInsertingFUImplementation();
    void testInsertingRFArchitecture();
    void testInsertingRFImplementation();
    void testGettingFUByID();
    void testGettingRFByID();
    void testGettingCostData();
    void testFUArchitectureMatching();
    void testRFArchitectureMatching();
    void testFUArchitectureIDbyOperationSet();

private:
    void initializeHDB();
};


/**
 * Called before each test.
 */
void
HDBManagerTest::setUp() {
}


/**
 * Called after each test.
 */
void
HDBManagerTest::tearDown() {
}


/**
 * Tests creating a new HDB.
 */
void
HDBManagerTest::testCreatingHDB() {
    FileSystem::removeFileOrDirectory(HDB_TO_CREATE_1);
    TS_ASSERT_THROWS_NOTHING(CachedHDBManager::createNew(HDB_TO_CREATE_1));
    TS_ASSERT(FileSystem::fileExists(HDB_TO_CREATE_1));   
}


/**
 * Tests inserting and removing an FU entry with architecture to HDB.
 */
void
HDBManagerTest::testInsertingFUArchitecture() {

    FileSystem::removeFileOrDirectory(HDB_TO_CREATE_2);
    CachedHDBManager& manager = CachedHDBManager::createNew(HDB_TO_CREATE_2);
    ADFSerializer serializer;
    serializer.setSourceFile(TEST_ADF);
    Machine* mach = serializer.readMachine();
    FunctionUnit* fu = mach->functionUnitNavigator().item(0);
    fu->unsetMachine();

    FUArchitecture arch(fu);
    RowID archID = manager.addFUArchitecture(arch);
    RowID entryID = manager.addFUEntry();
    manager.setArchitectureForFU(entryID, archID);
    
    FUEntry* fuEntry = manager.fuByEntryID(entryID);
    TS_ASSERT(fuEntry->hasArchitecture());
    TS_ASSERT(!fuEntry->hasImplementation());
    
    FUArchitecture& loadedArch = fuEntry->architecture();
    FunctionUnit& loadedFU = loadedArch.architecture();
    TS_ASSERT(loadedFU.portCount() == 3);
    TS_ASSERT(loadedFU.operationCount() == 2);
    TS_ASSERT(loadedFU.hasOperation("add"));
    TS_ASSERT(loadedFU.hasOperation("sub"));
    HWOperation* addOp = loadedFU.operation("add");
    HWOperation* subOp = loadedFU.operation("sub");
    TS_ASSERT(addOp->latency() == 4);
    TS_ASSERT(subOp->latency() == 3);
    FUPort* o1Port = addOp->port(1);
    FUPort* t1Port = addOp->port(2);
    FUPort* r1Port = addOp->port(3);
    TS_ASSERT(subOp->port(1) == addOp->port(1));
    TS_ASSERT(subOp->port(2) == addOp->port(2));
    TS_ASSERT(subOp->port(3) == addOp->port(3));
    TS_ASSERT(loadedFU.pipelineElementCount() == 1);
    PipelineElement* elem = loadedFU.pipelineElement(0);
    ExecutionPipeline* addPL = addOp->pipeline();
    TS_ASSERT(addPL->isPortRead(*o1Port, 0));
    TS_ASSERT(addPL->isPortRead(*t1Port, 0));
    TS_ASSERT(!addPL->isPortRead(*r1Port, 0));
    TS_ASSERT(addPL->isResourceUsed(elem->name(), 1));
    TS_ASSERT(addPL->isResourceUsed(elem->name(), 2));
    TS_ASSERT(addPL->isPortWritten(*r1Port, 3));

    delete fuEntry;
    manager.unsetArchitectureForFU(entryID);
    fuEntry = manager.fuByEntryID(entryID);
    TS_ASSERT(!fuEntry->hasArchitecture());
    manager.removeRFArchitecture(archID);

    delete fuEntry;
    delete mach;
}


/**
 * Tests inserting an FU implementation to the database.
 */
void
HDBManagerTest::testInsertingFUImplementation() {

    FileSystem::removeFileOrDirectory(HDB_TO_CREATE_3);
    HDBManager& manager = CachedHDBManager::createNew(HDB_TO_CREATE_3);
    ADFSerializer serializer;
    serializer.setSourceFile(TEST_ADF);
    Machine* mach = serializer.readMachine();
    FunctionUnit* fu = mach->functionUnitNavigator().item(0);
    fu->unsetMachine();

    FUEntry* newEntry = new FUEntry();
    FUImplementation* impl = new FUImplementation(
        "addsub", "opc", "clk", "rst", "glock", "");
    newEntry->setImplementation(impl);
    TS_ASSERT_THROWS(manager.addFUImplementation(*newEntry), InvalidData);
    RowID entryID = manager.addFUEntry();
    newEntry->setID(entryID);
    TS_ASSERT_THROWS(manager.addFUImplementation(*newEntry), InvalidData);

    // add architecture
    FUArchitecture* arch = new FUArchitecture(fu);
    arch->setParameterizedWidth(fu->port(0)->name());
    arch->setParameterizedWidth(fu->port(1)->name());
    arch->setParameterizedWidth(fu->port(2)->name());
    arch->setGuardSupport(fu->port(2)->name());
    RowID archID = manager.addFUArchitecture(*arch);
    manager.setArchitectureForFU(entryID, archID);

    newEntry->setArchitecture(arch);
    new FUPortImplementation("o1", "o1", "width", "o1_load", "", *impl);
    new FUPortImplementation("t1", "t1", "width", "t1_load", "", *impl);
    new FUPortImplementation("r1", "r1", "width", "", "guard", *impl);
    impl->setOpcode("add", 0);
    impl->setOpcode("sub", 1);
    impl->addParameter("width", "positive", "");
    new FUExternalPort("ext1", HDB::OUT, "1", "description", *impl);
    new FUExternalPort("ext2", HDB::IN, "1", "description", *impl);
    BlockImplementationFile* file = new BlockImplementationFile(
        "foo/bar", BlockImplementationFile::VHDL);
    impl->addImplementationFile(file);

    RowID implID = 0;
    TS_ASSERT_THROWS_NOTHING(
        implID = manager.addFUImplementation(*newEntry));
    FUEntry* loadedEntry = manager.fuByEntryID(entryID);
    TS_ASSERT(loadedEntry->hasArchitecture());
    TS_ASSERT(loadedEntry->hasImplementation());
    FUImplementation& loadedImpl = loadedEntry->implementation();
    FunctionUnit& loadedFU = loadedEntry->architecture().architecture();
    TS_ASSERT(loadedImpl.hasParameter("width"));
    TS_ASSERT(loadedImpl.architecturePortCount() == 3);
    for (int i = 0; i < 3; i++) {
        FUPortImplementation& port = loadedImpl.architecturePort(i);
        if (port.name() == "o1") {
            TS_ASSERT(
                port.architecturePort() == 
                loadedFU.operation("add")->port(1)->name());
        } else if (port.name() == "t1") {
            TS_ASSERT(
                port.architecturePort() ==
                loadedFU.operation("add")->port(2)->name());
        } else if (port.name() == "r1") {
            TS_ASSERT(
                port.architecturePort() ==
                loadedFU.operation("add")->port(3)->name());
        } else {
            TS_ASSERT(false);
        }
    }

    TS_ASSERT_EQUALS(loadedImpl.externalPortCount(), 2);
    TS_ASSERT_EQUALS(loadedImpl.implementationFileCount(), 1);

    manager.removeFUImplementation(implID);
    
    delete loadedEntry;
    delete newEntry;
}


/**
 * Tests inserting a RF architecture to HDB.
 */
void
HDBManagerTest::testInsertingRFArchitecture() {

    FileSystem::removeFileOrDirectory(HDB_TO_CREATE_4);
    HDBManager& manager = CachedHDBManager::createNew(HDB_TO_CREATE_4);
    RFArchitecture arch(2, 1, 1, 2, 1, 1, false);
    RowID archID = manager.addRFArchitecture(arch);
    RowID entryID = manager.addRFEntry();
    manager.setArchitectureForRF(entryID, archID);

    RFEntry* entry1 = manager.rfByEntryID(entryID);
    TS_ASSERT(entry1->hasArchitecture());
    TS_ASSERT(!entry1->hasImplementation());
    RFArchitecture& loadedArch = entry1->architecture();
    TS_ASSERT(loadedArch.hasParameterizedWidth());
    TS_ASSERT(loadedArch.hasParameterizedSize());
    TS_ASSERT_EQUALS(loadedArch.readPortCount(), 2);
    TS_ASSERT_EQUALS(loadedArch.writePortCount(), 1);
    TS_ASSERT_EQUALS(loadedArch.bidirPortCount(), 1);
    TS_ASSERT_EQUALS(loadedArch.maxReads(), 2);
    TS_ASSERT_EQUALS(loadedArch.maxWrites(), 1);
    TS_ASSERT(!loadedArch.hasGuardSupport());
    delete entry1;
}
            

/**
 * Tests inserting an RF implementation to HDB.
 */
void
HDBManagerTest::testInsertingRFImplementation() {

   try 
     {
	
    FileSystem::removeFileOrDirectory(HDB_TO_CREATE_5);
    HDBManager& manager = CachedHDBManager::createNew(HDB_TO_CREATE_5);
    RowID entryID = manager.addRFEntry();

    RFImplementation* impl = new RFImplementation(
        "rf_module", "clk", "rst", "glock", "size", "width", "");
    new RFPortImplementation(
        "r1", HDB::OUT, "r1_load", "r1_opc", "bit_width", *impl);
    new RFPortImplementation(
        "w1", HDB::IN, "w1_load", "w1_opc", "bit_width", *impl);
    RowID implID = manager.addRFImplementation(*impl, entryID);

    RFEntry* entry = manager.rfByEntryID(entryID);
    TS_ASSERT(!entry->hasArchitecture());
    TS_ASSERT(entry->hasImplementation());
    RFImplementation& loadedImpl = entry->implementation();
    TS_ASSERT_EQUALS(loadedImpl.sizeParameter(), "size");
    TS_ASSERT_EQUALS(loadedImpl.widthParameter(), "width");
    TS_ASSERT_EQUALS(loadedImpl.guardPort(), "");
    TS_ASSERT_EQUALS(loadedImpl.clkPort(), "clk");
    TS_ASSERT_EQUALS(loadedImpl.rstPort(), "rst");
    TS_ASSERT_EQUALS(loadedImpl.glockPort(), "glock");
    TS_ASSERT_EQUALS(loadedImpl.portCount(), 2);
    
    for (int i = 0; i < loadedImpl.portCount(); i++) {
        RFPortImplementation& port = loadedImpl.port(i);
        TS_ASSERT_EQUALS(port.opcodePortWidthFormula(), "bit_width");
        if (port.name() == "r1") {
            TS_ASSERT_EQUALS(port.direction(), HDB::OUT);
            TS_ASSERT_EQUALS(port.opcodePort(), "r1_opc");
            TS_ASSERT_EQUALS(port.loadPort(), "r1_load");
        } else {
            TS_ASSERT_EQUALS(port.name(), "w1");
            TS_ASSERT_EQUALS(port.direction(), HDB::IN);
            TS_ASSERT_EQUALS(port.opcodePort(), "w1_opc");
            TS_ASSERT_EQUALS(port.loadPort(), "w1_load");
        }
    }

    delete impl;
    delete entry;

    manager.removeRFImplementation(implID);
    entry = manager.rfByEntryID(entryID);
    TS_ASSERT(!entry->hasImplementation());
    delete entry;
     } catch(Exception& e) 
	  {
	     
	     std::cerr << e.fileName() << e.lineNum() << ":" << e.errorMessage() << std::endl;
	  }
   
}


/**
 * Tests getting an FU entry from HDB by ID.
 */
void
HDBManagerTest::testGettingFUByID() {

    initializeHDB();
    HDBManager& manager = HDBRegistry::instance().hdb(HDB_TO_CREATE_1);
    FUEntry* entry1 = manager.fuByEntryID(1);
    FUEntry* entry2 = manager.fuByEntryID(2);
    TS_ASSERT_THROWS(manager.fuByEntryID(3), KeyNotFound);
    TS_ASSERT(entry1->hasArchitecture());
    TS_ASSERT(entry1->hasImplementation());
    TS_ASSERT(entry1->id() == 1);
    TS_ASSERT(entry1->hdbFile() == HDB_TO_CREATE_1);
    
    TS_ASSERT(entry2->hasArchitecture());
    TS_ASSERT(!entry2->hasImplementation());
    TS_ASSERT(!entry2->hasCostFunction());

    // verify the architecture
    FUArchitecture& entry1Arch = entry1->architecture();
    FunctionUnit& entry1FU = entry1Arch.architecture();
    TS_ASSERT(entry1FU.operationCount() == 2);
    TS_ASSERT(entry1FU.hasOperation("ADD"));
    TS_ASSERT(entry1FU.hasOperation("SUB"));
    TS_ASSERT(entry1FU.portCount() == 3);
    TS_ASSERT_EQUALS(entry1FU.pipelineElementCount(), 1);
    HWOperation* addOp = entry1FU.operation("ADD");
    TS_ASSERT(addOp->latency() == 3);
    FUPort* io1Port = addOp->port(1);
    FUPort* io2Port = addOp->port(2);
    FUPort* io3Port = addOp->port(3);
    TS_ASSERT(!io1Port->isTriggering());
    TS_ASSERT(!io1Port->isOpcodeSetting());
    TS_ASSERT(io2Port->isTriggering());
    TS_ASSERT(io2Port->isOpcodeSetting());
    TS_ASSERT(entry1Arch.hasGuardSupport(io3Port->name()));
    TS_ASSERT(!entry1Arch.hasGuardSupport(io1Port->name()));
    ExecutionPipeline* pLine = addOp->pipeline();
    TS_ASSERT(pLine->isPortRead(*io1Port, 0));
    TS_ASSERT(pLine->isPortRead(*io2Port, 0));
    TS_ASSERT(!pLine->isPortRead(*io3Port, 0));
    TS_ASSERT(!pLine->isPortRead(*io1Port, 1));
    TS_ASSERT(pLine->isPortWritten(*io3Port, 2));
    string resName = entry1FU.pipelineElement(0)->name();
    TS_ASSERT(pLine->isResourceUsed(resName, 1));
    TS_ASSERT(!pLine->isResourceUsed(resName, 0));

    // verify the implementation
    FUImplementation& entry1Impl = entry1->implementation();
    TS_ASSERT(entry1Impl.moduleName() == "addsub");
    TS_ASSERT(entry1Impl.opcodePort() == "opc");
    TS_ASSERT(entry1Impl.clkPort() == "clk");
    TS_ASSERT(entry1Impl.rstPort() == "rst");
    TS_ASSERT(entry1Impl.glockPort() == "glock");
    TS_ASSERT(entry1Impl.glockReqPort() == "");
    TS_ASSERT(entry1Impl.opcode("ADD") == 0);
    TS_ASSERT(entry1Impl.opcode("SUB") == 1);
    TS_ASSERT(entry1Impl.architecturePortCount() == 3);
    TS_ASSERT(entry1Impl.externalPortCount() == 0);
    for (int i = 0; i <= 2; i++) {
        FUPortImplementation& port = entry1Impl.architecturePort(i);
        if (port.name() == "o1") {
            TS_ASSERT(port.architecturePort() == io1Port->name());
            TS_ASSERT_EQUALS(port.widthFormula(), "32");
            TS_ASSERT(port.loadPort() == "o1_ld");
        } else if (port.name() == "t1") {
            TS_ASSERT(port.architecturePort() == io2Port->name());
        } else if (port.name() == "r1") {
            TS_ASSERT(port.architecturePort() == io3Port->name());
            TS_ASSERT(port.guardPort() == "guard");
        } else {
            TS_ASSERT(false);
        }
    }

    TS_ASSERT(entry1Impl.implementationFileCount() == 1);
    BlockImplementationFile& file = entry1Impl.file(0);
    TS_ASSERT(file.pathToFile() == "/home/tce/addsub.vhdl");
    TS_ASSERT(file.format() == BlockImplementationFile::VHDL);
    
    delete entry1;
    delete entry2;
}


/**
 * Tests getting an RF entry from HDB by ID.
 */
void
HDBManagerTest::testGettingRFByID() {

    HDBManager& manager = HDBRegistry::instance().hdb(HDB_TO_CREATE_1);
    TS_ASSERT_THROWS(manager.rfByEntryID(0), KeyNotFound);

    RFEntry* entry = manager.rfByEntryID(1);
    TS_ASSERT(entry->hasArchitecture());
    TS_ASSERT(entry->hasImplementation());
    TS_ASSERT(entry->id() == 1);
    TS_ASSERT(entry->hdbFile() == HDB_TO_CREATE_1);
    
    // verify the architecture
    RFArchitecture& architecture = entry->architecture();
    TS_ASSERT(!architecture.hasParameterizedWidth());
    TS_ASSERT(!architecture.hasParameterizedSize());
    TS_ASSERT(architecture.width() == 16);
    TS_ASSERT(architecture.size() == 8);
    TS_ASSERT(architecture.readPortCount() == 1);
    TS_ASSERT(architecture.writePortCount() == 1);
    TS_ASSERT(architecture.bidirPortCount() == 0);
    TS_ASSERT(architecture.maxReads() == 1);
    TS_ASSERT(architecture.maxWrites() == 0);
    TS_ASSERT(architecture.latency() == 1);
    TS_ASSERT(architecture.hasGuardSupport());

    // verify the implementation
    RFImplementation& implementation = entry->implementation();
    TS_ASSERT(implementation.moduleName() == "rf_8x16");
    TS_ASSERT(implementation.clkPort() == "clk");
    TS_ASSERT(implementation.rstPort() == "rst");
    TS_ASSERT(implementation.glockPort() == "glock");
    TS_ASSERT(implementation.guardPort() == "guard");
    TS_ASSERT(implementation.sizeParameter() == "");
    TS_ASSERT(implementation.widthParameter() == "");
    TS_ASSERT(implementation.portCount() == 2);
    for (int i = 0; i < implementation.portCount(); i++) {
        RFPortImplementation& port = implementation.port(i);
        if (port.name() == "rd") {
            TS_ASSERT(port.direction() == HDB::OUT);
            TS_ASSERT(port.loadPort() == "rd_ld");
            TS_ASSERT_EQUALS(port.opcodePort(), "rd_opc");
        } else if (port.name() == "wr") {
            TS_ASSERT(port.direction() == HDB::IN);
            TS_ASSERT(port.loadPort() == "wr_ld");
            TS_ASSERT_EQUALS(port.opcodePort(), "wr_opc");
        } else {
            TS_ASSERT(false);
        }
    }
    
    TS_ASSERT(implementation.implementationFileCount() == 1);
    BlockImplementationFile& file = implementation.file(0);
    TS_ASSERT(file.pathToFile() == "/home/tce/rf_8_16.vhdl");
    TS_ASSERT(file.format() == BlockImplementationFile::VHDL);
    
    delete entry;
}


/**
 * Initializes the HDB by the initializehdb script.
 */
void
HDBManagerTest::initializeHDB() {
    FileSystem::runShellCommand("./initializehdb " + HDB_TO_CREATE_1);
}

/**
 * Tests getting cost data.
 */
void 
HDBManagerTest::testGettingCostData() {
    HDBManager& manager = HDBRegistry::instance().hdb(HDB_TO_CREATE_1);
    TS_ASSERT_THROWS_NOTHING(
        manager.socketCostEstimationData("delay", 7, "DefaultICDecoder"));
    TS_ASSERT_EQUALS(
        manager.socketCostEstimationData("delay", 7, "DefaultICDecoder").
        stringValue(), "520.0");
}


/**
 * Tests FU architecture matching.
 */
void
HDBManagerTest::testFUArchitectureMatching() {

    FunctionUnit* fu1 = new FunctionUnit("fu1");
    FUPort* o1Port = new FUPort("o1", 16, *fu1, false, false);
    FUPort* t1Port = new FUPort("t1", 8, *fu1, true, true);
    HWOperation* op1 = new HWOperation("op1", *fu1);
    op1->bindPort(2, *o1Port);
    op1->bindPort(1, *t1Port);
    op1->pipeline()->addPortRead(1, 0, 1);
    op1->pipeline()->addPortWrite(2, 2, 1);
    op1->pipeline()->addResourceUse("res1", 1, 1);
    HWOperation* op2 = new HWOperation("op2", *fu1);
    op2->bindPort(2, *o1Port);
    op2->bindPort(1, *t1Port);
    op2->pipeline()->addPortRead(1, 0, 2);
    op2->pipeline()->addPortWrite(2, 5, 1);
    op2->pipeline()->addResourceUse("res1", 1, 2);
    FUArchitecture* fuArch = new FUArchitecture(fu1);
    fuArch->setParameterizedWidth("o1");
    fuArch->setParameterizedWidth("t1");

    FileSystem::removeFileOrDirectory(HDB_TO_CREATE_6);
    HDBManager& manager = CachedHDBManager::createNew(HDB_TO_CREATE_6);
    RowID archID = manager.addFUArchitecture(*fuArch);
    RowID entryID = manager.addFUEntry();
    manager.setArchitectureForFU(entryID, archID);
    
    std::set<RowID> fusByArchitecture = manager.fuEntriesByArchitecture(
        *fu1);
    TS_ASSERT(AssocTools::containsKey(fusByArchitecture, entryID));

    // create more restrictive pipeline
    op2->pipeline()->addResourceUse("res1", 3, 1);
    fusByArchitecture = manager.fuEntriesByArchitecture(*fu1);
    TS_ASSERT(AssocTools::containsKey(fusByArchitecture, entryID));

    // create less restrictive pipeline
    op2->pipeline()->removeResourceUse("res1");
    fusByArchitecture = manager.fuEntriesByArchitecture(*fu1);
    TS_ASSERT(fusByArchitecture.empty());

    delete fuArch;
}


/**
 * Tests RF architecture matching.
 */
void
HDBManagerTest::testRFArchitectureMatching() {

    RFArchitecture* rfArch = new RFArchitecture(2, 2, 0, 2, 2, 1, true, 0);
    
    FileSystem::removeFileOrDirectory(HDB_TO_CREATE_7);
    HDBManager& manager = CachedHDBManager::createNew(HDB_TO_CREATE_7);
    RowID archID = manager.addRFArchitecture(*rfArch);
    RowID entryID = manager.addRFEntry();
    manager.setArchitectureForRF(entryID, archID);
    
    std::set<RowID> rfEntries = manager.rfEntriesByArchitecture(
        2, 2, 0, 1, 1, 1, false);
    TS_ASSERT(AssocTools::containsKey(rfEntries, entryID));

    rfEntries = manager.rfEntriesByArchitecture(2, 2, 0, 2, 2, 2, true, 1);
    TS_ASSERT(rfEntries.empty());

    delete rfArch;
}

/**
 * Test getting fuArchitectures base of operation set.
 */
void 
HDBManagerTest::testFUArchitectureIDbyOperationSet() {

    FunctionUnit* fu1 = new FunctionUnit("fu1");
    FUPort* o1Port = new FUPort("o1", 16, *fu1, false, false);
    FUPort* t1Port = new FUPort("t1", 8, *fu1, true, true);
    HWOperation* op1 = new HWOperation("op1", *fu1);
    op1->bindPort(2, *o1Port);
    op1->bindPort(1, *t1Port);
    op1->pipeline()->addPortRead(1, 0, 1);
    op1->pipeline()->addPortWrite(2, 2, 1);
    op1->pipeline()->addResourceUse("res1", 1, 1);
    HWOperation* op2 = new HWOperation("op2", *fu1);
    op2->bindPort(2, *o1Port);
    op2->bindPort(1, *t1Port);
    op2->pipeline()->addPortRead(1, 0, 2);
    op2->pipeline()->addPortWrite(2, 5, 1);
    op2->pipeline()->addResourceUse("res1", 1, 2);
    FUArchitecture* fuArch = new FUArchitecture(fu1);
    fuArch->setParameterizedWidth("o1");
    fuArch->setParameterizedWidth("t1");

    FunctionUnit* fu2 = new FunctionUnit("fu2");
    FUPort* o2Port = new FUPort("o2", 16, *fu2, false, false);
    FUPort* t2Port = new FUPort("t2", 8, *fu2, true, true);
    HWOperation* op3 = new HWOperation("op3", *fu2);
    op3->bindPort(2, *o2Port);
    op3->bindPort(1, *t2Port);
    op3->pipeline()->addPortRead(1, 0, 1);
    op3->pipeline()->addPortWrite(2, 2, 1);
    op3->pipeline()->addResourceUse("res1", 1, 1);
    HWOperation* op4 = new HWOperation("op4", *fu2);
    op4->bindPort(2, *o2Port);
    op4->bindPort(1, *t2Port);
    op4->pipeline()->addPortRead(1, 0, 2);
    op4->pipeline()->addPortWrite(2, 5, 1);
    op4->pipeline()->addResourceUse("res1", 1, 2);

    // same operation that fu1 has
    HWOperation* op5 = new HWOperation("op1", *fu2);
    op5->bindPort(2, *o2Port);
    op5->bindPort(1, *t2Port);
    op5->pipeline()->addPortRead(1, 0, 1);
    op5->pipeline()->addPortWrite(2, 2, 1);
    op5->pipeline()->addResourceUse("res1", 1, 1);

    FUArchitecture* fuArch2 = new FUArchitecture(fu2);
    fuArch2->setParameterizedWidth("o2");
    fuArch2->setParameterizedWidth("t2");
    
    FileSystem::removeFileOrDirectory(HDB_TO_CREATE_8);
    HDBManager& manager = CachedHDBManager::createNew(HDB_TO_CREATE_8);

    // add one entry to make sure we test architecture IDs instead of entry IDs
    manager.addFUEntry();
    RowID archID1 = manager.addFUArchitecture(*fuArch);
    RowID entryID1 = manager.addFUEntry();
    manager.setArchitectureForFU(entryID1, archID1);
    RowID archID2 = manager.addFUArchitecture(*fuArch2);
    RowID entryID2 = manager.addFUEntry();
    manager.setArchitectureForFU(entryID2, archID2);

    // test to match first fu
    std::set<std::string> operations;
    operations.insert("op2");
    std::set<RowID> fuArchitectureIDsByOperationSet =
        manager.fuArchitectureIDsByOperationSet(operations);
    TS_ASSERT(
        AssocTools::containsKey(fuArchitectureIDsByOperationSet, archID1));
    TS_ASSERT(
        !AssocTools::containsKey(fuArchitectureIDsByOperationSet, archID2));

    // test matching both fus with different operations
    operations.insert("op3");
    std::set<RowID> fuArchitectureIDsByOperationSet2 =
        manager.fuArchitectureIDsByOperationSet(operations);
    TS_ASSERT(
        AssocTools::containsKey(fuArchitectureIDsByOperationSet2, archID1));
    TS_ASSERT(
        AssocTools::containsKey(fuArchitectureIDsByOperationSet2, archID2));

    // test matching both fus with same operation
    operations.clear();
    operations.insert("op1");
    std::set<RowID> fuArchitectureIDsByOperationSet3 =
        manager.fuArchitectureIDsByOperationSet(operations);
    TS_ASSERT(
        AssocTools::containsKey(fuArchitectureIDsByOperationSet3, archID1));
    TS_ASSERT(
        AssocTools::containsKey(fuArchitectureIDsByOperationSet3, archID2));

    // test with operation names that doesn't exists
    operations.clear();
    operations.insert("op6");
    std::set<RowID> fuArchitectureIDsByOperationSet4 =
        manager.fuArchitectureIDsByOperationSet(operations);
    TS_ASSERT(
        !AssocTools::containsKey(fuArchitectureIDsByOperationSet4, archID1));
    TS_ASSERT(
        !AssocTools::containsKey(fuArchitectureIDsByOperationSet4, archID2));
    
    // test with empty set of operation names
    operations.clear();
    std::set<RowID> fuArchitectureIDsByOperationSet5 =
        manager.fuArchitectureIDsByOperationSet(operations);
    TS_ASSERT(
        !AssocTools::containsKey(fuArchitectureIDsByOperationSet5, archID1));
    TS_ASSERT(
        !AssocTools::containsKey(fuArchitectureIDsByOperationSet5, archID2));
}

#endif
