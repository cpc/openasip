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
 * @file InstructionTemplateTest.hh 
 * A test suite for InstructionTemplate.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef InstructionTemplateTest_HH
#define InstructionTemplateTest_HH

#include <string>

#include <TestSuite.h>
#include "Machine.hh"
#include "InstructionTemplate.hh"
#include "ADFSerializer.hh"
#include "FileSystem.hh"

using std::string;
using namespace TTAMachine;

const string it1Name = "it1";
const string it2Name = "it2";
const string bus1Name = "bus1";
const string bus2Name = "bus2";
const string bus3Name = "bus3";
const string iu1Name = "iu1";
const string iu2Name = "iu2";
const string iu3Name = "iu3";

/**
 * A test suite for InstructionTemplate class.
 */
class InstructionTemplateTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();
    void testSetName();
    void testModifyingSlots();
    void testLoadState();
    void testObjectStateLoadingErrors();

private:
    Machine* mach_;
};


/**
 * Called before each test.
 */
void
InstructionTemplateTest::setUp() {
    mach_ = new Machine();
    new InstructionTemplate(it1Name, *mach_);
}


/**
 * Called after each test.
 */
void
InstructionTemplateTest::tearDown() {
    delete mach_;
}


/**
 * Tests creation of instruction templates.
 */
void
InstructionTemplateTest::testCreation() {
    TS_ASSERT_THROWS(
        new InstructionTemplate(it1Name, *mach_), ComponentAlreadyExists);
    TS_ASSERT_THROWS_NOTHING(new InstructionTemplate(it2Name, *mach_));

    TS_ASSERT_THROWS(
        new InstructionTemplate("bad name", *mach_), InvalidName);
}


/**
 * Tests the setName method.
 */
void
InstructionTemplateTest::testSetName() {
    InstructionTemplate* it2 = new InstructionTemplate(it2Name, *mach_);
    TS_ASSERT_THROWS_NOTHING(it2->setName("foo"));
    TS_ASSERT(it2->name() == "foo");
    TS_ASSERT_THROWS(it2->setName(it1Name), ComponentAlreadyExists);
    TS_ASSERT_THROWS(it2->setName("bad name"), InvalidName);
}


/**
 * Tests modifying template slots.
 */
void
InstructionTemplateTest::testModifyingSlots() {

    Bus* bus1 = new Bus(bus1Name, 32, 32, Machine::SIGN);
    Bus* bus2 = new Bus(bus2Name, 32, 32, Machine::SIGN);
    Bus* bus3 = new Bus(bus3Name, 32, 32, Machine::SIGN);

    mach_->addBus(*bus1);
    mach_->addBus(*bus3);
    ImmediateUnit* iu1 = new ImmediateUnit(
        iu1Name, 32, 32, 1, 0, Machine::SIGN);
    ImmediateUnit* iu2 = new ImmediateUnit(
        iu2Name, 32, 32, 1, 0, Machine::SIGN);
    mach_->addImmediateUnit(*iu1);

    InstructionTemplate* it2 = new InstructionTemplate(it2Name, *mach_);
    TS_ASSERT_THROWS(it2->addSlot(bus1->name(), 0, *iu1), OutOfRange);
    TS_ASSERT_THROWS(
        it2->addSlot(bus1->name(), 32, *iu2), IllegalRegistration);
    TS_ASSERT_THROWS(it2->addSlot(bus2->name(), 32, *iu1), InstanceNotFound);

    mach_->addBus(*bus2);
    mach_->addImmediateUnit(*iu2);

    TS_ASSERT_THROWS_NOTHING(it2->addSlot(bus1->name(), 32, *iu1));
    TS_ASSERT_THROWS(
        it2->addSlot(bus1->name(), 32, *iu2), ComponentAlreadyExists);
    TS_ASSERT_THROWS_NOTHING(it2->addSlot(bus2->name(), 2, *iu1));
    TS_ASSERT_THROWS_NOTHING(it2->addSlot(bus3->name(), 22, *iu2));

    TS_ASSERT(it2->usesSlot(bus1->name()));
    TS_ASSERT(it2->usesSlot(bus2->name()));
    TS_ASSERT(it2->destinationUsesSlot(bus1->name(), *iu1));
    TS_ASSERT(!it2->destinationUsesSlot(bus1->name(), *iu2));
    TS_ASSERT(it2->destinationUsesSlot(bus2->name(), *iu1));
    TS_ASSERT(!it2->destinationUsesSlot(bus2->name(), *iu2));
    TS_ASSERT(it2->destinationUsesSlot(bus3->name(), *iu2));

    TS_ASSERT(it2->numberOfDestinations() == 2);
    TS_ASSERT(it2->supportedWidth() == 34);
    TS_ASSERT(it2->supportedWidth(*iu1) == 34);
    TS_ASSERT(it2->supportedWidth(*iu2) == 22);
    TS_ASSERT(it2->supportedWidth(bus1->name()) == 32);
    TS_ASSERT(it2->supportedWidth(bus2->name()) == 2);
    TS_ASSERT(it2->supportedWidth(bus3->name()) == 22);

    TS_ASSERT(it2->isOneOfDestinations(*iu1));
    TS_ASSERT(it2->isOneOfDestinations(*iu2));

    ImmediateUnit* iu3 = new ImmediateUnit(
        iu3Name, 32, 32, 1, 0, Machine::SIGN);
    mach_->addImmediateUnit(*iu3);

    TS_ASSERT(!it2->isOneOfDestinations(*iu3));

    it2->removeSlot(bus3->name()); 

    TS_ASSERT(!it2->usesSlot(bus3->name()));
    TS_ASSERT(it2->usesSlot(bus2->name()));

    it2->removeSlots(*iu1);

    TS_ASSERT(!it2->usesSlot(bus1->name()));
    TS_ASSERT(!it2->usesSlot(bus2->name()));
}


