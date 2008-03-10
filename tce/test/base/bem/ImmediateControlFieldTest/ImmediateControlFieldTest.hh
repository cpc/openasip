/**
 * @file ImmediateControlFieldTest.hh
 *
 * A test suite for ImmediateControlField.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef ImmediateControlFieldTest_HH
#define ImmediateControlFieldTest_HH

#include <string>

#include <TestSuite.h>
#include "BinaryEncoding.hh"
#include "ImmediateControlField.hh"

using std::string;

/**
 * Test suite for testing ImmediateControlField class.
 */
class ImmediateControlFieldTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();
    void testTemplateEncodings();

private:
    BinaryEncoding* bem_;
};


/**
 * Called before each test.
 */
void
ImmediateControlFieldTest::setUp() {
    bem_ = new BinaryEncoding();
}


/**
 * Called after each test.
 */
void
ImmediateControlFieldTest::tearDown() {
    delete bem_;
}


/**
 * Tests the constructor of ImmediateControlField class.
 */
void
ImmediateControlFieldTest::testCreation() {
    ImmediateControlField* field = new ImmediateControlField(*bem_);
    TS_ASSERT(field->parent() == bem_);
    TS_ASSERT(bem_->hasImmediateControlField());
    TS_ASSERT_THROWS(new ImmediateControlField(*bem_), ObjectAlreadyExists);   
    TS_ASSERT(field->width() == 0);
    TS_ASSERT(field->childFieldCount() == 0);
}


/**
 * Tests assigning encoding for instruction templates.
 */
void
ImmediateControlFieldTest::testTemplateEncodings() {
    
    const string temp1 = "t1";
    const string temp2 = "t2";
    const string temp3 = "t3";

    ImmediateControlField* field = new ImmediateControlField(*bem_);
    TS_ASSERT(field->templateCount() == 0);

    field->addTemplateEncoding(temp1, 0);
    field->addTemplateEncoding(temp2, 1);
    
    TS_ASSERT(field->childFieldCount() == 0);
    TS_ASSERT(field->width() == 1);
    TS_ASSERT(field->hasTemplateEncoding(temp1));
    TS_ASSERT(field->hasTemplateEncoding(temp2));
    TS_ASSERT(!field->hasTemplateEncoding("foobar"));
    
    TS_ASSERT_THROWS(
	field->addTemplateEncoding(temp1, 1), ObjectAlreadyExists);

    field->addTemplateEncoding(temp3, 2);
    TS_ASSERT(field->width() == 2);
    TS_ASSERT(field->hasTemplateEncoding(temp3));
    field->removeTemplateEncoding(temp3);
    TS_ASSERT(!field->hasTemplateEncoding(temp3));

    TS_ASSERT(field->templateEncoding(temp1) == 0);
    TS_ASSERT(field->templateEncoding(temp2) == 1);

    field->addTemplateEncoding(temp1, 5);
    TS_ASSERT(field->templateEncoding(temp1) == 5);

    TS_ASSERT_THROWS(field->instructionTemplate(-1), OutOfRange);
    TS_ASSERT_THROWS(field->instructionTemplate(2), OutOfRange);
    TS_ASSERT_THROWS_NOTHING(field->instructionTemplate(0));

    field->setExtraBits(2);
    TS_ASSERT(field->width() == 5);
}
   
#endif
