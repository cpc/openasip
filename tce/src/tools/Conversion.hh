/**
 * @file Conversion.hh
 *
 * Conversion static class contains functions for converting between
 * different types, e.g. from any (possible) type to a string.
 *
 * Declarations.
 *
 * @author Pekka J‰‰skel‰inen 2003 (pjaaskel@cs.tut.fi)
 *
 */

#ifndef TTA_CONVERSION_HH
#define TTA_CONVERSION_HH

#include <string>
#include <iostream>

#include <xercesc/util/XMLString.hpp>


#include "Exception.hh"

#if _XERCES_VERSION >= 20200
XERCES_CPP_NAMESPACE_USE
#endif

class Conversion {
public:
    template <typename T>
    static std::string toString(const T& source);
    static std::string toString(bool source);

    static std::string XMLChToString(const XMLCh* source);
    static XMLCh* toXMLCh(const std::string& string);

    template <typename T>
    static int toInt(const T& source)
        throw (NumberFormatException);

    static int toInt(const double& source)
        throw (NumberFormatException);

    template <typename T>
    static unsigned int toUnsignedInt(const T& source)
        throw (NumberFormatException);

    template <typename T>
    static double toDouble(const T& source)
        throw (NumberFormatException);

    template <typename T>
    static float toFloat(const T& source)
        throw (NumberFormatException);

    static std::string toBinString(int source);
    static std::string toBinString(double source);
    static std::string toBinary(
        unsigned int source,
        unsigned int stringWidth = 0);

    template <typename T>
    static std::string toHexString(T source, std::size_t digits = 0);

private:

    template <typename SourceType, typename DestType, bool destIsNumeric>
    static void convert(const SourceType& source, DestType& dest)
        throw (NumberFormatException);

    static bool restWhiteSpace(std::istream& str);


};
#include "Conversion.icc"

#endif
