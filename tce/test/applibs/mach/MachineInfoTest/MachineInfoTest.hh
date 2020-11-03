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
 * @file MachineInfoTest.hh
 *
 * A test suite for MachineInfo functions.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef MACHINE_INFO_TEST_HH
#define MACHINE_INFO_TEST_HH

#include <TestSuite.h>
#include "MachineInfo.hh"
#include "Machine.hh"

class MachineInfoTest : public CxxTest::TestSuite {
public:
    MachineInfoTest();

    void setUp();
    void tearDown();

    void testGetOpset();
private:
};

MachineInfoTest::MachineInfoTest() {
}

void 
MachineInfoTest::setUp() {
}

void 
MachineInfoTest::tearDown() {
}

/**
 * Tests that the requesting opset of machine works.
 */
void
MachineInfoTest::testGetOpset() {

    TTAMachine::Machine* targetMachine = NULL;
    
    targetMachine = 
        TTAMachine::Machine::loadFromADF(
            "data/10_bus_reduced_connectivity.adf");
    
    MachineInfo::OperationSet opSet = MachineInfo::getOpset(*targetMachine);

    TS_ASSERT_DIFFERS(opSet.find("ABS"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("ABSF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("ADD"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("ADDF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("AND"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("CFI"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("CIF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("DIV"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("DIVU"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("DIVF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("EQ"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("EQF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("GT"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("GTF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("GTU"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("IOR"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("LDD"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("LDH"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("LDQ"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("LDQU"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("LDW"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MAX"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MAXU"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MIN"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MINU"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MOD"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MODU"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MUL"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MULF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("NEG"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("NEGF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("OUTPUTDATA"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("PRINT_INT"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("ROTL"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("ROTR"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("SHL"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("SHR"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("SHRU"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("STD"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("STDOUT"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("STH"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("STQ"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("STW"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("SUB"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("SUBF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("SXHW"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("SXQW"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("XOR"), opSet.end());
    TS_ASSERT_EQUALS(opSet.size(), static_cast<unsigned int>(48));

    
    delete targetMachine;
}

#endif
