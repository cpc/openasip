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
