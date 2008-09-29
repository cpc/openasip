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
 * @file StringTools.hh
 *
 * Helper functions for handling strings.
 *
 * Declarations.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_STRINGTOOLS_HH
#define TTA_STRINGTOOLS_HH

#include <string>
#include <vector>


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

    static std::vector<std::string> chopString(
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
};

#endif
