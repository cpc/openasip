/**
 * @file BitVector.cc
 *
 * Implementation of BitVector class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
