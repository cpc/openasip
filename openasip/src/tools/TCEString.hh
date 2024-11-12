/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file TCEString.hh
 *
 * Declaration of TCEString class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011
 * @note rating: red
 */

#ifndef TCE_STRING_HH
#define TCE_STRING_HH

#include <string>
#include <vector>
#include <locale>


class TCEString;

// Type aliases
typedef const std::string& stringCRef;
typedef const TCEString& TCEStringCRef;

/**
 * A simple wrapper around std::string to make forward declarations possible.
 * 
 * Also provides additional string helpers.
 */
class TCEString : public std::string {
public:
    TCEString();
    TCEString(const char* text);
    TCEString(const std::string& text);
    TCEString(const char c); 
    virtual ~TCEString();

    TCEString& replaceString(
        const std::string& old, const std::string& newString);
    bool startsWith(const std::string& str) const;
    bool endsWith(const std::string& str) const;
    bool ciEqual(const TCEString& other) const;
    std::vector<TCEString> split(const std::string& delim) const;
    TCEString& appendIf(bool expression, stringCRef ifTrue);

    TCEString lower() const;
    TCEString upper() const;
    TCEString capitalize() const;

    TCEString operator+(int val) const;
    TCEString operator+(char c) const;

    TCEString& operator<<(const TCEString& rhs);
    TCEString& operator<<(const char* rhs);
    TCEString& operator<<(const int rhs);

    static TCEString toUpper(
        const TCEString& str, const std::locale& loc = std::locale());
    static std::string toUpper(
        const std::string& str, const std::locale& loc = std::locale());
    static TCEString& toUpper(
        TCEString& str, const std::locale& loc = std::locale());
    static std::string& toUpper(
        std::string& str, const std::locale& loc = std::locale());
    static std::string toLower(
            const std::string& str, const std::locale& loc = std::locale());

    static std::string& appendToNonEmpty(
        std::string& toAppend, stringCRef appender);
    static std::string appendToNonEmpty(
        stringCRef toAppend, stringCRef appender);
    static std::string applyIf(
        bool expression, stringCRef ifTrue, stringCRef ifFalse);
    static std::string applyIf(
        bool expression, stringCRef ifTrue);
    template<typename IntegerType>
    static std::string& appendInteger(
        std::string& toAppend,
        const IntegerType& appender);
    template<typename IntegerType>
    static std::string appendInteger(
        stringCRef toAppend,
        const IntegerType& appender);

    template<typename IterableContainer>
    static std::string makeString(
        const IterableContainer& container,
        const std::string& separator = ", ");

    static unsigned replace(
        std::string& str,
        const std::string& oldPattern,
        const std::string& newPattern);

    static std::string filterDigits(const std::string& str);

    static std::string intToHexString(int num);
    static std::string unsignedToHexString(unsigned num);

    struct ICLess {
        bool operator() (const TCEString& lhs, const TCEString& rhs) const;
    };

private:
};



#include "TCEString.icc"

#endif
