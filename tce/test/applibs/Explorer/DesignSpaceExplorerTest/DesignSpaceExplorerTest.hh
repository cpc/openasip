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
            "../../../../data/mach/minimal_be.adf");
    DSDBManager::MachineConfiguration conf;
    conf.architectureID = dsdb->addArchitecture(*adf);
    //conf.implementationID = dsdb->addImplementation(*idf, 0, 0);
    conf.hasImplementation = false;
    //RowID confID = 
    dsdb->addConfiguration(conf);
    //RowID appID = 
#ifdef LLVM_OLDER_THAN_3_7
    dsdb->addApplication("data/TestApp-old");
    //RowID appID2 = 
    dsdb->addApplication("data/TestApp2-old");
#else
    dsdb->addApplication("data/TestApp");
    //RowID appID2 = 
    dsdb->addApplication("data/TestApp2");
#endif
    
    DesignSpaceExplorer explorer;
    explorer.setDSDB(*dsdb);
    CostEstimates results;
    bool estimate = false;
    Application::setVerboseLevel(0);
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
