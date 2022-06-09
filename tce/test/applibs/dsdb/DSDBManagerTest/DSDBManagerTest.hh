/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file DSBManagerTest.hh 
 *
 * A test suite for DSDBManager.
 * 
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DSDB_MANAGER_TEST_HH
#define TTA_DSDB_MANAGER_TEST_HH

#include <string>
#include <TestSuite.h>

#include "FileSystem.hh"
#include "DSDBManager.hh"

#include "IDFSerializer.hh"
#include "ADFSerializer.hh"
#include "Machine.hh"

using std::string;

static const std::string DSDB_TEST_FILE_1 = "dsdb1.ddb";
static const std::string DSDB_TEST_FILE_2 = "dsdb2.ddb";

/**
 * Class that tests DSDBManager class.
 */
class DSDBManagerTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreatingDSDB();
    void testDSDB();
};


/**
 * Called before each test.
 */
void
DSDBManagerTest::setUp() {
}


/**
 * Called after each test.
 */
void
DSDBManagerTest::tearDown() {
}


/**
 * Tests creating a new DSDB.
 */
void
DSDBManagerTest::testCreatingDSDB() {
    FileSystem::removeFileOrDirectory(DSDB_TEST_FILE_1);
    DSDBManager* manager = NULL;
    TS_ASSERT_THROWS_NOTHING(
        manager = DSDBManager::createNew(DSDB_TEST_FILE_1));
    delete manager;
    TS_ASSERT(FileSystem::fileExists(DSDB_TEST_FILE_1));   
}

/**
 * Tests basic DSDB manager functionality.
 */
