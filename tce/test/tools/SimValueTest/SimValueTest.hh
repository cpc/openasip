/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file SimValueTest.hh
 * 
 * A test suite for SimValue.
 *
 * @author Pekka J‰‰skel‰inen 2004,2010 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Mikko Jarvela 2014 (mikko.jarvela-no.spam-tut.fi)
 */

#ifndef SIM_VALUE_TEST_HH
#define SIM_VALUE_TEST_HH

#include <TestSuite.h>
#include "SimValue.hh"
#include "TCEString.hh"

class SimValueTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testConstructors();
    void testAssignments();
    void testAdditions();
    void testDifferences();
    void testDivisions();
    void testProducts();
    void testEqualities();

    void testMisc();
    
    
private:
};


/**
 * Called before each test.
 */
void
SimValueTest::setUp() {
}


/**
 * Called after each test.
 */
void
SimValueTest::tearDown() {
}

/**
 * Tests constructors.
 */
void
SimValueTest::testConstructors() {
    // SimValue()
    SimValue simValue1;
    TS_ASSERT_EQUALS(simValue1.width(), SIMULATOR_MAX_INTWORD_BITWIDTH);
    
    // explicit SimValue(int width)
    SimValue simValue2(80);
    TS_ASSERT_EQUALS(simValue2.width(), 80);

    // explicit SimValue(int value, int width)
    SimValue simValue3(0xabcdef00, 16);
    TS_ASSERT_EQUALS(simValue3.rawData_[SIMVALUE_MAX_BYTE_SIZE-2], 0xef);
    TS_ASSERT_EQUALS(simValue3.rawData_[SIMVALUE_MAX_BYTE_SIZE-1], 0x00);
    TS_ASSERT_EQUALS(simValue3.intValue(), (int)0xffffef00);
    TS_ASSERT_EQUALS(simValue3.unsignedValue(), (unsigned)0x0000ef00);
    
    // SimValue(const SimValue& source)
    SimValue simValue4(simValue3);
    TS_ASSERT_EQUALS(simValue4.rawData_[SIMVALUE_MAX_BYTE_SIZE-2], 0xef);
    TS_ASSERT_EQUALS(simValue4.rawData_[SIMVALUE_MAX_BYTE_SIZE-1], 0x00);
    TS_ASSERT_EQUALS(simValue4.intValue(), int(0xffffef00));
    TS_ASSERT_EQUALS(simValue4.unsignedValue(), unsigned(0x0000ef00));
    
    // Test the mask variable that is set in constructors.
    SimValue simValue5(0x0f0f0f0f, 4);
    TS_ASSERT_EQUALS(simValue5.intValue(), -1);
    SimValue simValue6(0xf0f0f0f1, 4);
    TS_ASSERT_EQUALS(simValue6.intValue(), 1);
}


/**
 * Tests assignment operators.
 */
void
SimValueTest::testAssignments() {
    
    SimValue val(10);
    SIntWord test1 = 12;

    TS_ASSERT_EQUALS(val.width(), 10);

    val = test1;

    TS_ASSERT_EQUALS(val.sIntWordValue(), test1);

    FloatWord test5 = 4.4;

    val = test5;

    TS_ASSERT_EQUALS(val.floatWordValue(), test5);

    DoubleWord test6 = 54.332;


    val = test6;
    TS_ASSERT_EQUALS(val.doubleWordValue(), test6);

    SimValue val2(10);

    val2 = val;
    TS_ASSERT_EQUALS(val2.doubleWordValue(), test6);

    // Test unsigned bit masking.
    SimValue val3(3);
    val3 = 8; // (0..01)000
    unsigned int test = 0;
    TS_ASSERT_EQUALS(val3.unsignedValue(), test);
    val3 = 7; // (0..0)111
    test = 7;
    TS_ASSERT_EQUALS(val3.unsignedValue(), test);

    // Test signed value extension.
    val3 = -4; // (1..1)100
    TS_ASSERT_EQUALS(val3.intValue(), -4);
    val3 = -5; // (1..1)011 
    TS_ASSERT_EQUALS(val3.intValue(), 3);
    val3 = 3; // (0..0)011
    TS_ASSERT_EQUALS(val3.intValue(), 3);
    val3 = 4; //(0..0)100
    TS_ASSERT_EQUALS(val3.intValue(), -4);

    SimValue val4(64);
    val4 = 0x80000001;
    TS_ASSERT_EQUALS(val4.intValue(), -2147483647);
    TS_ASSERT_EQUALS(val4.unsignedValue(), 0x80000001);


    // ###############################################################
    // New tests.
    
    // SimValue& operator=(const SIntWord& source)
    SimValue simValue(32);
    simValue = SIntWord(0x12345678);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-4], 0x12);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-3], 0x34);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-2], 0x56);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-1], 0x78);

    // SimValue& operator=(const UIntWord& source)
    simValue = UIntWord(0xffeeddcc);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-4], 0xff);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-3], 0xee);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-2], 0xdd);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-1], 0xcc);

    // SimValue& operator=(const HalfFloatWord& source)
    simValue = HalfFloatWord(uint16_t(0x8000));
    TS_ASSERT_EQUALS(
        simValue.halfFloatWordValue().getBinaryRep(), uint16_t(0x8000));

    simValue = HalfFloatWord(uint16_t(0x1234));
    TS_ASSERT_EQUALS(
        simValue.halfFloatWordValue().getBinaryRep(), uint16_t(0x1234));

    // SimValue& operator=(const FloatWord& source)
    simValue = FloatWord(1.0);
    TS_ASSERT_EQUALS(simValue.floatWordValue(), FloatWord(1.0));

    simValue = FloatWord(-1.0);
    TS_ASSERT_EQUALS(simValue.floatWordValue(), FloatWord(-1.0));
 
    // SimValue& operator=(const DoubleWord& source)
    simValue = DoubleWord(1.0);
    TS_ASSERT_EQUALS(simValue.doubleWordValue(), DoubleWord(1.0));

    simValue = DoubleWord(-1.0);
    TS_ASSERT_EQUALS(simValue.doubleWordValue(), DoubleWord(-1.0));
    
    // SimValue& operator=(const SimValue& source)
    simValue = SimValue(0xfedcba98, 32);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-4], 0xfe);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-3], 0xdc);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-2], 0xba);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-1], 0x98);
}

