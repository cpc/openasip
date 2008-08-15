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
 * @file FUArchitectureTest.hh 
 *
 * A test suite for FUArchihtecture.
 * 
 * @author Jari Mäntyneva 2006 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_ARCHITECTURE_TEST_HH
#define TTA_FU_ARCHITECTURE_TEST_HH

#include <string>
#include <TestSuite.h>

#include "FUArchitecture.hh"
#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"

#include <iostream>
using std::cerr;
using std::endl;
using std::string;
using namespace HDB;
using namespace TTAMachine;

/**
 * Class that tests FUArchitecture class.
 */
class FUArchitectureTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testEquals();

private:

};


/**
 * Called before each test.
 */
void
FUArchitectureTest::setUp() {
}


/**
 * Called after each test.
 */
void
FUArchitectureTest::tearDown() {
}


/**
 * Tests creating loading HDBs.
 */
void
FUArchitectureTest::testEquals() {

    FunctionUnit* fu1 = new FunctionUnit("fu1");
    FUPort* port1 = new FUPort("port1", 16, *fu1, false, false);
    HWOperation* op1 = new HWOperation("op1", *fu1);
    op1->bindPort(1, *port1);
    ExecutionPipeline* pLine1 = op1->pipeline();
    pLine1->addResourceUse("res1", 0, 3);

    FUArchitecture* fuArch1 = new FUArchitecture(fu1);

    FunctionUnit* fu2 = new FunctionUnit("fu2");
    FUPort* port2 = new FUPort("port2", 8, *fu2, false, false);
    HWOperation* op2 = new HWOperation("op2", *fu2);
    op2->bindPort(1, *port2);
    ExecutionPipeline* pLine2 = op2->pipeline();
    pLine2->addResourceUse("res2", 1, 3);

    FUArchitecture* fuArch2 = new FUArchitecture(fu2);

    FunctionUnit* fu3 = new FunctionUnit("fu3");
    FUPort* port3 = new FUPort("port3", 16, *fu3, false, false);
    HWOperation* op3 = new HWOperation("op3", *fu3);
    op3->bindPort(1, *port3);
    ExecutionPipeline* pLine3 = op3->pipeline();
    pLine3->addResourceUse("res3", 0, 4);

    FUArchitecture* fuArch3 = new FUArchitecture(fu3);

    TS_ASSERT(fuArch1 == fuArch1);
    TS_ASSERT_DIFFERS(fuArch1, fuArch2);
    TS_ASSERT_DIFFERS(fuArch2, fuArch3);
    TS_ASSERT_DIFFERS(fuArch1, fuArch3);

}

#endif
