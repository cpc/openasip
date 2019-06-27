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
 * @file Conversion.cc
 *
 * Conversion static class contains functions for converting between
 * different types, e.g. from any (possible) type to a string.
 *
 * @author Pekka J‰‰skel‰inen 2003 (pjaaskel-no.spam-cs.tut.fi)
 *
 */
#include <string>
using std::string;

#include <iostream>
using std::istream;
using std::hex;

#include <sstream>
using std::stringstream;


#include "Conversion.hh"

/**
 * Returns true if rest of the stream is only white space.
 *
 * Given stream is read, that is, it is not in the same state as it
 * was before the function call.
 *
 * @param str The stream to check.
 * @return True if there's no garbage in stream.
 */
bool
Conversion::restWhiteSpace(std::istream& str) {

    string temp = "";
    str >> temp;

    if (!str.fail()) {
        // if the stream contains non-white-space characters, the
        // stream will not fail in the next read
        return false;
    }

    return true;
}


/**
 * Converts and integer to the string which is in binary format.
 *
 * Binary format includes a 'b' character at the end of the string.
 *
 * @param source An integer to be converted into a binary string.
 * @return Returns the binary string.
 */
std::string
Conversion::toBinString(int source) {
    string result = toBinary(source);
    result.append("b");
    return result;
}


/**
 * Converts a double to binary representation.
 *
 * @param source Double to be converted.
 * @return Double as binary string.
 */
string
Conversion::toBinString(double source) {

    union castUnion {
        double d;
        struct {int w1; int w2;} s;
    };

    castUnion cast;
    cast.d = source;

    std::string result = "";

    for (unsigned int i = 0; i < sizeof(int) * 8; i++) {

        if ((cast.s.w1 & 0x80000000) != 0) {
            result.append("1");
        } else {
            result.append("0");
        }
        cast.s.w1 <<= 1;
    }

    for (unsigned int i = 0; i < sizeof(int) * 8; i++) {

        if ((cast.s.w2 & 0x80000000) != 0) {
            result.append("1");
        } else {
            result.append("0");
        }
        cast.s.w2 <<= 1;
    }

    string::size_type firstOne = result.find("1");

    if (firstOne != string::npos) {
        result = result.substr(firstOne, string::npos);
    } else {
        result = "0";
    }

    result.append("b");

    return result;
}


/**
 * Converts an integer to the string which is in binary format.
 *
 * The width of the returned string is given too. If the width is
 * greater than required, the number is zero-extended. If the width is
 * smaller than required, the most significant bits are reduced. If '0' is
 * given in stringWidth, the binary string is not extended, nor reduced. This
 * is the default value.
 *
 * @param source An integer to be converted into a binary string.
 * @param stringWidth The width of the returned string.
 * @return The binary string.
 */
std::string
Conversion::toBinary(unsigned int source, unsigned int stringWidth) {

    std::string result = "";
    for (unsigned int i = 0; i < sizeof(unsigned int) * 8; i++) {

        if ((source & 0x80000000) != 0) {
            result.append("1");
        } else {
            result.append("0");
        }
        source <<= 1;
    }

    if (stringWidth == 0) {
        string::size_type firstOne = result.find("1");
        if (firstOne != string::npos) {
            result = result.substr(firstOne, string::npos);
        } else {
            result = "0";
        }
    } else {
        if (result.length() > stringWidth) {
            result = result.substr(
                result.length() - stringWidth, string::npos);
        } else if (result.length() < stringWidth) {
            int zerosToAdd = stringWidth - result.length();
            for (int i = 0; i < zerosToAdd; i++) {
                result.insert(0, "0");
            }
        }
    }

    return result;
}

/**
 * Converts bytes of a string hexadecimal value to target buffer.
 *
 * Hex value can be in "0x" format or without the prefix. Caller is 
 * responsible for checking that the target buffer has enough allocated
 * memory for conversion.
 *
 * @param source Hex value.
 * @param target Buffer where hex value bits are converted to.
 */
void 
Conversion::toRawData(const std::string& hexSource, unsigned char* target) {
    std::string hexValue = hexSource;
    std::stringstream hexStream;
    hexStream << hexSource;
    char first = hexStream.get();
    char second = hexStream.get();

    if (first == '0' && second == 'x') {
        hexValue = hexSource.substr(2); // remove "0x"
    }

    // if number of hex numbers is uneven (e.g "12345FA"), insert additional
    // 0 to the front so that the hex numbers form N full bytes
    if (hexValue.size() % 2 == 1) {
        hexValue.insert(0, "0");
    }
    
    // start filling the input parameter 2 hex numbers (byte) at a time to
    // the data buffer
    for (size_t i = 0; i < hexValue.size(); i=i+2) {
        *target = toInt("0x" + hexValue.substr(i, 2)); 
        ++target;
    }
}
