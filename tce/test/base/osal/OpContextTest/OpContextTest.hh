/** 
 * @file OpContextTest.hh
 * 
 * A test suite for OperationContext.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 */

#ifndef OP_CONTEXT_TEST_HH
#define OP_CONTEXT_TEST_HH

#include <string>
using std::string;

#include <TestSuite.h>

#include "OperationContext.hh"
#include "OperationState.hh"
#include "Exception.hh"


class OpContextTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testStateStoring();
    void testNonExistingState();

    void testAdvanceClock();

private:
    OperationContext context;

    class MyDummyState : public OperationState {
    public:
        MyDummyState(string n) : name_(n), advanced_(false) {};
        const char* name() { return name_.c_str(); };
        void advanceClock(OperationContext&) { advanced_ = true; };
        bool isAvailable(const OperationContext&) const { return false; };
        bool advanced() { return advanced_; };
    private:
        string name_;
        bool advanced_;
    };
};


/**
 * Called before each test.
 */
void
OpContextTest::setUp() {
}


/**
 * Called after each test.
 */
void
OpContextTest::tearDown() {
}

/**
 * Tests the state storing capabilities.
 */
void 
OpContextTest::testStateStoring() {
    MyDummyState state1("myDummy1");
    MyDummyState state2("myDummy2");
    MyDummyState state3("myDummy3");

    context.registerState(&state1);
    context.registerState(&state2);
    context.registerState(&state3);
    context.setMemory(NULL);
    
    TS_ASSERT_EQUALS((context.state("myDummy1")).name(), std::string("myDummy1"));
    TS_ASSERT_EQUALS((context.state("myDummy2")).name(), std::string("myDummy2"));
    TS_ASSERT_EQUALS((context.state("myDummy3")).name(), std::string("myDummy3"));

    context.unregisterState("myDummy3");
    TS_ASSERT_THROWS(context.state("myDummy3"), KeyNotFound);

}

/**
 * Assures that exception is thrown when trying to access non-existing state.
 */
void
OpContextTest::testNonExistingState() {
    TS_ASSERT_THROWS(context.state("some"), KeyNotFound);
}

/**
 * Tests the advanceClock() which should call advanceClock() for each
 * registered OperationState instance.
 */
void
OpContextTest::testAdvanceClock() {
    OperationContext con;
    MyDummyState s1("state1"), s2("state2"), s3("state3"), s4("state4");
    
    con.registerState(&s1);
    con.registerState(&s2);
    con.registerState(&s3);
    con.registerState(&s4);

    TS_ASSERT_EQUALS(s1.advanced(), false);
    TS_ASSERT_EQUALS(s2.advanced(), false);
    TS_ASSERT_EQUALS(s3.advanced(), false);
    TS_ASSERT_EQUALS(s4.advanced(), false);

    con.advanceClock();

    TS_ASSERT_EQUALS(s1.advanced(), true);
    TS_ASSERT_EQUALS(s2.advanced(), true);
    TS_ASSERT_EQUALS(s3.advanced(), true);
    TS_ASSERT_EQUALS(s4.advanced(), true);
}


#endif
