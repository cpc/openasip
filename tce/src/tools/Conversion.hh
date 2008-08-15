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
