/** 
 * @file MachineResourceModifierTest.hh 
 *
 * A test suite testing DesignSpaceMachineResourceModifier class functions.
 * 
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
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
    mach.addBus(bus);
    mach.addBus(bus1);
    mach.addBus(bus2);
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

    modifier.increaseAllRFsThatDiffersByAmount(3, mach);

    TS_ASSERT_EQUALS(rfNav.count(), 4);
    RegisterFile rf2("rf2_0", 2, 16, 1, 1, 0, RegisterFile::VOLATILE);
    mach.addRegisterFile(rf2);
    TS_ASSERT_EQUALS(rfNav.count(), 5);

    modifier.increaseAllRFsThatDiffersByAmount(1, mach);
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
    modifier.increaseAllRFsThatDiffersByAmount(1, mach);
    TS_ASSERT_EQUALS(rfNav.count(), 11);
}

#endif
