/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file TCEString.cc
 *
 * Definition of TCEString class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */
#include <string>
#include "TCEString.hh"
#include "Conversion.hh"

TCEString::TCEString() : std::string() {
}

TCEString::TCEString(const char* text) : std::string(text) {
}

TCEString::TCEString(const std::string& text) : std::string(text) {
}

TCEString::TCEString(const char c) : std::string(std::string("") + c) {
}

TCEString::~TCEString() {
}

/**
 * Returns true if two strings are equal if the case is not taken into account.
 *
 * @param a A string.
 * @param b Another string.
 * @return True if strings equal case-insensitively.
 */
bool
TCEString::ciEqual(const TCEString& other) const {
    unsigned int len = length();
    if (len != other.length()) return false;
    for (unsigned int i = 0; i < len; i++) {
        if (tolower((*this)[i]) != tolower(other[i])) {
            return false;
        }
    }
    return true;
}

TCEString
TCEString::lower() const {
    return StringTools::stringToLower(*this);
}


TCEString
TCEString::upper() const {
    return StringTools::stringToUpper(*this);
}

/**
 * Replaces all occurences of string 'old' with 'newString'
 */
TCEString&
TCEString::replaceString(
    const std::string& old, const std::string& newString) {
        
    TCEString modifiedString(*this);
    std::string::size_type location = modifiedString.find(old);
    while (location != std::string::npos) {
        modifiedString.replace(
            modifiedString.begin() + location,
            modifiedString.begin() + location + old.length(),
            newString.c_str());
        // Avoid infinite recursion if replacing with a string
        // that also contains the searched string. This happens
        // when escaping reserved characters such as '_' -> '\\_'.
        location = modifiedString.find(
            old, std::distance(
                modifiedString.begin(), 
                modifiedString.begin() + location + newString.length() + (size_t)1));
    }
    *this = modifiedString;
    return *this;
}

/**
 * Return a copy of the string with its first character in upper case and 
 * the rest in lower case.
 */
TCEString
TCEString::capitalize() const {
    if (size() == 0) return "";
    if (size() == 1) return upper();
    return StringTools::stringToUpper(substr(0, 1)) + 
        StringTools::stringToLower(substr(1, size() - 1));
}

std::vector<TCEString>
TCEString::split(const std::string& delim) const {
    return StringTools::chopString(*this, delim);
}

// stream operators for easier string construction
TCEString&
TCEString::operator<<(const TCEString& rhs) {
    *this += rhs;
    return *this;
}

TCEString&
TCEString::operator<<(const char* rhs) {
    *this += TCEString(rhs);
    return *this;
}

TCEString&
TCEString::operator<<(const int rhs) {
    *this += Conversion::toString(rhs);
    return *this;
}
