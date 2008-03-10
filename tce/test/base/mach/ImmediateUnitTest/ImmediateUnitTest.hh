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
