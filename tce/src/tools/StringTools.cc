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
 * @file StringTools.cc
 *
 * Definitions.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <cctype>
#include <algorithm>
#include <sstream>

#include "StringTools.hh"


using std::string;
using std::vector;
using std::isspace;
using std::copy;
using std::istringstream;


/**
 * Removes leading and trailing whitespace from the string.
 *
 * @return The trimmed string.
 * @param source The string to trim.
 */
std::string
StringTools::trim(const std::string& source) {

    string result = "";
    int i = 0;

    // remove leading white space
    while (i < static_cast<int>(source.size()) && isspace(source[i])) {
        ++i;
    }
    result = source.substr(i);

    i = result.size() - 1;
    while (i >= 0 && isspace(result[i])) {
        --i;
    }
    result = result.substr(0, i+1);

    return result;
}


/**
 * Converts string to char*.
 *
 * @param source The string to be converted.
 * @return The string as a char*.
 */
char*
StringTools::stringToCharPtr(const std::string& source) {
    char* ch = new char[source.size() + 1];
    copy(source.begin(), source.end(), ch);
    ch[source.size()] = 0;
    return ch;
}


/**
 * Checks whether a string contains a char or not.
 *
 * @param source The investigated string.
 * @param ch Character which is checked whether it is in a string or not.
 * @param caseSensitive Flag indicating whether checking is case sensitive
 *        or not.
 * @return True, if source string contains ch, false otherwise.
 */
bool
StringTools::containsChar(
    const std::string& source,
    char ch,
    bool caseSensitive) {

    string::size_type pos = 0;
    if (!caseSensitive) {
        char upC = toupper(ch);
        string upString = StringTools::stringToUpper(source);
        pos = upString.find(upC, 0);
    } else {
        pos = source.find(ch, 0);
    }

    return pos != string::npos;
}

/**
 * Checks whether a string ends with the given search string.
 *
 * @param source The investigated string.
 * @param searchString The string to search from the end.
 * @return True, if source ends with searchString.
 */
bool
StringTools::endsWith(
    const std::string& source,
    const std::string& searchString) {

    return source.size() >= searchString.size() &&
        source.substr(
            source.size() - searchString.size(), searchString.size()) ==
        searchString;
}


/**
 * Converts a string to upper case letters.
 *
 * @param source String to be converted.
 * @return String as upper case letters.
 */
string
StringTools::stringToUpper(const std::string& source) {
    string upString;
    upString.reserve(source.length());
    for (unsigned int i = 0; i < source.length(); ++i) {
        upString.push_back(toupper(source[i]));
    }
    return upString;
}


/**
 * Converts a string to lower case letters.
 *
 * @param source String to be converted.
 * @return String as lower case letters.
 */
string
StringTools::stringToLower(const std::string& source) {

    string lowString;
    lowString.reserve(source.length());
    for (unsigned int i = 0; i < source.length(); ++i) {
        lowString.push_back(tolower(source[i]));
    }
    return lowString;
}


/**
 * Chops string using a given delimiter.
 *
 * Result is returned as a vector. Extra blanks are ignored.
 *
 * @param source String to be chopped.
 * @param delimiter Delimiter used.
 * @return A vector that contains chopped strings.
 */
vector<TCEString>
StringTools::chopString(
    const std::string& source,
    const std::string& delimiter) {

    string line = trim(source);
    vector<TCEString> results;
    while (line.length() > 0) {
        string::size_type location = line.find(delimiter);
        if (location == string::npos) {
            results.push_back(line);
            line = "";
        } else {
            results.push_back(line.substr(0, location));
            line.replace(0, location + 1, "");
            line = trim(line);
        }
    }
    return results;
}


/**
 * Chops string using a given delimiter.
 *
 * Result is returned as a vector. Extra blanks are ignored.
 *
 * @param source String to be chopped.
 * @param delimiter Delimiter used.
 * @param store String vector reference where result strings are stored.
 */
void
StringTools::chopString(
    const std::string& source,
    const std::string& delimiter, 
    std::vector<std::string>& results) {

    string line = trim(source);
    while (line.length() > 0) {
        string::size_type location = line.find(delimiter);
        if (location == string::npos) {
            results.push_back(line);
            line = "";
        } else {
            results.push_back(line.substr(0, location));
            line.replace(0, location + 1, "");
            line = trim(line);
        }
    }
}


/**
 * Returns true if two strings are equal if the case is not taken into account.
 *
 * @param a A string.
 * @param b Another string.
 * @return True if strings equal case-insensitively.
 */
bool
StringTools::ciEqual(const std::string& a, const std::string& b) {
    return (stringToLower(a) == stringToLower(b));
}


/**
 * Splits a string into rows.
 *
 * The original string is split in to rows containing a maximum of 'rowLength'
 * characters. After 'rowLength' characters a new-line character is inserted
 * to the string to be returned. If the given string has less than 'rowLength'
 * characters, the string is returned as it was. Also, the "last row" may have
 * less than 'rowLength' characters.
 *
 * If 'rowLength' is 0, the original string is returned.
 *
 * @param original The string to be split into rows.
 * @param rowLength The length of a row in the returned string.
 * @return A string that has been split into rows containing a maximum of
 *         'rowLength' characters.
 */
std::string
StringTools::splitToRows(
    const std::string& original,
    const unsigned int rowLength) {

    if (rowLength == 0) {
        return original;
    } else {

        unsigned int counter = 0;
        string newString = "";

        while (counter < original.size()) {
            newString += original.substr(counter, 1);
            counter++;

            if (counter % rowLength == 0 && counter != original.size()) {
                newString += "\n";
            }
        }

        return newString;
    }
}

/**
 * Replaces all occurrences of 'occurrence' in 'source' with 'newString'
 * 
 * @param source The source string to be modified
 * @param occurrence The string occurrences to be replaced
 * @param newString The string that's replaced over the found occurrences
 * @return A string containing the modifications
 */ 
std::string
StringTools::replaceAllOccurrences(
        const std::string& source,
        const std::string& occurrence,
        const std::string& newString) {

    std::string modifiedString(source);
    std::string::size_type location = modifiedString.find(occurrence);
    while (location != std::string::npos) {
        modifiedString.replace(modifiedString.begin() + location,
                       modifiedString.begin() + location + occurrence.length(),
                       newString.c_str());
        location = modifiedString.find(occurrence);
    }
    
    return modifiedString;
}

/**
 * Returns a (vhdl) indention string 
 *
 * @param level Indention level (one level is 2 spaces)
 * @return Indention string
 */
std::string 
StringTools::indent(int level) {
    string indent;
    for (int i = 0; i < level; i++) {
        indent += "  ";
    }
    return indent;
}
