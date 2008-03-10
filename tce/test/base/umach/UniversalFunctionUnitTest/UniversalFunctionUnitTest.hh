/** 
 * @file UniversalFunctionUnitTest.hh 
 *
 * A test suite for UniversalFunctionUnit.
 * 
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_UNIVERSAL_FUNCTION_UNIT_TEST_HH
#define TTA_UNIVERSAL_FUNCTION_UNIT_TEST_HH

#include <string>
#include <TestSuite.h>

#include "UniversalFunctionUnit.hh"
#include "UniversalMachine.hh"
#include "FUPort.hh"
#include "OperationPool.hh"

using std::string;
using namespace TTAMachine;

/**
 * Class that tests UniversalFunctionUnit class.
 */
class UniversalFunctionUnitTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testOperation();
    
private:
    OperationPool* opPool_;
    UniversalMachine* machine_;
    FunctionUnit* uFU_;
};


/**
 * Called before each test.
 */
void
UniversalFunctionUnitTest::setUp() {
    opPool_ = new OperationPool();
    machine_ = new UniversalMachine(*opPool_);
    uFU_ = machine_->functionUnitNavigator().item(0);
}


/**
 * Called after each test.
 */
void
UniversalFunctionUnitTest::tearDown() {
    delete machine_;
    delete opPool_;
}


/**
 * Tests the operation() methods.
 */
void
UniversalFunctionUnitTest::testOperation() {

    TS_ASSERT(uFU_->operationCount() == 0);
    TS_ASSERT(uFU_->portCount() == 0);
    TS_ASSERT_THROWS(uFU_->operation(0), OutOfRange);
    
    TS_ASSERT_THROWS(uFU_->operation("foobar"), InstanceNotFound);
    TS_ASSERT(uFU_->operationCount() == 0);

    const string ADD = "ADD";
    const string ABSF = "ABSF";

    HWOperation* add = uFU_->operation(ADD);
    TS_ASSERT(uFU_->operationCount() == 1);
    TS_ASSERT_EQUALS(uFU_->portCount(), 3);
    HWOperation* secondAdd = uFU_->operation(ADD);
    TS_ASSERT(add == secondAdd);
    
    uFU_->operation(ABSF);
    TS_ASSERT(uFU_->operationCount() == 2);
    TS_ASSERT_EQUALS(uFU_->portCount(), 5);

    BaseFUPort* oc32 = uFU_->port(UniversalFunctionUnit::OC_SETTING_PORT_32);
    BaseFUPort* oc64 = uFU_->port(UniversalFunctionUnit::OC_SETTING_PORT_64);

    TS_ASSERT(oc32->isOpcodeSetting());
    TS_ASSERT(oc64->isOpcodeSetting());
    TS_ASSERT(oc32->width() == 32);
    TS_ASSERT(oc64->width() == 64);
    OperationPool::cleanupCache();
}

#endif
