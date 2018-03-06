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
 * @file AsciiImageWriter.cc
 *
 * Implementation of AsciiImageWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <iostream>

#include "AsciiImageWriter.hh"
#include "BitVector.hh"

using namespace std;
using std::string;

/**
 * The constructor.
 *
 * @param bits The bits to be written.
 * @param rowLength The length of the row in the output stream.
 */
AsciiImageWriter::AsciiImageWriter(const BitVector& bits, int rowLength) :
    bits_(bits), rowLength_(rowLength), nextBitIndex_(0) {
}


/**
 * The destructor.
 */
AsciiImageWriter::~AsciiImageWriter() {
}


/**
 * Writes the bits to the given stream.
 *
 * @param stream The output stream.
 */
void
AsciiImageWriter::writeImage(std::ostream& stream) const {
    int column = 0;
    for (BitVector::const_iterator iter = bits_.begin(); iter != bits_.end();
         iter++) {
        BitVector::const_iterator nextIter = iter;
        nextIter++;
        stream << *iter;
        column++;
        if (column == rowLength_ && nextIter != bits_.end()) {
            stream << '\n';
            column = 0;
        } else if (nextIter == bits_.end()) {
            // pad the remaining bits with zeroes if necessary
            while (column < rowLength_) {
                stream << "0";
                column++;
            }
        }
    }
}


/**
 * Returns the bit vector to be written by this writer.
 *
 * @return The bit vector.
 */
const BitVector&
AsciiImageWriter::bits() const {
    return bits_;
}


/**
 * Returns the row length.
 *
 * @return The row length.
 */
int
AsciiImageWriter::rowLength() const {
    return rowLength_;
}


/**
 * Writes a sequence of bits to the given stream.
 *
 * When this method is called sequentially, the first bit to be written is
 * the next bit to the last bit written in the previous method call.
 *
 * @param stream The output stream.
 * @param length The length of the sequence to be written.
 * @exception OutOfRange If the bit vector does not contain enough bits for
 *                       the row.
 */
void
AsciiImageWriter::writeSequence(
    std::ostream& stream, int length, bool padEnd) const
    throw (OutOfRange) {
    unsigned int lastIndex = nextBitIndex_ + length - 1;

    if (lastIndex >= bits_.size() && !padEnd) {
        const string procName = "AsciiImageWriter::writeSequence";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    for (unsigned int index = nextBitIndex_; index <= lastIndex; index++) {
        if (index < bits_.size()) {
            stream << bits_[index];
        } else {
            stream << "0";
        }
    }
    nextBitIndex_ = lastIndex + 1;
}
        

/**
 * Calculate the next nibble in HEX
 */
uint8_t
AsciiImageWriter::calcNibble(const BitVector & bits, int lsb, int num) const {
    uint8_t nibble = 0;

    for(int i=0; i<num; i++) {
        if (bits[i+lsb]) {
            nibble += (1 << i);
        }
    }

    return nibble;
}


/**
 * Generate a vector of hex values (nibbles) out of a vector of bits
 */
/*
std::vector<uint8_t>
AsciiImageWriter::bool2nibble(const BitVector & bits) const {
    std::vector<uint8_t> nibbles;

    const int end = bits.size();

    for (int i=0; i<end; i+=4) {
        int num = ((end-i) > 4) ? 4 : (end-i);
        nibbles.push_back( calcNibble(bits, i, num) );
    }

    return nibbles;
}
*/

/**
 * Writes a sequence of bits in hex format to the given stream.
 *
 * When this method is called sequentially, the first bit to be written is
 * the next bit to the last bit written in the previous method call.
 *
 * @param stream The output stream.
 * @param length The length of the sequence to be written.
 * @exception OutOfRange If the bit vector does not contain enough bits for
 *                       the row.
 */
void
AsciiImageWriter::writeHexSequence(
    std::ostream& stream, int length, bool padEnd) const
    throw (OutOfRange) {
    int lastIndex = nextBitIndex_ + length - 1;

    if (lastIndex >= bits_.size() && !padEnd) {
        const string procName = "AsciiImageWriter::writeHexSequence";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

/*
    std::vector<bool> subbits(length, 0);
    std::copy(std::begin(bits_)+nextBitIndex_, std::begin(bits_)+lastIndex, std::begin(subbits));

    std::vector<uint8_t> N = bool2nibble(subbits);
    cout << "size(N) = " << N.size() << endl;
*/
    std::vector<uint8_t> nibbles;
    for (int i = nextBitIndex_; i <= lastIndex; i+=4) {
        int num = ((length-i) > 4) ? 4 : (length-i);
        nibbles.push_back( calcNibble(bits_, i, num) );
    }

    for (std::vector<uint8_t>::reverse_iterator rit = nibbles.rbegin(); rit!= nibbles.rend(); ++rit) {
        cout << std::to_string(*rit);
    }
    cout << endl;

    nextBitIndex_ += length;
}

