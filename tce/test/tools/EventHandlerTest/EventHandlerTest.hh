/** 
 * @file EventHandler.hh 
 *
 * A test suite for Event Handler tool.
 * 
 * @author Atte Oksman 2005 (oksman@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_EVENT_HANDLER_TEST_HH
#define TTA_EVENT_HANDLER_TEST_HH

#include <TestSuite.h>

#include "FileSystem.hh"
#include "Environment.hh"
#include "Listener.hh"
#include "Informer.hh"


/**
 * A test class for Event Handler tool. The tool uses Informer and Listener
 * classes.
 */
class EventHandlerTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testRegisterListener();
    
private:
    /// The listener that is used in tests.
    Listener* listener_;
    /// The informer that is used in tests.
    Informer* informer_;
    /// A event that may be used in tests.
    int defaultEvent_;

};


/**
 * Called before each test.
 */
void
EventHandlerTest::setUp() {
    listener_ = new Listener();
    informer_ = new Informer();
    defaultEvent_ = 42;
}


/**
 * Called after each test.
 */
void
EventHandlerTest::tearDown() {
    delete listener_;
    listener_ = NULL;
    delete informer_;
    informer_ = NULL;
}


/**
 * Tests registering a listener to a certain event.
 */
void
EventHandlerTest::testRegisterListener() {

    bool checkRetval = false;

    // simple add and remove
    checkRetval = informer_->registerListener(defaultEvent_, listener_);
    TS_ASSERT(checkRetval);
    checkRetval = informer_->unregisterListener(defaultEvent_, listener_);
    TS_ASSERT(checkRetval);

    // same listener added several times
    checkRetval = informer_->registerListener(defaultEvent_, listener_);
    TS_ASSERT(checkRetval);
    checkRetval = informer_->registerListener(defaultEvent_, listener_);
    TS_ASSERT(checkRetval);
    checkRetval = informer_->registerListener(defaultEvent_, listener_);
    TS_ASSERT(checkRetval);
    checkRetval = informer_->unregisterListener(defaultEvent_, listener_);
    TS_ASSERT(checkRetval);
    checkRetval = informer_->unregisterListener(defaultEvent_, listener_);
    TS_ASSERT(checkRetval);
    checkRetval = informer_->unregisterListener(defaultEvent_, listener_);
    TS_ASSERT(checkRetval);
}

#endif
