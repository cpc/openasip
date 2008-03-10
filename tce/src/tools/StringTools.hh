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

    static std::string splitToRows(
        const std::string& original,
        const unsigned int rowLength);
    
    static std::string replaceAllOccurrences(
        const std::string& source,
        const std::string& occurrence,
        const std::string& newString);
};

#endif
