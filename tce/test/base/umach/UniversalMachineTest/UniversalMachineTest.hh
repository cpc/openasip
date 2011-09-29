/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file UniversalMachineTest.hh 
 *
 * A test suite for UniversalMachine.
 * 
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
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
    void testAllComponents();
};

/**
 * Checks that there are all the components in the universal machine.
 */
void
UniversalMachineTest::testAllComponents() {
    
    // test the existence of the bus
    Machine::BusNavigator busNav = UniversalMachine::instance().busNavigator();
    TS_ASSERT(busNav.count() == 1);
    TS_ASSERT(busNav.hasItem(UM_BUS_NAME));
    Bus* bus = busNav.item(UM_BUS_NAME);
    TS_ASSERT(bus->segmentCount() == 1);
    TS_ASSERT(bus->hasSegment(UM_SEGMENT_NAME));
    TS_ASSERT(bus->guardCount() == 2);

    // test the existence of sockets
    Machine::SocketNavigator socketNav = 
        UniversalMachine::instance().socketNavigator();
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
    Machine::FunctionUnitNavigator fuNav = 
        UniversalMachine::instance().functionUnitNavigator();
    TS_ASSERT(fuNav.count() == 1);
    TS_ASSERT(fuNav.hasItem(UM_UNIVERSAL_FU_NAME));
    
    // test the existence of register files
    Machine::RegisterFileNavigator rfNav = 
        UniversalMachine::instance().registerFileNavigator();
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
