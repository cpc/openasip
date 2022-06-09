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
 * @file ConversionTest.hh 
 *
 * A test suite for Conversion.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2005 (pekka.jaaskelainen-no.spam-tut.fi)
 */

#ifndef TTA_CONVERSION_TEST_HH
#define TTA_CONVERSION_TEST_HH

#include <TestSuite.h>
#include <string>
#include <climits>

#include "Conversion.hh"
#include "Exception.hh"
#include "Application.hh"

using std::string;

/**
 * A test class for Conversion class.
 *
 * @todo: Conversions from XMLCh to string and vice versa.
 */
class ConversionTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testToInt();
    void testToUnsignedInt();
    void testStringToDouble();
    void testToFloat();
    void testToString();
    void testIntToBinString();
    void testIntToHexString();
    void xtestXMLChToString();
    void xtestStringtoXMLCh();

private:
    static const double epsilon;
};

const double ConversionTest::epsilon = 1e-6;


/**
 * Called before each test.
 */
void
ConversionTest::setUp() {
}


/**
 * Called after each test.
 */
void
ConversionTest::tearDown() {
}


/**
 * Test that string and unsigned integer to integer conversion works.
 *
 * Illegal strings are also tested.
 */
void
ConversionTest::testToInt() {
    TS_ASSERT_EQUALS(Conversion::toInt("12"), 12);
    TS_ASSERT_EQUALS(Conversion::toInt("03"), 3);
    TS_ASSERT_EQUALS(Conversion::toInt("-34"), -34);
    TS_ASSERT_EQUALS(Conversion::toInt("  -02  "), -2);

    // from hex format strings to int
    TS_ASSERT_EQUALS(Conversion::toInt("0x0012"), 18);
    TS_ASSERT_EQUALS(Conversion::toInt("0xabcdef"), 11259375);
    TS_ASSERT_EQUALS(Conversion::toInt("0xABCDEF"), 11259375);
    TS_ASSERT_EQUALS(Conversion::toInt("0xffffffff"), -1);

    // from binary format strings to int
    TS_ASSERT_EQUALS(Conversion::toInt("1001b"), 9);
    TS_ASSERT_EQUALS(Conversion::toInt("111001b"), 57);
    TS_ASSERT_EQUALS(Conversion::toInt("0b "), 0);
    TS_ASSERT_EQUALS(Conversion::toInt("11b "), 3);
    TS_ASSERT_EQUALS(Conversion::toInt("00000000000000000000000000000000b"),
                     0);
    TS_ASSERT_EQUALS(Conversion::toInt("1110111101111b"), 7663);
    TS_ASSERT_EQUALS(Conversion::toInt("11111111111111111111111111111111b"),
                     -1);
    TS_ASSERT_EQUALS(Conversion::toInt("01111111111111111111111111111111b"),
                     2147483647);

    // illegal integers should throw an exception
    TS_ASSERT_THROWS(Conversion::toInt("c2"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("2c"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("23 failure"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("23 23"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("23c2"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt(""), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("   "), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("2500000000"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("4294967298f"), NumberFormatException);

    TS_ASSERT_THROWS(Conversion::toInt("0x23 failure"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("0x23 23"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("-0x2a 23"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("-0x23"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("0xabce23h"), NumberFormatException);

    TS_ASSERT_THROWS(Conversion::toInt("111021b"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("-010101b"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("b"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("1011b 23"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("1011b 1001b"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toInt("1011 1001b"), NumberFormatException);

    // unsigned integer conversion
    unsigned int test = 0;
    TS_ASSERT_EQUALS(Conversion::toInt(test), 0);
    test = 1234567890;
    TS_ASSERT_EQUALS(Conversion::toInt(test), 1234567890);

    // number with starting '1' will be too high to be represented by int
    test = 0xfabcdef0;
    TS_ASSERT_THROWS(Conversion::toInt(test), NumberFormatException);
    test = 0xA0000000;
    TS_ASSERT_THROWS(Conversion::toInt(test), NumberFormatException);

    //TS_ASSERT_EQUALS(Conversion::toInt(1.0e7), 1000000);
    CATCH_ANY(Conversion::toInt(1.0e7));
}


/**
 * Tests that converting string to unsigned integer works.
 */
void
ConversionTest::testToUnsignedInt() {

    unsigned int test = 12;
    TS_ASSERT_EQUALS(Conversion::toUnsignedInt("12"), test);
    test = 3;
    TS_ASSERT_EQUALS(Conversion::toUnsignedInt("03"), test);

    // from hex format strings to unsigned int
    test = 18;
    TS_ASSERT_EQUALS(Conversion::toUnsignedInt("0x0012"), test);
    test = 11259375;
    TS_ASSERT_EQUALS(Conversion::toUnsignedInt("0xabcdef"), test);
    TS_ASSERT_EQUALS(Conversion::toUnsignedInt("0xABCDEF"), test);
    if (sizeof(unsigned int) == 4) {
        TS_ASSERT_EQUALS(Conversion::toUnsignedInt("0xffffffff"), UINT_MAX);
    }

    // from binary format strings to unsigned int
    test = 9;
    TS_ASSERT_EQUALS(Conversion::toUnsignedInt("1001b"), test);
    test = 57;
    TS_ASSERT_EQUALS(Conversion::toUnsignedInt("111001b"), test);
    test = 0;
    TS_ASSERT_EQUALS(Conversion::toUnsignedInt("0b "), test);
    test = 3;
    TS_ASSERT_EQUALS(Conversion::toUnsignedInt("11b "), test);
    test = 0;
    TS_ASSERT_EQUALS(
        Conversion::toUnsignedInt("00000000000000000000000000000000b"), test);
    test = 7663;
    TS_ASSERT_EQUALS(Conversion::toUnsignedInt("1110111101111b"), test);
    if (sizeof(unsigned int) == 4) {
        TS_ASSERT_EQUALS(
            Conversion::toUnsignedInt("11111111111111111111111111111111b"),
            UINT_MAX);
    }

    // illegal unsigned integers should throw an exception
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("-34"), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("  -02  "), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toUnsignedInt("c2"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toUnsignedInt("2c"), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("23 failure"), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("23 23"), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("23c2"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toUnsignedInt(""), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("   "), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("5000000000"), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("4294967298f"), NumberFormatException);

    if (sizeof(unsigned int) == 4) {
        TS_ASSERT_THROWS(
            Conversion::toUnsignedInt("111111111111111111111111111111111b"),
            NumberFormatException);
    }

    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("0x23 failure"), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("0x23 23"), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("-0x2a 23"), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("-0x23"), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("0xabce23h"), NumberFormatException);

    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("111021b"), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("-010101b"), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("b"), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("1011b 23"), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("1011b 1001b"), NumberFormatException);
    TS_ASSERT_THROWS(
        Conversion::toUnsignedInt("1011 1001b"), NumberFormatException);
}
    

/**
 * Test that string to double conversion works.
 *
 * Illegal strings are also tested.
 */
void
ConversionTest::testStringToDouble() {
    TS_ASSERT_EQUALS(Conversion::toDouble("1.2"), 1.2);
    TS_ASSERT_EQUALS(Conversion::toDouble("12"), 12);
    TS_ASSERT_EQUALS(Conversion::toDouble("03"), 3);
    TS_ASSERT_EQUALS(Conversion::toDouble("03.40"), 3.4);
    TS_ASSERT_EQUALS(Conversion::toDouble("-3.4"), -3.4);
    TS_ASSERT_EQUALS(Conversion::toDouble("-003.40"), -3.4);
    TS_ASSERT_EQUALS(Conversion::toDouble("-34"), -34);
    TS_ASSERT_EQUALS(Conversion::toDouble("  -02  "), -2);
    TS_ASSERT_EQUALS(Conversion::toDouble("  -02.0  "), -2);
    TS_ASSERT_EQUALS(Conversion::toDouble("  2e-2  "), 2e-2);
    TS_ASSERT_EQUALS(Conversion::toDouble("2e-256"), 2e-256);
    TS_ASSERT_DELTA(Conversion::toDouble("2e50"), 2e50, epsilon);

    TS_ASSERT_THROWS(Conversion::toDouble(""), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toDouble("-2.0 2"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toDouble("c2.0"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toDouble("2.0c"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toDouble("31.0 c"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toDouble(" -2.0c0"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toDouble("0x09"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toDouble("0x09.9"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toDouble("1e400"), NumberFormatException);
}

/**
 * Test that string and double to float conversion works.
 *
 * Illegal strings are also tested.
 */
void
ConversionTest::testToFloat() {

    // from string to float
    TS_ASSERT_DELTA(Conversion::toFloat("1.2"), 1.2, epsilon);
    TS_ASSERT_DELTA(Conversion::toFloat("12"), 12, epsilon);
    TS_ASSERT_DELTA(Conversion::toFloat("03"), 3, epsilon);
    TS_ASSERT_DELTA(Conversion::toFloat("03.40"), 3.4, epsilon);
    TS_ASSERT_DELTA(Conversion::toFloat("-3.4"), -3.4, epsilon);
    TS_ASSERT_DELTA(Conversion::toFloat("-003.40"), -3.4, epsilon);
    TS_ASSERT_DELTA(Conversion::toFloat("-34"), -34, epsilon);
    TS_ASSERT_DELTA(Conversion::toFloat("  -02  "), -2, epsilon);
    TS_ASSERT_DELTA(Conversion::toFloat("  -02.0  "), -2, epsilon);
    TS_ASSERT_DELTA(Conversion::toFloat("  2e-2  "), 2e-2, epsilon);
    TS_ASSERT_DELTA(Conversion::toFloat("  2e30  "), 2e30, 2e30*epsilon);

    TS_ASSERT_THROWS(Conversion::toFloat(""), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toFloat("-2.0 2"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toFloat("c2.0"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toFloat("2.0c"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toFloat("31.0 c"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toFloat(" -2.0c0"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toFloat("0x02"), NumberFormatException);
    TS_ASSERT_THROWS(Conversion::toFloat("1e400"), NumberFormatException);

    // from double to float
    double test = 0.0;
    TS_ASSERT_DELTA(Conversion::toFloat(test), test, test*epsilon);
    test = 1;
    TS_ASSERT_DELTA(Conversion::toFloat(test), test, test*epsilon);
    test = -8.918273;
    TS_ASSERT_DELTA(Conversion::toFloat(test), test, -test*epsilon);
    test = 1.05867;
    TS_ASSERT_DELTA(Conversion::toFloat(test), test, test*epsilon);
    test = 1e20;
    TS_ASSERT_DELTA(Conversion::toFloat(test), test, test*epsilon);
}

/**
 * Test that conversions from number formats to string work.
 *
 * Illegal strings are also tested.
 */
void
ConversionTest::testToString() {
    // from integer to string
    TS_ASSERT_EQUALS(Conversion::toString(12), "12");
    TS_ASSERT_EQUALS(Conversion::toString(-12), "-12");
    TS_ASSERT_EQUALS(Conversion::toString(012), "10");
    TS_ASSERT_EQUALS(Conversion::toString(0x0012), "18");

    // from double to string
    TS_ASSERT_EQUALS(Conversion::toString(3.4), "3.4");
    TS_ASSERT_EQUALS(Conversion::toString(03.40), "3.4");
    TS_ASSERT_EQUALS(Conversion::toString(-03.40), "-3.4");
    TS_ASSERT_EQUALS(Conversion::toString(34.0), "34");
    TS_ASSERT_EQUALS(Conversion::toString(-34.0), "-34");

    // from float to string
    TS_ASSERT_EQUALS(Conversion::toString(3.4f), "3.4");
    TS_ASSERT_EQUALS(Conversion::toString(03.40f), "3.4");
    TS_ASSERT_EQUALS(Conversion::toString(-03.40f), "-3.4");
    TS_ASSERT_EQUALS(Conversion::toString(34.0f), "34");
    TS_ASSERT_EQUALS(Conversion::toString(-34.0f), "-34");
}

/**
 * Test that conversion from integer to binary string works.
 */
void
ConversionTest::testIntToBinString() {

    string test = "";

    test = "11010b";
    TS_ASSERT_EQUALS(Conversion::toBinary(Conversion::toInt(test), 9), 
                     "000011010");
    TS_ASSERT_EQUALS(Conversion::toBinary(Conversion::toInt(test), 3), 
                     "010");
    TS_ASSERT_EQUALS(Conversion::toBinary(Conversion::toInt(test)),
                     "11010");

    TS_ASSERT_EQUALS(Conversion::toBinString(33), "100001b");
    TS_ASSERT_EQUALS(Conversion::toBinString(-1),
                     "11111111111111111111111111111111b");
    TS_ASSERT_EQUALS(Conversion::toBinString(-4),
                     "11111111111111111111111111111100b");

    test = "11110111101111b";
    TS_ASSERT_EQUALS(Conversion::toBinString(Conversion::toInt(test)), test);
    test = "0b";
    TS_ASSERT_EQUALS(Conversion::toBinString(Conversion::toInt(test)), test);
    test = "1b";
    TS_ASSERT_EQUALS(Conversion::toBinString(Conversion::toInt(test)), test);
    test = "00000000000000000000000000000000b";
    TS_ASSERT_EQUALS(Conversion::toBinString(Conversion::toInt(test)), "0b");
    test = "11111111111111111111111111111111b";
    TS_ASSERT_EQUALS(Conversion::toBinString(Conversion::toInt(test)), test);
    test = "1111111111111111111111111111111b";
    TS_ASSERT_EQUALS(Conversion::toBinString(Conversion::toInt(test)), test);
    test = "00011111111111111b";
    TS_ASSERT_EQUALS(Conversion::toBinString(Conversion::toInt(test)),
                     "11111111111111b");
}

/**
 * Test that conversion from integer to hexadecimal string works.
 */
void
ConversionTest::testIntToHexString() {
    string test = "";

    TS_ASSERT_EQUALS(Conversion::toHexString(33), "0x21");
    TS_ASSERT_EQUALS(Conversion::toHexString(-1), "0xffffffff");
    TS_ASSERT_EQUALS(Conversion::toHexString(-4), "0xfffffffc");

    test = "0xabcdef";
    TS_ASSERT_EQUALS(Conversion::toHexString(Conversion::toInt(test)), test);
    test = "0xABCDEF";
    TS_ASSERT_EQUALS(Conversion::toHexString(Conversion::toInt(test)), 
                     "0xabcdef");
    test = "0xabcdefab";
    TS_ASSERT_EQUALS(Conversion::toHexString(Conversion::toInt(test)), test);
    test = "0x0";
    TS_ASSERT_EQUALS(Conversion::toHexString(Conversion::toInt(test)), test);
    test = "0x1";
    TS_ASSERT_EQUALS(Conversion::toHexString(Conversion::toInt(test)), test);
    test = "0x00000000";
    TS_ASSERT_EQUALS(Conversion::toHexString(Conversion::toInt(test)), "0x0");
}

#endif
