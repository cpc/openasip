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
 * @file AssocTools.hh
 * @author Pekka J‰‰skel‰inen (pekka.jaaskelainen-no.spam-tut.fi) 2004
 *
 * Tools for handling STL Associative Containers (usually set).
 *
 * Declarations.
 *
 */

#ifndef TTA_ASSOC_TOOLS_HH
#define TTA_ASSOC_TOOLS_HH

#include <set>
#include <utility>

#include "Exception.hh"

class AssocTools {
public:
    template <typename ContainerType>
    static void deleteAllItems(ContainerType& aMap);

    template <typename ContainerType>
    static void deleteAllValues(ContainerType& aMap);

    template <typename ContainerType, typename KeyType>
    static bool containsKey(
        const ContainerType& aContainer,
        const KeyType& aKey);

    template <typename ContainerType>
    static void difference(
        const ContainerType& firstContainer,
        const ContainerType& secondContainer,
        ContainerType& difference);

    template <typename ContainerType1, typename ContainerType2>
    static std::set<std::pair<
        typename ContainerType1::value_type, 
        typename ContainerType2::value_type> > pairs(
        ContainerType1& firstContainer,
        ContainerType2& secondContainer);

    template <typename Comparator,typename ContainerType1, 
              typename ContainerType2> 
    static std::set<std::pair<
                        typename ContainerType1::value_type, 
        typename ContainerType2::value_type>, Comparator > pairs(
        ContainerType1& firstContainer,
        ContainerType2& secondContainer);
        
    template <typename ContainerType>
    static void append(const ContainerType& src, ContainerType &dest);
};

#include "AssocTools.icc"

#endif
