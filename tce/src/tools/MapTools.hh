/**
 * @file MapTools.hh
 *
 * @author Pekka J‰‰skel‰inen (pekka.jaaskelainen@tut.fi) 2004
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
