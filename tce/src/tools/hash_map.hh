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

#if defined(STD_TR1_UNORDERED_MAP)

#include <tr1/unordered_map>

//using std::tr1::unordered_map;
#define hash_map std::tr1::unordered_map

#elif defined(STD_UNORDERED_MAP)

#include <unordered_map>
//using std::unordered_map;
#define hash_map std::unordered_map

#elif defined(HASHMAP_GNU_EXT)

#include <ext/hash_map>

using __gnu_cxx::hash_map;

#elif defined(HASHMAP_STD)

#include <hash_map>

using std::hash_map;

#else

#warning Pretending std::map to be hash_map.

#define hash_map std::map

#endif

/// @todo Check for STLport hash_map
/// @todo Check for Intel's compiler. It seems Intel's compiler provides
/// compatibility for GNU so HASHMAP_GNU_EXT gets defined.
