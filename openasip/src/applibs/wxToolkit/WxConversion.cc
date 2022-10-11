/*
    Copyright (c) 2002-2013 Tampere University.

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
 * @file WxConversion.cc
 *
 * Implementation on the WxConversion class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2013
 * @note rating: red
 */

#include "WxConversion.hh"
#include "Conversion.hh"

using std::string;

/**
 * Converts an int to a wxString.
 *
 * @param source int to convert.
 * @return The int as a wxString.
 */
wxString
WxConversion::toWxString(const int& source) {
    return wxString((Conversion::toString(source)).c_str(), *wxConvCurrent);
}

/**
 * Converts an unsigned int to a wxString.
 *
 * @param source unsigned int to convert.
 * @return The unsigned int as a wxString.
 */
wxString
WxConversion::toWxString(const unsigned int& source) {
    return wxString((Conversion::toString(source)).c_str(), *wxConvCurrent);
}

/**
 * Converts an long to a wxString.
 *
 * @param source int to convert.
 * @return The int as a wxString.
 */
wxString
WxConversion::toWxString(const SLongWord& source) {
    return wxString((Conversion::toString(source)).c_str(), *wxConvCurrent);
}

/**
 * Converts an unsigned long to a wxString.
 *
 * @param source unsigned int to convert.
 * @return The unsigned int as a wxString.
 */
wxString
WxConversion::toWxString(const ULongWord& source) {
    return wxString((Conversion::toString(source)).c_str(), *wxConvCurrent);
}

/**
 * Converts char to a wxString.
 *
 * @param source char to convert.
 * @return The char as a wxString.
 */
wxString
WxConversion::toWxString(const char& source) {
    return wxString((Conversion::toString(source)).c_str(), *wxConvCurrent);
}


/**
 * Converts a floating point value to wxString.
 *
 * @param source float to convert.
 * @return The floating point value as a wxString.
 */
wxString
WxConversion::toWxString(const float& source) {
    return wxString((Conversion::toString(source)).c_str(), *wxConvCurrent);
}

/**
 * Converts a double precission floating point value to wxString.
 *
 * @param source double to convert.
 * @return The double value as a wxString.
 */
wxString
WxConversion::toWxString(const double& source) {
    return wxString((Conversion::toString(source)).c_str(), *wxConvCurrent);
}

/**
 * Converts an wxChar* array (zero terminated strings) to a char* array.
 *
 * Assumes the client frees the array after use.
 */
char**
WxConversion::toCStringArray(size_t elements, wxChar** source) {
    char** cstringArray = new char*[elements];
    for (size_t i = 0; i < elements; ++i) {
        wxString wxStr(source[i]);
        cstringArray[i] = new char[wxStr.size()];
        cstringArray[i] = strndup((const char*)wxStr.mb_str(), wxStr.size());
    }
    return cstringArray;
}
