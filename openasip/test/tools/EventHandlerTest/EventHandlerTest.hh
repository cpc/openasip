/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file EventHandler.hh 
 *
 * A test suite for Event Handler tool.
 * 
 * @author Atte Oksman 2005 (oksman-no.spam-cs.tut.fi)
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
