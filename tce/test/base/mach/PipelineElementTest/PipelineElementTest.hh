/** 
 * @file PipelineElementTest.hh 
 * A test suite for PipelineElement.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef PipelineElementTest_HH
#define PipelineElementTest_HH

#include <TestSuite.h>
#include "FunctionUnit.hh"
#include "PipelineElement.hh"

using std::string;
using namespace TTAMachine;

class PipelineElementTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testSetName();

private:
};


/**
 * Called before each test.
 */
void
PipelineElementTest::setUp() {
}


/**
 * Called after each test.
 */
void
PipelineElementTest::tearDown() {
}


/**
 * Tests setting the name of the pipeline element.
 */
void
PipelineElementTest::testSetName() {

    FunctionUnit* fu = new FunctionUnit("fu");
    PipelineElement* res1 = new PipelineElement("res1", *fu);
    PipelineElement* res2 = new PipelineElement("res2", *fu);

    TS_ASSERT_THROWS_NOTHING(res1->setName("foo"));
    TS_ASSERT(res1->name() == "foo");
    TS_ASSERT_THROWS(res2->setName("foo"), ComponentAlreadyExists);
    TS_ASSERT(res2->name() == "res2");

    delete res1;
    delete res2;
    delete fu;
}
    
#endif
