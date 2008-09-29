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
 * @file MapTools.hh
 *
 * @author Pekka J‰‰skel‰inen (pekka.jaaskelainen-no.spam-tut.fi) 2004
 *
 * Tools for handling STL Pair Associative Containers (usually map).
 *
 * Declarations.
 *
 */

#ifndef TTA_MAP_TOOLS_HH
#define TTA_MAP_TOOLS_HH

#include <set>

#include "Exception.hh"

class MapTools {
public:
    template <typename MapType, typename ValueType>
    static bool containsValue(const MapType& aMap, const ValueType& aValue);

    template <typename MapType, typename KeyType>
    static bool containsKey(const MapType& aMap, const KeyType& aKey);

    template <typename KeyType, typename MapType, typename ValueType>
    static KeyType keyForValue(const MapType& aMap, const ValueType& aValue)
        throw (KeyNotFound);

    template <typename ValueType, typename MapType, typename KeyType>
    static ValueType valueForKey(const MapType& aMap, const KeyType& aKey)
        throw (KeyNotFound);

    template <typename MapType>
    static void deleteAllValues(MapType& aMap);

    template <typename MapType>
    static void deleteAllKeys(MapType& aMap);
    
    template <typename MapType, typename KeyType>
    static void deleteByKey(MapType& aMap, const KeyType& aKey);

    template <typename MapType, typename ValueType>
    static bool removeItemsByValue(MapType& aMap, const ValueType& aValue);

    template <typename KeyType, typename MapType>
    static std::set<KeyType> keys(const MapType& aMap);
};

#include "MapTools.icc"

#endif
