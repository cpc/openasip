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
    TS_ASSERT_EQUALS(bus.segmentCount(), 1);
    TS_ASSERT_EQUALS(bus1.segmentCount(), 1);
    TS_ASSERT_EQUALS(bus2.segmentCount(), 1);

    Machine::BusNavigator busNav = mach.busNavigator();
    TS_ASSERT_EQUALS(busNav.count(), 3);
    MachineResourceModifier modifier;
    modifier.addBusesByAmount(3, mach);

    TS_ASSERT_EQUALS(busNav.count(), 6);

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
