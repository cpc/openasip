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
