/**
 * @file hash_set.hh
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 *
 * Definition for a hash set.
 *
 * Hash set provides amortized constant time access for situations in which
 * the keys need not to be stored in sorted order. hash_set is not provided
 * by STL. It's usually provided as an STL extension. For example, SGI STL
 * implements it. This header selects the best possible implementation
 * for hash_set available in the system.
 */

#include "tce_config.h"

// if unordered_map is available, should also be unordered_set
#ifdef UNORDERED_MAP

#include <unordered_set>
#define hash_set unordered_set

// if hash_map is available, should also be hash_set
#elif HASHMAP_GNU_EXT

#include <ext/hash_set>

using __gnu_cxx::hash_set;

#elif HASHMAP_STD

#include <hash_set>

using std::hash_set;

#else

#warning Pretending std::set to be hash_set.

#define hash_set std::set

#endif

/// @todo Check for STLport hash_map
/// @todo Check for Intel's compiler. It seems Intel's compiler provides
/// compatibility for GNU so HASHMAP_GNU_EXT gets defined.
