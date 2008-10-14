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
 * @file DesignSpaceExplorerTest.hh 
 *
 * A test suite for DesignSpaceExplorer class.
 * 
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
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
    /*
       not needed until estimating is enabled again
    IDF::MachineImplementation* idf = 
        IDF::MachineImplementation::loadFromIDF(
            "data/minimal.idf");
    */
    // path to minimal adf under project root
    TTAMachine::Machine* adf =
        TTAMachine::Machine::loadFromADF(
            "../../../../data/mach/minimal.adf");
    DSDBManager::MachineConfiguration conf;
    conf.architectureID = dsdb->addArchitecture(*adf);
    //conf.implementationID = dsdb->addImplementation(*idf, 0, 0);
    conf.hasImplementation = false;
    //RowID confID = 
    dsdb->addConfiguration(conf);
    //RowID appID = 
    dsdb->addApplication("data/TestApp");
    //RowID appID2 = 
    dsdb->addApplication("data/TestApp2");
    
    DesignSpaceExplorer explorer;
    explorer.setDSDB(*dsdb);
    CostEstimates results;
    bool estimate = false;
    //Application::setVerboseLevel(1);
    TS_ASSERT(explorer.evaluate(conf, results, estimate));
    
    //std::cout << "result area: " << results.area() << std::endl;
    //std::cout << "result delay: " 
    // << results.longestPathDelay() << std::endl;
    // there should be one energy estimate
    
    /*
    This is disabled until, some machine with estimate info in default 
    hdb is found.
    std::cout << "energies: " << results.energies() << std::endl << std::endl;
    TS_ASSERT(results.energies() == 2);
    if (results.energies() == 2) {
        // the energy estimate must not be equal to zero    
        TS_ASSERT_DIFFERS(results.energy(0), 0);
        TS_ASSERT_DIFFERS(results.energy(1), 0);
        //std::cout << "result energy: " << results.energy(0) << std::endl;
    }
    */
}

#endif
