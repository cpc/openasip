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
    const BitVector& vector, unsigned int firstIndex, unsigned int lastIndex) {
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
BitVector::pushBack(long long unsigned int integer, int size) {
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
/**
 * Pushes back the given bit.
 *
 * @param bit The bit to be added.
 */
void
BitVector::pushBack(bool bit) {
    push_back(bit);
}

/**
 * Converts BitVector to std::string format
 * 
 */

std::string
BitVector::toString() const {
    std::string bits;
    for(int i = 0; i < this->size(); i++) {
        bits.append(std::to_string(this->at(i)));
    }
    return bits;
}
