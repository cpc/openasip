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
 * @file GuardTest.hh 
 * A test suite for Guard.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef GuardTest_HH
#define GuardTest_HH

#include <string>

#include <TestSuite.h>
#include "Guard.hh"
#include "Machine.hh"
#include "FUPort.hh"
#include "ObjectState.hh"

using std::string;
using namespace TTAMachine;

const string bus1Name = "bus1";
const string rf1Name = "rf1";
const string fu1Name = "fu1";
const string port1Name = "port1";

class GuardTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();
    void testRestrictive();
    void testLoadState();

private:
    Machine* mach_;
};


/**
 * Called before each test.
 */
void
GuardTest::setUp() {
    mach_ = new Machine();
    mach_->addBus(*new Bus(bus1Name, 32, 32, Machine::SIGN));
    mach_->addRegisterFile(
        *new RegisterFile(rf1Name, 32, 32, 1, 1, 0, RegisterFile::NORMAL));
    FunctionUnit* fu1 = new FunctionUnit(fu1Name);
    mach_->addFunctionUnit(*fu1);
    new FUPort(port1Name, 32, *fu1, false, false);
}


/**
 * Called after each test.
 */
void
GuardTest::tearDown() {
    delete mach_;
}


/**
 * Tests creation of guards.
 */
void
GuardTest::testCreation() {

    Bus* bus1 = mach_->busNavigator().item(bus1Name);
    RegisterFile* rf1 = mach_->registerFileNavigator().item(rf1Name);
    RegisterGuard* regGuard = NULL;
    TS_ASSERT_THROWS_NOTHING(
        regGuard = new RegisterGuard(false, *rf1, 0, *bus1));
    TS_ASSERT(regGuard->registerFile() == rf1);
    TS_ASSERT(regGuard->registerIndex() == 0);
    TS_ASSERT(regGuard->parentBus() == bus1);

    TS_ASSERT_THROWS(new RegisterGuard(false, *rf1, 32, *bus1), OutOfRange);
    TS_ASSERT_THROWS(
        new RegisterGuard(false, *rf1, 0, *bus1), ComponentAlreadyExists);

    FUPort* port1 = 
        mach_->functionUnitNavigator().item(fu1Name)->operationPort(0);
    PortGuard* portGuard = NULL;
    TS_ASSERT_THROWS_NOTHING(
        portGuard = new PortGuard(false, *port1, *bus1));
    TS_ASSERT(portGuard->port() == port1);
    TS_ASSERT_THROWS(
        new PortGuard(false, *port1, *bus1), ComponentAlreadyExists);

    TS_ASSERT_THROWS_NOTHING(new UnconditionalGuard(false, *bus1));
    TS_ASSERT_THROWS_NOTHING(new UnconditionalGuard(true, *bus1));
    TS_ASSERT_THROWS(
        new UnconditionalGuard(false, *bus1), ComponentAlreadyExists);
}


/**
 * Tests the methods of querying if the guard is more or less restrictive
 * than the other.
 */
void
GuardTest::testRestrictive() {

    Bus* bus1 = mach_->busNavigator().item(bus1Name);
    RegisterFile* rf1 = mach_->registerFileNavigator().item(rf1Name);
    RegisterGuard* regGuard1 = new RegisterGuard(false, *rf1, 0, *bus1);
    RegisterGuard* regGuard2 = new RegisterGuard(false, *rf1, 1, *bus1);

    TS_ASSERT(!regGuard1->isLessRestrictive(*regGuard2));
    TS_ASSERT(!regGuard1->isMoreRestrictive(*regGuard2));
    TS_ASSERT(regGuard1->isDisjoint(*regGuard2));
}


/**
 * Tests the functionality of loadState methods.
 */
void
GuardTest::testLoadState() {

    Bus* bus1 = mach_->busNavigator().item(bus1Name);
    RegisterFile* rf1 = mach_->registerFileNavigator().item(rf1Name);
    RegisterGuard* regGuard1 = new RegisterGuard(false, *rf1, 3, *bus1);

    ObjectState* machState = mach_->saveState();
    Machine* newMach = new Machine();
    TS_ASSERT_THROWS_NOTHING(newMach->loadState(machState));

    ObjectState* regGuardState = regGuard1->saveState();
    ObjectState* busState = machState->childByName(Bus::OSNAME_BUS);
    busState->addChild(regGuardState);
    TS_ASSERT_THROWS(
        newMach->loadState(machState), ObjectStateLoadingException);
    delete regGuardState;
    ObjectState* rfState = machState->childByName(
        RegisterFile::OSNAME_REGISTER_FILE);
    rfState->setAttribute(BaseRegisterFile::OSKEY_SIZE, 2);
    TS_ASSERT_THROWS(
        newMach->loadState(machState), ObjectStateLoadingException);
    delete rfState;
    TS_ASSERT_THROWS(
        newMach->loadState(machState), ObjectStateLoadingException);

    delete machState;
    
    FunctionUnit* fu1 = mach_->functionUnitNavigator().item(fu1Name);
    PortGuard* portGuard1 = 
        new PortGuard(false, *fu1->operationPort(0), *bus1);
    machState = mach_->saveState();
    TS_ASSERT_THROWS_NOTHING(newMach->loadState(machState));
    
    ObjectState* portGuardState = portGuard1->saveState();
    busState = machState->childByName(Bus::OSNAME_BUS);
    busState->addChild(portGuardState);
    TS_ASSERT_THROWS(
        newMach->loadState(machState), ObjectStateLoadingException);

    delete portGuardState;
    delete machState->childByName(FunctionUnit::OSNAME_FU)->
        childByName(FUPort::OSNAME_FUPORT);
    TS_ASSERT_THROWS(
        newMach->loadState(machState), ObjectStateLoadingException);
    delete machState->childByName(FunctionUnit::OSNAME_FU);
    TS_ASSERT_THROWS(
        newMach->loadState(machState), ObjectStateLoadingException);
    portGuardState = busState->childByName(PortGuard::OSNAME_PORT_GUARD);
    delete portGuardState;
    TS_ASSERT_THROWS_NOTHING(newMach->loadState(machState));
    delete machState;

    UnconditionalGuard* ucGuard1 = new UnconditionalGuard(false, *bus1);
    machState = mach_->saveState();
    TS_ASSERT_THROWS_NOTHING(newMach->loadState(machState));
    ObjectState* ucGuardState = ucGuard1->saveState();
    machState->childByName(Bus::OSNAME_BUS)->addChild(ucGuardState);
    TS_ASSERT_THROWS(
        newMach->loadState(machState), ObjectStateLoadingException);
    
    delete machState;
    delete newMach;
}    

#endif
