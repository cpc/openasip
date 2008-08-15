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
