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
 * @file Conversion.hh
 *
 * Conversion static class contains functions for converting between
 * different types, e.g. from any (possible) type to a string.
 *
 * Declarations.
 *
 * @author Pekka J‰‰skel‰inen 2003 (pjaaskel-no.spam-cs.tut.fi)
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
    static int toInt(const T& source);

    static int toInt(const double& source);

    template <typename T>
    static unsigned int toUnsignedInt(const T& source);

    template <typename T>
    static double toDouble(const T& source);

    template <typename T>
    static float toFloat(const T& source);

    static std::string toBinString(int source);
    static std::string toBinString(double source);
    static std::string toBinary(
        unsigned int source,
        unsigned int stringWidth = 0);

    template <typename T>
    static std::string toHexString(T source, std::size_t digits = 0);

    static void toRawData(
        const std::string& hexSource, unsigned char* target);

private:
    template <typename SourceType, typename DestType, bool destIsNumeric>
    static void convert(const SourceType& source, DestType& dest);

    static bool restWhiteSpace(std::istream& str);
};
#include "Conversion.icc"

#endif
