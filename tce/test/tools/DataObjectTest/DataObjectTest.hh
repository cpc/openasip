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
 * @file DataObjectTest.hh 
 *
 * A test suite for DataObject.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi) 
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_DATA_OBJECT_TEST_HH
#define TTA_DATA_OBJECT_TEST_HH

#include <TestSuite.h>
#include <string>
using std::string;

#include "DataObject.hh"
#include "Exception.hh"

class DataObjectTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testConversions();
    void testFreshness();
    void testCopying();
    void testInequality();
private:
};


/**
 * Called before each test.
 */
void
DataObjectTest::setUp() {
}


/**
 * Called after each test.
 */
void
DataObjectTest::tearDown() {
}


/**
 * Test that conversions from different types work correctly.
 */
void
DataObjectTest::testConversions() {

    // integer
    DataObject object1;
    TS_ASSERT_THROWS(object1.integerValue(), NumberFormatException);
    
    object1.setInteger(5);

    string sValue = "";
    TS_ASSERT_THROWS_NOTHING(sValue = object1.stringValue());
    TS_ASSERT_EQUALS(sValue, "5");

    double dValue = 0.0f;
    TS_ASSERT_THROWS_NOTHING(dValue = object1.doubleValue());
    TS_ASSERT_EQUALS(dValue, 5);

    int iValue = 0;
    TS_ASSERT_THROWS_NOTHING(iValue = object1.integerValue());
    TS_ASSERT_EQUALS(iValue, 5);

    float fValue = 0.0f;
    TS_ASSERT_THROWS_NOTHING(fValue = object1.floatValue());
    TS_ASSERT_EQUALS(fValue, 5);

    // string
    DataObject object2;
    object2.setString("jussi");
    TS_ASSERT_THROWS(object2.integerValue(), NumberFormatException);
    TS_ASSERT_THROWS(object2.doubleValue(), NumberFormatException);
    TS_ASSERT_THROWS(object2.floatValue(), NumberFormatException);

    // double
    DataObject object3;
    object3.setDouble(6.9);

    TS_ASSERT_THROWS_NOTHING(sValue = object3.stringValue());
    TS_ASSERT_EQUALS(sValue, "6.9");
    
    TS_ASSERT_EQUALS(object3.integerValue(), 6);

    // float
    DataObject object4;
    object4.setFloat(4.2);
    
    TS_ASSERT_THROWS_NOTHING(dValue = object4.doubleValue());
    TS_ASSERT_EQUALS(dValue, 4.2);
    
    TS_ASSERT_THROWS_NOTHING(sValue = object4.stringValue());
    TS_ASSERT_EQUALS(sValue, "4.2");

    TS_ASSERT_THROWS(object4.integerValue(), NumberFormatException);

    // string -> bool
    bool bValue = false;
    DataObject object5;
    
    object5.setString("TRUE");
    TS_ASSERT_THROWS_NOTHING(bValue = object5.boolValue());
    TS_ASSERT_EQUALS(bValue, true);

    object5.setString("false");
    TS_ASSERT_THROWS_NOTHING(bValue = object5.boolValue());
    TS_ASSERT_EQUALS(bValue, false);

    object5.setString("foo");
    TS_ASSERT_THROWS(object5.boolValue(), NumberFormatException);

}

/**
 * Test that conversions are done correctly when new values are
 * assigned to data object.
 */
void
DataObjectTest::testFreshness() {
    DataObject object;
    object.setInteger(100);
    string sValue = "";
    TS_ASSERT_THROWS_NOTHING(sValue = object.stringValue());
    TS_ASSERT_EQUALS(sValue, "100");
    object.setString("1000");
    int iValue = 0;
    TS_ASSERT_THROWS_NOTHING(iValue = object.integerValue());
    TS_ASSERT_EQUALS(iValue, 1000);
    object.setDouble(43.7);
    TS_ASSERT_THROWS_NOTHING(sValue = object.stringValue());
    TS_ASSERT_EQUALS(sValue, "43.7");
}

/**
 * Test that copying and assignment works.
 */
void
DataObjectTest::testCopying() {
    DataObject object1;
    object1.setFloat(101.1);
    DataObject object2(object1);
    DataObject object3;
    object3 = object2;
    TS_ASSERT_EQUALS(object1.floatValue(), object2.floatValue());
    TS_ASSERT_EQUALS(object2.floatValue(), object3.floatValue());
}

/**
 * Test that the inequality comparisons work as they should.
 */
void
DataObjectTest::testInequality() {
    DataObject object1, object2, object3;
    object1.setInteger(6);
    object2.setInteger(6);
    object3.setString("7");
    
    TS_ASSERT(!(object1 != object2));
    TS_ASSERT(object1 != object3);
}

#endif
