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
 * @file TCEString.hh
 *
 * Declaration of TCEString class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TCE_STRING_HH
#define TCE_STRING_HH

#include <string>
#include <vector>

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

    TCEString lower() const;
    TCEString upper() const;
    TCEString capitalize() const;

    TCEString& operator<<(const TCEString& rhs);
    TCEString& operator<<(const char* rhs);
    TCEString& operator<<(const int rhs);

    struct ICLess {
        bool operator() (const TCEString& lhs, const TCEString& rhs) const;
    };

private:
};

#include "TCEString.icc"

#endif
