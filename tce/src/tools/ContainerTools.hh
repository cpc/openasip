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
 * @file ContainerTools.hh
 * @author Pekka J‰‰skel‰inen (pjaaskel-no.spam-cs.tut.fi) 2003
 *
 * Tools for handling STL containers.
 *
 * Declarations.
 *
 */

#ifndef TTA_CONTAINERTOOLS_HH
#define TTA_CONTAINERTOOLS_HH

class ContainerTools {
public:
    template <typename ContainerType, typename ElementType>
    static bool containsValue(
	const ContainerType& aContainer, const ElementType& aKey);

    template <typename ContainerType, typename ElementType>
    static bool removeValueIfExists(
        ContainerType& aContainer, const ElementType& aKey);

    template <typename ContainerType, typename ElementType>
    static bool deleteValueIfExists(
        ContainerType& aContainer, const ElementType& aKey);

    template <typename ContainerType>
    static void removeValues(
        ContainerType& aContainer,
        const ContainerType& toRemove);

};

#include "ContainerTools.icc"

#endif
