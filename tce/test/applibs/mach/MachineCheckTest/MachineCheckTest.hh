/**
 * @file MachineCheckTest.hh
 *
 * A test suite for the basic block selector.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef MACHINE_CHECK_TEST_HH
#define MACHINE_CHECK_TEST_HH

#include <TestSuite.h>
#include "MachineConnectivityCheck.hh"
#include "FUPort.hh"
#include "Machine.hh"
#include "OperationBindingCheck.hh"
#include "RegisterQuantityCheck.hh"

class MachineCheckTest : public CxxTest::TestSuite {
public:
    MachineCheckTest();

    void setUp();
    void tearDown();

    void testMachineConnectivityCheck();
    void testOperationBindingCheck();
    void testRegisterQuantityCheck();

private:
    const TTAMachine::BaseFUPort& findFUPort(
        const TTAMachine::Machine& mach, 
        const std::string& fuName,
        const std::string& portName);

    const TTAMachine::RFPort& findRFPort(
        const TTAMachine::Machine& mach, 
        const std::string& rfName,
        const std::string& portName);

};

MachineCheckTest::MachineCheckTest() {
}

void 
MachineCheckTest::setUp() {
}

void 
MachineCheckTest::tearDown() {
}


const TTAMachine::BaseFUPort& 
MachineCheckTest::findFUPort(
    const TTAMachine::Machine& mach, 
    const std::string& fuName,
    const std::string& portName) {

     TTAMachine::Machine::FunctionUnitNavigator fuNav = 
         mach.functionUnitNavigator();

     return *fuNav.item(fuName)->port(portName);
}

const TTAMachine::RFPort&
MachineCheckTest::findRFPort(
    const TTAMachine::Machine& mach, 
    const std::string& rfName,
    const std::string& portName) {

    TTAMachine::Machine::RegisterFileNavigator rfNav = 
        mach.registerFileNavigator();

    return *rfNav.item(rfName)->port(portName);    
}

/**
 * Tests that the parent class MachineConnectivityCheck's helper functions
 * work.
 */
void
MachineCheckTest::testMachineConnectivityCheck() {

    TTAMachine::Machine* targetMachine = NULL;

    CATCH_ANY(
        targetMachine = 
        TTAMachine::Machine::loadFromADF(
            "data/10_bus_reduced_connectivity.adf"));

    TS_ASSERT(
        MachineConnectivityCheck::isConnected(
            findFUPort(*targetMachine, "fu16", "r0"),
            findFUPort(*targetMachine, "fu16", "trigger")));

    TS_ASSERT(
        !MachineConnectivityCheck::isConnected(
            findFUPort(*targetMachine, "fu15", "r0"),
            findRFPort(*targetMachine, "integer0", "wr0")));
}

void
MachineCheckTest::testOperationBindingCheck() {

    OperationBindingCheck opc;
    
    TTAMachine::Machine* okMachine = NULL;
    TTAMachine::Machine* brokenMachine = NULL;

    CATCH_ANY(
        okMachine = 
        TTAMachine::Machine::loadFromADF(
            "data/10_bus_reduced_connectivity.adf"));


    // check that it dos not report erros on a correct machine
    MachineCheckResults res;
    // should return false;
    TS_ASSERT(opc.check(*okMachine, res));
    TS_ASSERT_EQUALS(res.errorCount(),0);
    delete okMachine;

    // check broken machines.

    CATCH_ANY(
        brokenMachine =
        TTAMachine::Machine::loadFromADF(
            "data/no_trigger.adf"));
    // should return false;
    TS_ASSERT(!opc.check(*brokenMachine, res));
    TS_ASSERT_EQUALS(res.errorCount(),1);

/* This is already caught during the ADF loading/machine construction.

    CATCH_ANY(
        brokenMachine = TTAMachine::Machine::loadFromADF(
            "data/2triggers.adf"));
    // should return false;
    TS_ASSERT(!opc.check(*brokenMachine, res));
    TS_ASSERT_EQUALS(res.errorCount(),2);
    std::cout << "next text" << std::endl;

    for ( ; i < res.errorCount(); i++) {
        std::cout << res.error(i).second << std::endl;
    }
*/

    CATCH_ANY(
        brokenMachine = TTAMachine::Machine::loadFromADF(
            "data/noread.adf"));
    // should return false;
    TS_ASSERT(!opc.check(*brokenMachine, res));
    TS_ASSERT_EQUALS(res.errorCount(),2);

    CATCH_ANY(
        brokenMachine = TTAMachine::Machine::loadFromADF(
            "data/nowrite.adf"));
    // should return false;
    TS_ASSERT(!opc.check(*brokenMachine, res));
    TS_ASSERT_EQUALS(res.errorCount(),3);    

/** This is already caught during the ADF loading/machine construction.
    CATCH_ANY(
        brokenMachine = TTAMachine::Machine::loadFromADF(
            "data/wrong_opcodesetting.adf"));

    // should return false;
    TS_ASSERT(!opc.check(*brokenMachine, res));
*/
    CATCH_ANY(
        brokenMachine = TTAMachine::Machine::loadFromADF(
            "data/trigger_not_used.adf"));
    // should return false;
    TS_ASSERT(!opc.check(*brokenMachine, res));
    TS_ASSERT_EQUALS(res.errorCount(),4);

}

void MachineCheckTest::testRegisterQuantityCheck() {

    TTAMachine::Machine* okMachine = NULL;
    TTAMachine::Machine* guardBrokenMachine = NULL;
    TTAMachine::Machine* intBrokenMachine = NULL;
    TTAMachine::Machine* brokenMachine = NULL;


    CATCH_ANY(
        guardBrokenMachine = 
        TTAMachine::Machine::loadFromADF(
            "data/10_bus_reduced_connectivity.adf"));

    CATCH_ANY(
        okMachine = 
        TTAMachine::Machine::loadFromADF(
            "data/minimal.adf"));

    CATCH_ANY(
        intBrokenMachine = 
        TTAMachine::Machine::loadFromADF(
            "data/intreg_missing.adf"));

    CATCH_ANY(
        brokenMachine = 
        TTAMachine::Machine::loadFromADF(
            "data/fewregs.adf"));

    
    RegisterQuantityCheck rqc;
    MachineCheckResults resOK;
    MachineCheckResults resGuard;
    MachineCheckResults resInt;
    MachineCheckResults resBroken;

    TS_ASSERT(rqc.check(*okMachine, resOK));

    TS_ASSERT(!(rqc.check(*guardBrokenMachine,resGuard)));
    TS_ASSERT_EQUALS(resGuard.errorCount(),1);

    TS_ASSERT(!(rqc.check(*intBrokenMachine,resInt)));
    TS_ASSERT_EQUALS(resInt.errorCount(),1);

    TS_ASSERT(!(rqc.check(*brokenMachine,resBroken)));
    TS_ASSERT_EQUALS(resBroken.errorCount(),2);
    
    delete okMachine;
    delete guardBrokenMachine;
    delete intBrokenMachine;
    delete brokenMachine;
}

#endif
