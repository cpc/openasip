/** 
 * @file UnboundedRegisterFileTest.hh 
 *
 * A test suite for UnboundedRegisterFile.
 * 
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
