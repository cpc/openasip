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
 * @file ImmediateUnitTest.hh 
 *
 * A test suite for ImmediateUnit class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef ImmediateUnitTest_HH
#define ImmediateUnitTest_HH

#include <string>

#include <TestSuite.h>
#include "ImmediateUnit.hh"

using std::string;
using namespace TTAMachine;

const string iu1Name = "iu1";

/**
 * Tests the functionality of ImmediateUnit class.
 */
class ImmediateUnitTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testSetName();
    void testDeletion();

private:
    Machine* mach_;
    ImmediateUnit* iu1_;
};


/**
 * Called before each test.
 */
void
ImmediateUnitTest::setUp() {
    mach_ = new Machine();
    iu1_ = new ImmediateUnit(iu1Name, 20, 32, 1, 0, Machine::SIGN);
    mach_->addImmediateUnit(*iu1_);
}


/**
 * Called after each test.
 */
void
ImmediateUnitTest::tearDown() {
    delete mach_;
}


/**
 * Tests setting the name of the immediate unit.
 */
void
ImmediateUnitTest::testSetName() {
    TS_ASSERT_THROWS_NOTHING(iu1_->setName("foo"));
    TS_ASSERT(iu1_->name() == "foo");
    ImmediateUnit* iu2 = new ImmediateUnit(
        iu1Name, 20, 32, 1, 0, Machine::SIGN);
    mach_->addImmediateUnit(*iu2);
    TS_ASSERT_THROWS(iu1_->setName(iu1Name), ComponentAlreadyExists);
    TS_ASSERT(iu1_->name() == "foo");    
    TS_ASSERT_THROWS_NOTHING(iu2->setName(iu1Name));
}


/**
 * Tests deletion of an immediate unit.
 */
void
ImmediateUnitTest::testDeletion() {

    const string bus1Name = "bus1";
    const string it1Name = "it1";
    const string emptyTempName = "empty";

    // test that template slots using the immediate unit are deleted
    Bus* bus1 = new Bus(bus1Name, 32, 32, Machine::SIGN);
    mach_->addBus(*bus1);
    InstructionTemplate* it1 = new InstructionTemplate(it1Name, *mach_);
    it1->addSlot(bus1->name(), 32, *iu1_);
    TS_ASSERT(mach_->instructionTemplateNavigator().count() == 2);
    TS_ASSERT(it1->destinationUsesSlot(bus1->name(), *iu1_));
    delete iu1_;
    TS_ASSERT(mach_->instructionTemplateNavigator().hasItem(it1Name));
    TS_ASSERT(!it1->usesSlot(bus1->name()));
    TS_ASSERT(mach_->immediateUnitNavigator().count() == 0);
}


#endif