void
DSDBManagerTest::testDSDB() {

    FileSystem::removeFileOrDirectory(DSDB_TEST_FILE_2);
    DSDBManager* manager = NULL;
    TS_ASSERT_THROWS_NOTHING(
        manager = DSDBManager::createNew(DSDB_TEST_FILE_2));

    // Test architectures.
    ADFSerializer adfSerializer;
    adfSerializer.setSourceFile("data/test.adf");
    TTAMachine::Machine* mach = adfSerializer.readMachine();
    RowID archID = manager->addArchitecture(*mach);
    TTAMachine::Machine* mach2 = manager->architecture(archID);

    TS_ASSERT(
        mach->functionUnitNavigator().count() ==
        mach2->functionUnitNavigator().count());

    TS_ASSERT(
        mach->functionUnitNavigator().item(0)->operationCount() ==
        mach2->functionUnitNavigator().item(0)->operationCount());

    delete mach;
    delete mach2;

    IDF::IDFSerializer idfSerializer;
    idfSerializer.setSourceFile("data/test.idf");
    IDF::MachineImplementation* impl =
        idfSerializer.readMachineImplementation();

    double longestPathDelayEstimate = 0.000000001;
    int areaEstimate = 100000;

    RowID implID = manager->addImplementation(
        *impl, longestPathDelayEstimate, areaEstimate);

    // Test machine configurations.
    DSDBManager::MachineConfiguration conf1(archID, false, -1);
    DSDBManager::MachineConfiguration conf2(archID, true, implID);
    RowID confID1 = manager->addConfiguration(conf1);
    RowID confID2 = manager->addConfiguration(conf2);
    TS_ASSERT(manager->hasConfiguration(confID1));
    TS_ASSERT(manager->hasConfiguration(confID2));

    DSDBManager::MachineConfiguration conf1db =
        manager->configuration(confID1);

    DSDBManager::MachineConfiguration conf2db =
        manager->configuration(confID2);

    TS_ASSERT(conf1db.architectureID == archID);
    TS_ASSERT(conf1db.hasImplementation == false);
    TS_ASSERT(conf2db.architectureID == archID);
    TS_ASSERT(conf2db.hasImplementation == true);
    TS_ASSERT(conf2db.implementationID == implID);

    std::set<RowID> confIDs = manager->configurationIDs();
    TS_ASSERT(confID1 != confID2);
    TS_ASSERT(confIDs.size() == 2);
    TS_ASSERT(confIDs.count(confID1) == 1);
    TS_ASSERT(confIDs.count(confID2) == 1);

    // Test configuration deletion.
    manager->removeConfiguration(confID1);
    TS_ASSERT(!manager->hasConfiguration(confID1));
    TS_ASSERT(manager->hasConfiguration(confID2));

    // Applications
    const std::string path1 = "/path/to/application1";
    const std::string path2 = "/path/to/application2";
    const std::string path3 = "/path/to/application3";
    RowID appID1 = manager->addApplication(path1);
    RowID appID3 = manager->addApplication(path3);
    RowID appID2 = manager->addApplication(path2);

    TS_ASSERT(manager->hasApplication(appID1));
    TS_ASSERT(manager->hasApplication(appID2));
    TS_ASSERT(manager->hasApplication(appID3));
    TS_ASSERT(manager->applicationPath(appID1) == path1);
    TS_ASSERT(manager->applicationPath(appID2) == path2);
    TS_ASSERT(manager->applicationPath(appID3) == path3);

    // application IDs
    std::set<RowID> appIDs = manager->applicationIDs();
    TS_ASSERT(appIDs.size() == 3);

    // architecture IDs
    std::set<RowID> archIDs = manager->architectureIDs();
    TS_ASSERT(archIDs.size() == 1);

    // configuration IDs for an architecture
    std::set<RowID>::const_iterator iter = archIDs.begin();
    TS_ASSERT(*iter == archID);
    std::set<RowID> archConfIDs = manager->archConfigurationIDs(archID);
    TS_ASSERT(archConfIDs.size() == 1);
    std::set<RowID>::const_iterator iter2 = archConfIDs.begin();
    TS_ASSERT(*iter2 == confID2);


    // Energy
    TS_ASSERT(!manager->hasEnergyEstimate(appID1, implID));
    TS_ASSERT(!manager->hasEnergyEstimate(appID2, implID));
    TS_ASSERT(!manager->hasEnergyEstimate(appID3, implID));
    double energy = 42;
    manager->addEnergyEstimate(appID3, implID, energy);
    TS_ASSERT(!manager->hasEnergyEstimate(appID1, implID));
    TS_ASSERT(!manager->hasEnergyEstimate(appID2, implID));
    TS_ASSERT(manager->hasEnergyEstimate(appID3, implID));
    TS_ASSERT(manager->energyEstimate(appID3, implID) == energy);

    // Cycle count
    TS_ASSERT(!manager->hasCycleCount(appID1, archID));
    TS_ASSERT(!manager->hasCycleCount(appID2, archID));
    TS_ASSERT(!manager->hasCycleCount(appID3, archID));
    ClockCycleCount cc = 123456;
    manager->addCycleCount(appID2, archID, cc);
    TS_ASSERT(!manager->hasCycleCount(appID1, archID));
    TS_ASSERT(manager->hasCycleCount(appID2, archID));
    TS_ASSERT(!manager->hasCycleCount(appID3, archID));
    TS_ASSERT(manager->cycleCount(appID2, archID) == cc);

    // Area estimate
    TS_ASSERT_EQUALS(manager->areaEstimate(implID), areaEstimate);
    areaEstimate = 123456;
    manager->setAreaEstimate(implID, areaEstimate);
    TS_ASSERT_EQUALS(manager->areaEstimate(implID), areaEstimate);

    // Longest path delay estimate
    TS_ASSERT_DELTA(
        manager->longestPathDelayEstimate(
            implID), longestPathDelayEstimate, 1e-8);
    longestPathDelayEstimate = 123.45;
    manager->setLongestPathDelayEstimate(implID, longestPathDelayEstimate);
    TS_ASSERT_DELTA(
        manager->longestPathDelayEstimate(
            implID), longestPathDelayEstimate, 1e-8);

    // file writing
    manager->writeArchitectureToFile(archID, "data/1.adf");
    manager->writeImplementationToFile(implID, "data/1.idf");
    std::string command = "diff data/test.adf data/1.adf > /dev/null";
    TS_ASSERT(FileSystem::runShellCommand(command));
    command = "diff data/test.idf data/1.idf > /dev/null";
    TS_ASSERT(FileSystem::runShellCommand(command));

    // getting data
    std::vector<DSDBManager::ConfigurationCosts> costs;
    costs = 
        manager->applicationCostEstimatesByConf(
            DSDBManager::ORDER_BY_APPLICATION);
    unsigned int expectedResultSize = 3;
    TS_ASSERT_EQUALS(costs.size(), expectedResultSize);
    TS_ASSERT_EQUALS(costs[0].application, path1);
    TS_ASSERT_EQUALS(costs[1].application, path2);
    TS_ASSERT_EQUALS(costs[2].application, path3);
    TS_ASSERT_EQUALS(costs[0].configurationID, confID2);
    TS_ASSERT_EQUALS(costs[1].configurationID, confID2);
    TS_ASSERT_EQUALS(costs[2].configurationID, confID2);
    TS_ASSERT_EQUALS(costs[0].energyEstimate, 0);
    TS_ASSERT_EQUALS(costs[1].energyEstimate, 0);
    TS_ASSERT_EQUALS(costs[2].energyEstimate, energy);
    TS_ASSERT(costs[0].cycleCount == 0);
    TS_ASSERT_EQUALS(costs[1].cycleCount, cc);
    TS_ASSERT(costs[2].cycleCount == 0);    

    delete manager;
    TS_ASSERT(FileSystem::fileExists(DSDB_TEST_FILE_1));
}

#endif
