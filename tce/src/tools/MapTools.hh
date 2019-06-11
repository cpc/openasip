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

    template <typename ValueType, typename MapType, typename KeyType>
    static const ValueType valueForKeyNoThrow(const MapType& aMap, const KeyType& aKey)
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
