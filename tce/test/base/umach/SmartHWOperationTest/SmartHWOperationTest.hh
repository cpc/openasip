/** 
 * @file SmartHWOperationTest.hh 
 *
 * A test suite for SmartHWOperationTest.
 * 
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SMART_HW_OPERATION_TEST_HH
#define TTA_SMART_HW_OPERATION_TEST_HH

#include <string>
#include <TestSuite.h>

#include "OperationPool.hh"
#include "UniversalMachine.hh"
#include "HWOperation.hh"
#include "FUPort.hh"

using std::string;
using namespace TTAMachine;

/**
 * Class that tests SmartHWOperation class.
 */
class SmartHWOperationTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testOperandBindings();

private:
    OperationPool* opPool_;
    UniversalMachine* machine_;
    FunctionUnit* uFU_;
};


/**
 * Called before each test.
 */
void
SmartHWOperationTest::setUp() {
    opPool_ = new OperationPool();
    machine_ = new UniversalMachine(*opPool_);
    uFU_ = machine_->functionUnitNavigator().item(0);
}


/**
 * Called after each test.
 */
void
SmartHWOperationTest::tearDown() {
    delete machine_;
    delete opPool_;
}


/**
 * Tests the automatic operand bindings.
 */
void
SmartHWOperationTest::testOperandBindings() {

    const string ADD = "ADD";
    const string ABSF = "ABSF";
    const string OPTEST = "OPTEST";

    HWOperation* add = uFU_->operation(ADD);

    FUPort* port1 = add->port(1);
    FUPort* port2 = add->port(2);
    FUPort* port3 = add->port(3);
    
    TS_ASSERT(add->port(4) == NULL);
    TS_ASSERT(add->port(5) == NULL);

    TS_ASSERT(port1->inputSocket() != NULL);
    TS_ASSERT(port2->inputSocket() != NULL);
    TS_ASSERT(port3->outputSocket() != NULL);
    
    TS_ASSERT(port2->isOpcodeSetting());

    HWOperation* absf = uFU_->operation(ABSF);

    port1 = absf->port(1);
    port2 = absf->port(2);

    TS_ASSERT(port1->inputSocket() != NULL);
    TS_ASSERT(port2->outputSocket() != NULL);
    TS_ASSERT(port1->isOpcodeSetting());

    HWOperation* opTest = uFU_->operation(OPTEST);
    
    port1 = opTest->port(1);
    port2 = opTest->port(2);
    port3 = opTest->port(3);
    
    TS_ASSERT(port1->inputSocket() != NULL);
    TS_ASSERT(port2->inputSocket() != NULL);
    TS_ASSERT(port3->inputSocket() != NULL);
    TS_ASSERT(!port2->isOpcodeSetting());
    TS_ASSERT(port3->isOpcodeSetting());

    OperationPool::cleanupCache();
}

#endif
