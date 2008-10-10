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
 * @file BitVector.cc
 *
 * Implementation of BitVector class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "BitVector.hh"
#include "MathTools.hh"
#include "Application.hh"

using std::string;

/**
 * The constructor.
 */
BitVector::BitVector() {
}


/**
 * The constructor.
 *
 * Creates a bit vector that is a sub vector of the given vector.
 *
 * @param vector The bit vector.
 * @param firstIndex The index of the given vector that is to be
 *                   the first element in the created vector.
 * @param lastIndex The index of the given vector that is to be that last
 *                  element in the created vector.
 * @exception OutOfRange If the given indexes are too big or too small.
 */
BitVector::BitVector(
    const BitVector& vector,
    unsigned int firstIndex,
    unsigned int lastIndex)
    throw (OutOfRange) {

    if (lastIndex < firstIndex || lastIndex >= vector.size()) {
        const string procName = "BitVector::BitVector";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    BitVector::const_iterator firstIter = vector.begin();
    firstIter += firstIndex;
    BitVector::const_iterator lastIter = vector.begin();
    lastIter += lastIndex + 1;
    insert(begin(), firstIter, lastIter);
    assert(size() == lastIndex - firstIndex + 1);
}


/**
 * The destructor.
 */
BitVector::~BitVector() {
}


/**
 * Pushes back the given number and increases the size of the vector by the
 * given amount.
 *
 * For example, if number 6 (110) is added with size 5, bits 00110 are 
 * concatenated to the vector. If size 2 is given, then bits 10 are
 * concatenated to the vector.
 *
 * @param integer The number to be added.
 * @param size The number of bits to be added.
 */
void
BitVector::pushBack(unsigned int integer, int size) {
    for (int i = size - 1; i >= 0; i--) {
        push_back(MathTools::bit(integer, i));
    }
}
    

/**
 * Pushes back the given bit vector.
 *
 * @param bits The bit vector.
 */
void
BitVector::pushBack(const BitVector& bits) {
    reserve(size() + bits.size());
    for (std::vector<bool>::const_iterator iter = bits.begin();
         iter != bits.end(); iter++) {
        push_back(*iter);
    }
}
