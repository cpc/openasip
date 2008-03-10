/** 
 * @file UniversalMachineTest.hh 
 *
 * A test suite for UniversalMachine.
 * 
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_UNIVERSAL_MACHINE_TEST_HH
#define TTA_UNIVERSAL_MACHINE_TEST_HH

#include <TestSuite.h>

#include "UniversalMachine.hh"
#include "Port.hh"
#include "OperationPool.hh"

using namespace TTAMachine;

/**
 * Class that tests UniversalMachine class.
 */
class UniversalMachineTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testAllComponents();
    
private:
    OperationPool* opPool_;
    UniversalMachine* machine_;
};


/**
 * Called before each test.
 */
void
UniversalMachineTest::setUp() {
    opPool_ = new OperationPool();
    machine_ = new UniversalMachine(*opPool_);
}


/**
 * Called after each test.
 */
void
UniversalMachineTest::tearDown() {
    delete machine_;
    delete opPool_;
}


/**
 * Checks that there are all the components in the universal machine.
 */
void
UniversalMachineTest::testAllComponents() {
    
    // test the existence of the bus
    Machine::BusNavigator busNav = machine_->busNavigator();
    TS_ASSERT(busNav.count() == 1);
    TS_ASSERT(busNav.hasItem(UM_BUS_NAME));
    Bus* bus = busNav.item(UM_BUS_NAME);
    TS_ASSERT(bus->segmentCount() == 1);
    TS_ASSERT(bus->hasSegment(UM_SEGMENT_NAME));
    TS_ASSERT(bus->guardCount() == 2);

    // test the existence of sockets
    Machine::SocketNavigator socketNav = machine_->socketNavigator();
    TS_ASSERT(socketNav.count() == 2);
    TS_ASSERT(socketNav.hasItem(UM_INPUT_SOCKET_NAME));
    TS_ASSERT(socketNav.hasItem(UM_OUTPUT_SOCKET_NAME));
    Socket* inputSocket = socketNav.item(
        UM_INPUT_SOCKET_NAME);
    Socket* outputSocket = socketNav.item(
        UM_OUTPUT_SOCKET_NAME);
    TS_ASSERT(inputSocket->isConnectedTo(*bus->segment(0)));
    TS_ASSERT(inputSocket->direction() == Socket::INPUT);
    TS_ASSERT(outputSocket->isConnectedTo(*bus->segment(0)));
    TS_ASSERT(outputSocket->direction() == Socket::OUTPUT);
    
    // test the existence of universal function unit
    Machine::FunctionUnitNavigator fuNav = machine_->functionUnitNavigator();
    TS_ASSERT(fuNav.count() == 1);
    TS_ASSERT(fuNav.hasItem(UM_UNIVERSAL_FU_NAME));
    
    // test the existence of register files
    Machine::RegisterFileNavigator rfNav = machine_->registerFileNavigator();
    TS_ASSERT(rfNav.count() == 4);
    TS_ASSERT(rfNav.hasItem(UM_BOOLEAN_RF_NAME));
    TS_ASSERT(rfNav.hasItem(UM_INTEGER_URF_NAME));
    TS_ASSERT(rfNav.hasItem(UM_DOUBLE_URF_NAME));
    TS_ASSERT(rfNav.hasItem(UM_SPECIAL_RF_NAME));
    
    TS_ASSERT(rfNav.item(0)->portCount() == 2);
    TS_ASSERT(rfNav.item(1)->portCount() == 2);
    TS_ASSERT(rfNav.item(2)->portCount() == 2);
    TS_ASSERT(rfNav.item(3)->portCount() == 2);

    // test the connections of RF ports
    RegisterFile* booleanRF = rfNav.item(UM_BOOLEAN_RF_NAME);
    TS_ASSERT(booleanRF->port(UM_BOOLEAN_RF_WRITE_PORT)->
              inputSocket() == inputSocket);
    TS_ASSERT(booleanRF->port(UM_BOOLEAN_RF_READ_PORT)->
              outputSocket() == outputSocket);

    RegisterFile* integerRF = rfNav.item(UM_INTEGER_URF_NAME);
    TS_ASSERT(integerRF->port(UM_INTEGER_URF_WRITE_PORT)->
              inputSocket() == inputSocket);
    TS_ASSERT(integerRF->port(UM_INTEGER_URF_READ_PORT)->
              outputSocket() == outputSocket);
    
    RegisterFile* doubleRF = rfNav.item(UM_DOUBLE_URF_NAME);
    TS_ASSERT(doubleRF->port(UM_DOUBLE_URF_WRITE_PORT)->
              inputSocket() == inputSocket);
    TS_ASSERT(doubleRF->port(UM_DOUBLE_URF_READ_PORT)->
              outputSocket() == outputSocket);
    
    RegisterFile* specialRF = rfNav.item(UM_SPECIAL_RF_NAME);
    TS_ASSERT(specialRF->port(UM_SPECIAL_RF_WRITE_PORT)->
              inputSocket() == inputSocket);
    TS_ASSERT(specialRF->port(UM_SPECIAL_RF_READ_PORT)->
              outputSocket() == outputSocket);

    OperationPool::cleanupCache();
}


#endif