/**
 * Tests addition operators.
 */
void
SimValueTest::testAdditions() {
    // const SimValue operator+(const SIntWord& rightHand)
    SimValue simValue = SimValue(-2, 32) + SIntWord(1);
    TS_ASSERT_EQUALS(simValue.sIntWordValue(), -1);

    // const SimValue operator+(const UIntWord& rightHand)
    simValue.setBitWidth(24);
    simValue = SimValue(0x12345678, 24) + UIntWord(0x7);
    TS_ASSERT_EQUALS(simValue.uIntWordValue(), UIntWord(0x0034567f));
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-3], 0x34);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-2], 0x56);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-1], 0x7f);

    // const SimValue operator+(const HalfFloatWord& rightHand)
    simValue = HalfFloatWord(uint16_t(0x3c00)); // 1.0 + (-2.0) = -1.0
    simValue = simValue + HalfFloatWord(uint16_t(0xc000));
    TS_ASSERT_EQUALS(
        simValue.halfFloatWordValue().getBinaryRep(), uint16_t(0xbc00)); 

    // const SimValue operator+(const FloatWord& rightHand)
    simValue = FloatWord(5.0);
    simValue = simValue + FloatWord(6.5);
    TS_ASSERT_EQUALS(simValue.floatWordValue(), FloatWord(11.5));

    // const SimValue operator+(const DoubleWord& rightHand)
    simValue = DoubleWord(5.0);
    simValue = simValue + DoubleWord(6.5);
    TS_ASSERT_EQUALS(simValue.doubleWordValue(), DoubleWord(11.5));
}

/**
 * Tests minus operators.
 */
void 
SimValueTest::testDifferences() {
    // const SimValue operator-(const SIntWord& rightHand)
    SimValue simValue = SimValue(-20000, 32) - SIntWord(30000);
    TS_ASSERT_EQUALS(simValue.sIntWordValue(), -50000);

    // const SimValue operator-(const UIntWord& rightHand)
    simValue = SimValue(0x12345678, 32) - UIntWord(0x01111111);
    TS_ASSERT_EQUALS(simValue.uIntWordValue(), UIntWord(0x11234567));
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-4], 0x11);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-3], 0x23);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-2], 0x45);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-1], 0x67);

    // const SimValue operator-(const HalfFloatWord& rightHand)
    simValue = HalfFloatWord(uint16_t(0x3c00)); // 1.0 - (-2.0) = 3.0
    simValue = simValue - HalfFloatWord(uint16_t(0xc000)); 
    TS_ASSERT_EQUALS(
        simValue.halfFloatWordValue().getBinaryRep(), uint16_t(0x4200)); 

    // const SimValue operator-(const FloatWord& rightHand)
    simValue = FloatWord(5.0);
    simValue = simValue - FloatWord(6.5);
    TS_ASSERT_EQUALS(simValue.floatWordValue(), FloatWord(-1.5));

    // const SimValue operator-(const DoubleWord& rightHand)
    simValue = DoubleWord(5.0);
    simValue = simValue - DoubleWord(6.5);
    TS_ASSERT_EQUALS(simValue.doubleWordValue(), DoubleWord(-1.5));
}

/**
 * Tests divider operators.
 */
