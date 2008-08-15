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
 * @file OptionsTest.hh
 *
 * A test suite for Options and OptionValue.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPTIONS_TEST_HH
#define TTA_OPTIONS_TEST_HH

#include <TestSuite.h>
#include <string>
#include <vector>
#include "Options.hh"
#include "OptionValue.hh"
#include "Exception.hh"

using std::string;
using std::vector;

/**
 * Implements the tests needed to verify correct operation of Options and
 * OptionValue.
 */
class OptionsTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testStore();
    void testSetNewValue();
    void testMisuse();

private:

    Options options_;

};

/**
 * Called before each test.
 */
void
OptionsTest::setUp() {
}


/**
 * Called after each test.
 */
void
OptionsTest::tearDown() {
}

/**
 * Tests that options are stored and retreaved correctly.
 */
void
OptionsTest::testStore() {

    string opt = "intOption";
    int newInt = 11111;
    int newInt2 = 2222;

    IntegerOptionValue* intVal = new IntegerOptionValue(newInt);
    IntegerOptionValue* intVal2 = new IntegerOptionValue(newInt2);

    options_.addOptionValue(opt, intVal);
    options_.addOptionValue(opt, intVal2);


    string opt2 = "fruit";
    string newString = "apple";
    string newString2 = "banana"; 

    StringOptionValue* strVal = new StringOptionValue(newString);
    StringOptionValue* strVal2 = new StringOptionValue(newString2);

    options_.addOptionValue(opt2, strVal);
    options_.addOptionValue(opt2, strVal2);

    string opt3 = "real";
    double newReal = 1.234;
    double newReal2 = 112;

    RealOptionValue* realVal = new RealOptionValue(newReal);
    RealOptionValue* realVal2 = new RealOptionValue(newReal2);

    options_.addOptionValue(opt3, realVal);
    options_.addOptionValue(opt3, realVal2);

    // boolean option
    string opt4 = "bool";
    bool newBool = true;
    bool newBool2 = false;

    BoolOptionValue* boolVal = new BoolOptionValue(newBool);
    BoolOptionValue* boolVal2 = new BoolOptionValue(newBool2);

    options_.addOptionValue(opt4, boolVal);
    options_.addOptionValue(opt4, boolVal2);
	

    // integer list option
    string opt5 = "intList";
    vector<int> newList(2, 2);
    vector<int> newList2(4, 7);

    IntegerListOptionValue* intList = new IntegerListOptionValue(newList);
    IntegerListOptionValue* intList2 = new IntegerListOptionValue(newList2);

    options_.addOptionValue(opt5, intList);
    options_.addOptionValue(opt5, intList2);


    // int option check
    int count = options_.valueCount(opt);
    TS_ASSERT_EQUALS(count, 2);

    // gives the latest added value
    OptionValue& val = options_.optionValue(opt);
    TS_ASSERT_EQUALS(val.integerValue(), newInt2);

    // this gives the oldest value
    OptionValue& val2 = options_.optionValue(opt, 1);
    TS_ASSERT_EQUALS(val2.integerValue(), newInt);

    // string option check
    count = options_.valueCount(opt2);
    TS_ASSERT_EQUALS(count, 2);

    // the oldest string value
    OptionValue& val3 = options_.optionValue(opt2, 1);
    TS_ASSERT_EQUALS(val3.stringValue(), newString);
    
    // the latest string value
    OptionValue& val4 = options_.optionValue(opt2, 0);
    TS_ASSERT_EQUALS(val4.stringValue(), newString2);

    // real option check
    count = options_.valueCount(opt3);
    TS_ASSERT_EQUALS(count, 2);

    // the older real value
    OptionValue& val5 = options_.optionValue(opt3, 1);
    TS_ASSERT_EQUALS(val5.realValue(), newReal);
    
    // latest real value
    OptionValue& val6 = options_.optionValue(opt3);
    TS_ASSERT_EQUALS(val6.realValue(), newReal2);

    // bool option check
    count = options_.valueCount(opt4);
    TS_ASSERT_EQUALS(count, 2);

    // first added value
    OptionValue& val7 = options_.optionValue(opt4, 1);
    TS_ASSERT_EQUALS(val7.isFlagOn(), newBool);
    
    // latest added value
    OptionValue& val8 = options_.optionValue(opt4);
    TS_ASSERT_EQUALS(val8.isFlagOff(), !newBool2);

    // integerlist option check
    count = options_.valueCount(opt5);
    TS_ASSERT_EQUALS(count, 2);

    // older values
    OptionValue& val9 = options_.optionValue(opt5, 1);
    TS_ASSERT_EQUALS(val9.listSize(), 2);
    TS_ASSERT_EQUALS(val9.integerValue(1), 2);

    // newer values
    OptionValue& val10 = options_.optionValue(opt5);
    TS_ASSERT_EQUALS(val10.listSize(), 4);
    TS_ASSERT_EQUALS(val10.integerValue(), 7);

}

