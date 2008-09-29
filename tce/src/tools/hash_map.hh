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
 * @file hash_map.hh
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