void 
SimValueTest::testDivisions() {
    // const SimValue operator/(const SIntWord& rightHand)
    SimValue simValue = SimValue(-20000, 32) / SIntWord(50);
    TS_ASSERT_EQUALS(simValue.sIntWordValue(), -400);

    // const SimValue operator/(const UIntWord& rightHand)
    simValue = SimValue(16, 32) / UIntWord(2);
    TS_ASSERT_EQUALS(simValue.uIntWordValue(), UIntWord(8));
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-4], 0x00);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-3], 0x00);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-2], 0x00);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-1], 0x08);

    // const SimValue operator/(const HalfFloatWord& rightHand)
    simValue = HalfFloatWord(uint16_t(0x3c00));
    simValue = simValue / HalfFloatWord(uint16_t(0x4000)); // 1.0 / 2.0 = 0.5
    TS_ASSERT_EQUALS(
        simValue.halfFloatWordValue().getBinaryRep(), uint16_t(0x3800)); 

    // const SimValue operator/(const FloatWord& rightHand)
    simValue = FloatWord(5.0);
    simValue = simValue / FloatWord(2.0);
    TS_ASSERT_EQUALS(simValue.floatWordValue(), FloatWord(2.5));

    // const SimValue operator/(const DoubleWord& rightHand)
    simValue = DoubleWord(5.0);
    simValue = simValue / DoubleWord(2.0);
    TS_ASSERT_EQUALS(simValue.doubleWordValue(), DoubleWord(2.5));
}

/**
 * Tests product operators.
 */
void 
SimValueTest::testProducts() {
    // const SimValue operator*(const SIntWord& rightHand)
    SimValue simValue = SimValue(-20000, 32) * SIntWord(50);
    TS_ASSERT_EQUALS(simValue.sIntWordValue(), -1000000);

    // const SimValue operator*(const UIntWord& rightHand)
    simValue = SimValue(16, 32) * UIntWord(2);
    TS_ASSERT_EQUALS(simValue.uIntWordValue(), UIntWord(32));
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-4], 0x00);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-3], 0x00);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-2], 0x00);
    TS_ASSERT_EQUALS(simValue.rawData_[SIMVALUE_MAX_BYTE_SIZE-1], 0x20);

    // const SimValue operator*(const HalfFloatWord& rightHand)
    simValue = HalfFloatWord(uint16_t(0x3c00));
    simValue = simValue * HalfFloatWord(uint16_t(0x4000)); // 1.0 * 2.0 = 2.0
    TS_ASSERT_EQUALS(
        simValue.halfFloatWordValue().getBinaryRep(), uint16_t(0x4000)); 

    // const SimValue operator*(const FloatWord& rightHand)
    simValue = FloatWord(5.0);
    simValue = simValue * FloatWord(4.0);
    TS_ASSERT_EQUALS(simValue.floatWordValue(), FloatWord(20.0));

    // const SimValue operator*(const DoubleWord& rightHand)
    simValue = DoubleWord(4.0);
    simValue = simValue * DoubleWord(2.0);
    TS_ASSERT_EQUALS(simValue.doubleWordValue(), DoubleWord(8.0));
}

/**
 * Tests equality operators.
 */
void 
SimValueTest::testEqualities() {
    // int operator==(const SimValue& rightHand) const
    SimValue simValue(0xf, 32);
    int res;
    res = simValue == SimValue(0xf, 32);
    TS_ASSERT_EQUALS(res, 1);

    // int operator==(const SIntWord& rightHand) const    
    simValue = SIntWord(1);
    res = simValue == SIntWord(-1);
    TS_ASSERT_EQUALS(res, 0);

    // int operator==(const UIntWord& rightHand) const
    simValue = UIntWord(0x12345678);
    res = simValue == UIntWord(0x12345678);
    TS_ASSERT_EQUALS(res, 1);

    // int operator==(const HalfFloatWord& rightHand) const
    simValue = HalfFloatWord(uint16_t(0x4000));
    res = simValue == HalfFloatWord(uint16_t(0x3c00));
    TS_ASSERT_EQUALS(res, 0);

    // int operator==(const FloatWord& rightHand) const
    simValue = FloatWord(44.5);
    res = simValue == FloatWord(44.3);
    TS_ASSERT_EQUALS(res, 0);

    // int operator==(const DoubleWord& rightHand) const
    simValue = DoubleWord(44.5);
    res = simValue == DoubleWord(44.5);
    TS_ASSERT_EQUALS(res, 1);
}

/**
 * Other tests.
 */
void 
SimValueTest::testMisc() {
    SimValue simValue(16);

    simValue.setValue("0x1234");
    TS_ASSERT_EQUALS(simValue.binaryValue(), "0001001000110100");
    TS_ASSERT_EQUALS(simValue.hexValue(), "0x1234");

    simValue.clearToZero(16);
    TS_ASSERT_EQUALS(simValue.hexValue(), "0x0000");
}

#endif
