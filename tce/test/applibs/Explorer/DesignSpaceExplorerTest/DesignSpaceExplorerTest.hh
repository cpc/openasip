/** 
 * @file DesignSpaceExplorerTest.hh 
 *
 * A test suite for DesignSpaceExplorer class.
 * 
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_DESIGN_SPACE_EXPLORER_TEST_HH
#define TTA_DESIGN_SPACE_EXPLORER_TEST_HH

#include <string>
#include <TestSuite.h>
#include <map>

#include "DesignSpaceExplorer.hh"
#include "DSDBManager.hh"
#include "FileSystem.hh"
#include "CostEstimates.hh"
#include "MachineImplementation.hh"
#include "Machine.hh"


/**
 * Class that tests DesignSpaceExplorer class.
 */
class DesignSpaceExplorerTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testSchedule();
    void testSimulate();
    void testEvaluate();

private:

};


/**
 * Called before each test.
 */
void
DesignSpaceExplorerTest::setUp() {
}


/**
 * Called after each test.
 */
void
DesignSpaceExplorerTest::tearDown() {
}


/**
 * Test schedule function.
 */
void
DesignSpaceExplorerTest::testSchedule() {
}

/**
 * Test simulate function.
 */
void
DesignSpaceExplorerTest::testSimulate() {


}


/**
 * Test evaluate function.
 */
void
DesignSpaceExplorerTest::testEvaluate() {

    FileSystem::removeFileOrDirectory("data/test.dsdb");
    DSDBManager* dsdb = DSDBManager::createNew("data/test.dsdb");
    IDF::MachineImplementation* idf = 
        IDF::MachineImplementation::loadFromIDF(
            "data/10_bus_full_connectivity.idf");
    TTAMachine::Machine* adf =
        TTAMachine::Machine::loadFromADF(
            "data/10_bus_full_connectivity.adf");
    DSDBManager::MachineConfiguration conf;
    conf.architectureID = dsdb->addArchitecture(*adf);
    conf.implementationID = dsdb->addImplementation(*idf, 0, 0);
    conf.hasImplementation = true;
    //RowID confID = 
    dsdb->addConfiguration(conf);
    //RowID appID = 
    dsdb->addApplication("data/TestApp");
    //RowID appID2 = 
    dsdb->addApplication("data/TestApp2");
    
    DesignSpaceExplorer explorer;
    explorer.setDSDB(*dsdb);
    CostEstimates results;
    TS_ASSERT(explorer.evaluate(conf, results, true));
    //std::cout << "result area: " << results.area() << std::endl;
    //std::cout << "result delay: " 
    // << results.longestPathDelay() << std::endl;
    // there should be one energy estimate
    TS_ASSERT(results.energies() == 2);
    if (results.energies() == 2) {
        // the energy estimate must not be equal to zero    
        TS_ASSERT_DIFFERS(results.energy(0), 0);
        TS_ASSERT_DIFFERS(results.energy(1), 0);
        //std::cout << "result energy: " << results.energy(0) << std::endl;
    }
}

#endif
