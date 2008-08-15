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
 * @file Conversion.cc
 *
 * Conversion static class contains functions for converting between
 * different types, e.g. from any (possible) type to a string.
 *
 * @author Pekka J‰‰skel‰inen 2003 (pjaaskel@cs.tut.fi)
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
