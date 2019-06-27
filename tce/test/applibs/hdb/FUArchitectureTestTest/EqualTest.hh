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
 * @file FUArchitectureTest.hh 
 *
 * A test suite for FUArchihtecture.
 * 
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
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

    TS_ASSERT_DIFFERS(fuArch1, fuArch2);
    TS_ASSERT_DIFFERS(fuArch2, fuArch3);
    TS_ASSERT_DIFFERS(fuArch1, fuArch3);

}

#endif