void
OptionsTest::testSetNewValue() {
    // integer value
    OptionValue& intVal = options_.optionValue("intOption");
    TS_ASSERT_EQUALS(intVal.integerValue(), 2222);

    int newInt = 9;
    intVal.setIntegerValue(newInt);

    OptionValue& intVal2 = options_.optionValue("intOption");
    TS_ASSERT_EQUALS(intVal2.integerValue(), newInt);

    // string value
    OptionValue& strVal = options_.optionValue("fruit");
    TS_ASSERT_EQUALS(strVal.stringValue(), "banana");

    string newStr = "coconut";
    strVal.setStringValue(newStr);

    OptionValue& strVal2 = options_.optionValue("fruit");
    TS_ASSERT_EQUALS(strVal2.stringValue(), newStr);

    // real value
    OptionValue& realVal = options_.optionValue("real");
    TS_ASSERT_EQUALS(realVal.realValue(), 112);

    double newReal = 666.667;
    realVal.setRealValue(newReal);

    OptionValue& realVal2 = options_.optionValue("real");
    TS_ASSERT_EQUALS(realVal2.realValue(), newReal);

    // boolean value
    OptionValue& boolValue = options_.optionValue("bool");
    TS_ASSERT_EQUALS(boolValue.isFlagOn(), false);
    
    bool newBool = true;
    boolValue.setBoolValue(newBool);

    OptionValue& newBoolValue = options_.optionValue("bool");
    TS_ASSERT_EQUALS(newBoolValue.isFlagOn(), true);

    // integerlist values
    OptionValue& listVal = options_.optionValue("intList", 1);
    TS_ASSERT_EQUALS(listVal.integerValue(1), 2);

    vector<int> newList(3, 3);
    listVal.setIntegerListValue(newList);

    OptionValue& listVal2 = options_.optionValue("intList", 1);
    TS_ASSERT_EQUALS(listVal2.integerValue(2), 3);
    
}

/**
 * Tests the misusing of Options and OptionValue classes.
 */
void
OptionsTest::testMisuse() {
    TS_ASSERT_THROWS(options_.optionValue("bool", 3), OutOfRange);
    TS_ASSERT_THROWS(options_.optionValue("not found", 1), KeyNotFound);

    OptionValue& intVal = options_.optionValue("intOption");
    OptionValue& listVal = options_.optionValue("intList");
    OptionValue& boolVal = options_.optionValue("bool");

    StringOptionValue* strVal = new StringOptionValue("TypeMismatch");
    TS_ASSERT_THROWS(options_.addOptionValue("bool", strVal), TypeMismatch);

    TS_ASSERT_THROWS(intVal.stringValue(), WrongSubclass);
    TS_ASSERT_THROWS(intVal.realValue(), WrongSubclass);
    TS_ASSERT_THROWS(intVal.isFlagOn(), WrongSubclass);
    TS_ASSERT_THROWS(intVal.isFlagOff(), WrongSubclass);
    TS_ASSERT_THROWS(intVal.setBoolValue(true), WrongSubclass);
    TS_ASSERT_THROWS(intVal.setStringValue("error"), WrongSubclass);
    TS_ASSERT_THROWS(intVal.setRealValue(1.2), WrongSubclass);
    vector<int> newList(3, 3);
    TS_ASSERT_THROWS(intVal.setIntegerListValue(newList), WrongSubclass);

    TS_ASSERT_THROWS(boolVal.integerValue(), WrongSubclass);
    TS_ASSERT_THROWS(boolVal.listSize(), WrongSubclass);
    TS_ASSERT_THROWS(boolVal.setIntegerValue(2), WrongSubclass);

    TS_ASSERT_THROWS(listVal.integerValue(6), OutOfRange);
}

#endif
