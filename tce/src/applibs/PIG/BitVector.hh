/**
 * @file BitVector.hh
 *
 * Declaration of BitVector class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BIT_VECTOR_HH
#define TTA_BIT_VECTOR_HH

#include <vector>
#include "Exception.hh"

/**
 * BitVector is a specialization of STL's bit vector. It provides some
 * handy helper methods.
 */
class BitVector : public std::vector<bool> {
public:
    BitVector();
    BitVector(
        const BitVector& vector,
        unsigned int firstIndex,
        unsigned int lastIndex)
        throw (OutOfRange);
    virtual ~BitVector();

    void pushBack(unsigned int integer, int size);
    void pushBack(const BitVector& bits);
};

#endif
