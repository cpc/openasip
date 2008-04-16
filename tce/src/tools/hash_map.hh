/**
 * @file hash_map.hh
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 *
 * Definition for a hash map.
 *
 * Hash map provides amortized constant time access for situations in which
 * the keys need not to be stored in sorted order. hash_map is not provided
 * by STL. It's usually provided as an STL extension. For example, SGI STL
 * implements it. This header selects the best possible implementation
 * for hash_map available in the system.
 */

#include "tce_config.h"

#ifdef UNORDERED_MAP

#include <unordered_map>
#define hash_map unordered_map

#elif HASHMAP_GNU_EXT

#include <ext/hash_map>

using __gnu_cxx::hash_map;

#elif HASHMAP_STD

#include <hash_map>

using std::hash_map;

#else

#warning Pretending std::map to be hash_map.

#define hash_map std::map

#endif

/// @todo Check for STLport hash_map
/// @todo Check for Intel's compiler. It seems Intel's compiler provides
/// compatibility for GNU so HASHMAP_GNU_EXT gets defined.
