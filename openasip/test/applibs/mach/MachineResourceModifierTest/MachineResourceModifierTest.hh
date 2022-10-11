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
 * @file MachineResourceModifierTest.hh 
 *
 * A test suite testing DesignSpaceMachineResourceModifier class functions.
 * 
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_RESOURCE_MODIFIER_TEST_HH
#define TTA_MACHINE_RESOURCE_MODIFIER_TEST_HH

#include <string>
#include <TestSuite.h>
#include <map>

#include "Application.hh"
#include "MachineResourceModifier.hh"
#include "Exception.hh"
#include "Machine.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "ADFSerializer.hh"
#include "RegisterFile.hh"
#include "Conversion.hh"

using namespace TTAMachine;

using std::string;

/**
 * Class that tests machine resource modifier.
 */
class MachineResourcemodifierTest : public CxxTest::TestSuite {
public:
    void testAddBusesByAmount();
};

/**
 * Test bus adding.
 */
void
MachineResourcemodifierTest::testAddBusesByAmount() {

    Machine mach;
    Bus bus("bus0", 22, 2, Machine::ZERO);
    Bus bus1("bus1", 22, 2, Machine::ZERO);
    Bus bus2("toinen", 26, 1, Machine::SIGN);
    Segment segment("bus", bus);
    Segment segment1("bus1", bus1);
    Segment segment2("bus2", bus2);
    mach.addBus(bus);
    mach.addBus(bus1);
    mach.addBus(bus2);

    Machine::BusNavigator busNav = mach.busNavigator();
    TS_ASSERT_EQUALS(busNav.count(), 3);
    MachineResourceModifier modifier;
    modifier.addBusesByAmount(3, mach);

    TS_ASSERT_EQUALS(busNav.count(), 6);

    // test that all added buses have one segment
    for (int i = 0; i < 6; i++) {
        TTAMachine::Bus& bus = *busNav.item(i);
        TS_ASSERT_EQUALS(bus.segmentCount(), 1);
    }

    RegisterFile* rf = 
        new TTAMachine::RegisterFile(
            "rf0", 4, 32, 2, 2, 0, RegisterFile::NORMAL);
    for (int n = 0; n < 2; n++) {
        new TTAMachine::RFPort(
            "write" + Conversion::toString(n + 1), *rf);
    }
    for (int n = 0; n < 2; n++) {
        new TTAMachine::RFPort(
            "read" + Conversion::toString(n + 1), *rf);
    }
    mach.addRegisterFile(*rf);
    
    Machine::RegisterFileNavigator rfNav = mach.registerFileNavigator();
    TS_ASSERT_EQUALS(rfNav.count(), 1);

    try {
        modifier.increaseAllRFsThatDiffersByAmount(3, mach);
    } catch (Exception& e) {
        TS_FAIL(e.errorMessage());
    }

    TS_ASSERT_EQUALS(rfNav.count(), 4);
    RegisterFile rf2("rf2_0", 2, 16, 1, 1, 0, RegisterFile::VOLATILE);
    mach.addRegisterFile(rf2);
    TS_ASSERT_EQUALS(rfNav.count(), 5);

    try {
        modifier.increaseAllRFsThatDiffersByAmount(1, mach);
    } catch (Exception& e) {
        TS_FAIL(e.errorMessage());
    }
    TS_ASSERT_EQUALS(rfNav.count(), 7);
    RegisterFile* rf3 = 
        new TTAMachine::RegisterFile(
            "rf3_0", 4, 32, 2, 2, 0, RegisterFile::NORMAL);
    for (int n = 0; n < 2; n++) {
        new TTAMachine::RFPort(
            "write" + Conversion::toString(n + 1), *rf3);
    }
    for (int n = 0; n < 1; n++) {
        new TTAMachine::RFPort(
            "read" + Conversion::toString(n + 1), *rf3);
    }

    mach.addRegisterFile(*rf3);
    TS_ASSERT_EQUALS(rfNav.count(), 8);
    try {
        modifier.increaseAllRFsThatDiffersByAmount(1, mach);
    } catch (Exception& e) {
        TS_FAIL(e.errorMessage());
    }
    TS_ASSERT_EQUALS(rfNav.count(), 11);
}

#endif
