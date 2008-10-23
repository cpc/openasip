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
 * @file hash_set.hh
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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

#include <tr1/unordered_set>

using std::tr1::unordered_set;
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
