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
 * @file StringTools.hh
 *
 * Helper functions for handling strings.
 *
 * @deprecated New helpers should be added to TCEString instead.
 *
 * Declarations.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_STRINGTOOLS_HH
#define TTA_STRINGTOOLS_HH

#include <string>
#include <vector>
#include "TCEString.hh"

class StringTools {

public:
    static std::string trim(const std::string& source);
    static char* stringToCharPtr(const std::string& source);

    static bool containsChar(
        const std::string& source,
        char ch,
        bool caseSensitive = true);

    static bool endsWith(
        const std::string& source,
        const std::string& searchString);

    static std::string stringToUpper(const std::string& source);
    static std::string stringToLower(const std::string& source);
    static bool ciEqual(const std::string& a, const std::string& b);

    static std::vector<TCEString> chopString(
        const std::string& source,
        const std::string& delimiter);

    static void chopString(
        const std::string& source,
        const std::string& delimiter, 
        std::vector<std::string>& results);

    static std::string splitToRows(
        const std::string& original,
        const unsigned int rowLength);
    
    static std::string replaceAllOccurrences(
        const std::string& source,
        const std::string& occurrence,
        const std::string& newString);

    static std::string indent(int level);
};

#endif
