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
 * @file UnboundedRegisterFileTest.hh 
 *
 * A test suite for UnboundedRegisterFile.
 * 
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_UNBOUNDED_REGISTER_FILE_TEST_HH
#define TTA_UNBOUNDED_REGISTER_FILE_TEST_HH

#include <string>
#include <TestSuite.h>

#include "UnboundedRegisterFile.hh"
#include "UniversalMachine.hh"
#include "OperationPool.hh"

using std::string;
using namespace TTAMachine;

/**
 * Class that tests UnboundedRegisterFile class.
 */
class UnboundedRegisterFileTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testGetters();
    
private:
    RegisterFile* uRF_;
};


/**
 * Called before each test.
 */
void
UnboundedRegisterFileTest::setUp() {
    uRF_ = UniversalMachine::instance().registerFileNavigator().item(
        UM_INTEGER_URF_NAME);
}


/**
 * Called after each test.
 */
void
UnboundedRegisterFileTest::tearDown() {
}


/**
 * Tests the getter methods.
 */
void
UnboundedRegisterFileTest::testGetters() {

    TS_ASSERT(uRF_->maxReads() == 1);
    TS_ASSERT(uRF_->maxWrites() == 1);
    OperationPool::cleanupCache();
}

#endif
