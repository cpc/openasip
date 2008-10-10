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
    OperationPool* opPool_;
    UniversalMachine* machine_;
    RegisterFile* uRF_;
};


/**
 * Called before each test.
 */
void
UnboundedRegisterFileTest::setUp() {
    opPool_ = new OperationPool();
    machine_ = new UniversalMachine(*opPool_);
    uRF_ = machine_->registerFileNavigator().item(
        UM_INTEGER_URF_NAME);
}


/**
 * Called after each test.
 */
void
UnboundedRegisterFileTest::tearDown() {
    delete machine_;
    delete opPool_;
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
