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
 * @file TCEString.cc
 *
 * Definition of TCEString class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>

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

/**
 * Turns the string to lowercase.
 */
TCEString
TCEString::lower() const {
    return StringTools::stringToLower(*this);
}

/**
 * Turns the string to uppercase.
 */
TCEString
TCEString::upper() const {
    return StringTools::stringToUpper(*this);
}

/**
 * Replaces all occurrences of string 'old' with 'newString'
 */
TCEString&
TCEString::replaceString(
    const std::string& old, const std::string& newString) {

    TCEString::replace(*this, old, newString);
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

TCEString&
TCEString::appendIf(bool expression, stringCRef ifTrue) {
    if (expression) {
        append(ifTrue);
    }
    return *this;
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

TCEString
TCEString::operator+(int val) const {
    return TCEString(*this) += Conversion::toString(val);
}

TCEString
TCEString::operator+(char c) const {
    return *this + TCEString(c);
}

TCEString
TCEString::toUpper(const TCEString& str, const std::locale& loc) {
    TCEString newStr(str);
    return toUpper(newStr, loc);
}

std::string
TCEString::toUpper(const std::string& str, const std::locale& loc) {
    std::string newStr(str);
    return toUpper(newStr, loc);
}

TCEString&
TCEString::toUpper(TCEString& str, const std::locale& loc) {
    for (size_t i = 0; i < str.size(); i++) {
        str.at(i) = std::toupper(str.at(i), loc);
    }
    return str;
}

std::string&
TCEString::toUpper(std::string& str, const std::locale& loc) {
    for (size_t i = 0; i < str.size(); i++) {
        str.at(i) = std::toupper(str.at(i), loc);
    }
    return str;
}

std::string
TCEString::toLower(
    const std::string& str, const std::locale& loc) {
    std::string result(str);
    for (size_t i = 0; i < result.size(); i++) {
        result.at(i) = std::tolower(result.at(i), loc);
    }
    return result;
}

/**
 * Appends a string if the target string is not empty.
 *
 * @param toAppend The target string.
 * @param appender The string to append with.
 * @return Reference to original possibly appended with appender.
 */
std::string&
TCEString::appendToNonEmpty(
    std::string& toAppend, stringCRef appender) {
    if (!toAppend.empty()) {
        return toAppend += appender;
    } else {
        return toAppend;
    }
}

/**
 * Returns string of target string appended with another string if the target
 * string is not empty.
 *
 * @param toAppend The target string.
 * @param appender The string to append with.
 * @return String initialized with toAppend and possibly appended with appender.
 */
std::string
TCEString::appendToNonEmpty(
    stringCRef toAppend, stringCRef appender) {
    std::string tmp(toAppend);
    return appendToNonEmpty(tmp, appender);
}

/**
 * Returns first string if "expression" is true. Otherwise return second.
 */
std::string
TCEString::applyIf(bool expression, stringCRef ifTrue, stringCRef ifFalse) {
    if (expression) {
        return ifTrue;
    } else {
        return ifFalse;
    }
}

/**
 * Returns string if "expression" is true. Otherwise return empty string.
 */
std::string
TCEString::applyIf(bool expression, stringCRef ifTrue) {
   return applyIf(expression, ifTrue, "");
}

/**
 * Replaces all occurrences of oldPattern in str with newPattern.
 *
 * @return Number of made replacements.
 */
unsigned
TCEString::replace(
    std::string& str,
    const std::string& oldPattern,
    const std::string& newPattern) {

    unsigned replacementCount = 0;

    std::string::size_type location = str.find(oldPattern);
    while (location != std::string::npos) {
        str.replace(
            str.begin() + location,
            str.begin() + location + oldPattern.length(),
            newPattern.c_str());
        replacementCount++;
        // Avoid infinite recursion if replacing with a string
        // that also contains the searched string. This happens
        // when escaping reserved characters such as '_' -> '\\_'.
        location = str.find(oldPattern, location + newPattern.length());
    }
    return replacementCount;
}


/**
 * Returns string in which all non-digit characters have been removed.
 */
std::string
TCEString::filterDigits(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (auto c : str) {
        if (std::isdigit(c)) result += c;
    }
    return result;
}

std::string
TCEString::intToHexString(int num) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << num;
    return ss.str();
}

std::string
TCEString::unsignedToHexString(unsigned num) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << num;
    return ss.str();
}


/**
 * Implementation of lhs < rhs string comparison case insensitively.
 *
 * @param lhs The left side string.
 * @param rhs Thr rigth side string.
 * @return Boolean result from comparison.
 */
bool
TCEString::ICLess::operator() (
    const TCEString& lhs, const TCEString& rhs) const {
    return lhs.lower() < rhs.lower();
}