/**
 * Tests loading the state from an ObjectState tree.
 */
void
InstructionTemplateTest::testLoadState() {

    InstructionTemplate* it1 = 
        mach_->instructionTemplateNavigator().item(it1Name);
    ObjectState* it1State = it1->saveState();
    TS_ASSERT_THROWS(
        new InstructionTemplate(it1State, *mach_),
        ObjectStateLoadingException);
    it1State->setAttribute(Component::OSKEY_NAME, it2Name);
    TS_ASSERT_THROWS_NOTHING(new InstructionTemplate(it1State, *mach_));
    
    delete it1State;
    delete mach_->instructionTemplateNavigator().item(it2Name);

    Bus* bus1 = new Bus(bus1Name, 32, 32, Machine::SIGN);
    mach_->addBus(*bus1);
    ImmediateUnit* iu1 = new ImmediateUnit(
        iu1Name, 50, 32, 1, 0, Machine::SIGN);
    mach_->addImmediateUnit(*iu1);

    InstructionTemplate* it2 = new InstructionTemplate(it2Name, *mach_);
    it2->addSlot(bus1->name(), 16, *iu1);
    ObjectState* it2State = it2->saveState();
    delete it2;
    it2 = new InstructionTemplate(it2State, *mach_);
    TS_ASSERT(it2->usesSlot(bus1->name()));
    TS_ASSERT(it2->destinationUsesSlot(bus1->name(), *iu1));
    TS_ASSERT(it2->supportedWidth() == 16);

    delete it2;
    mach_->removeBus(*bus1);
   
    TS_ASSERT_THROWS(
        new InstructionTemplate(it2State, *mach_),
        ObjectStateLoadingException);
    mach_->addBus(*bus1);
    TS_ASSERT_THROWS_NOTHING(new InstructionTemplate(it2State, *mach_));
    delete iu1;
    TS_ASSERT_THROWS(
        new InstructionTemplate(it2State, *mach_), 
        ObjectStateLoadingException);
    delete it2State;
}


/**
 * Tests loading instruction template from an erroneous mdf file.
 */
void
InstructionTemplateTest::testObjectStateLoadingErrors() {

    const string DIR_SEP = FileSystem::DIRECTORY_SEPARATOR;
    const string DUPLICATE_SLOT = "." + DIR_SEP + "data" + DIR_SEP + 
        "DuplicateSlot.mdf";
    const string TWO_DESTINATIONS = "." + DIR_SEP + "data" + DIR_SEP +
        "TwoDestinations.mdf";
    const string EMPTY_TEMPLATE_NOT_DECLARED_IN_ALL_IUS = "." + DIR_SEP + 
        "data" + DIR_SEP + "EmptyTemplateNotDeclaredInAllIUs.mdf";
    const string SAME_TEMPLATE_EMPTY_AND_NOT = "." + DIR_SEP + "data" + 
        DIR_SEP + "SameTemplateEmptyAndNot.mdf";
    const string VALID = "." + DIR_SEP + "data" + DIR_SEP + "Valid.mdf";

    ADFSerializer serializer;
    Machine mach;

    serializer.setSourceFile(DUPLICATE_SLOT);
    TS_ASSERT_THROWS(serializer.readState(), SerializerException);

    serializer.setSourceFile(TWO_DESTINATIONS);
    TS_ASSERT_THROWS(serializer.readState(), SerializerException);

    serializer.setSourceFile(EMPTY_TEMPLATE_NOT_DECLARED_IN_ALL_IUS);
    TS_ASSERT_THROWS(serializer.readState(), SerializerException);

    serializer.setSourceFile(SAME_TEMPLATE_EMPTY_AND_NOT);
    TS_ASSERT_THROWS(serializer.readState(), SerializerException);

    serializer.setSourceFile(VALID);
    ObjectState* machState = serializer.readState();
    TS_ASSERT_THROWS_NOTHING(mach.loadState(machState));
    delete machState;
}


#endif
