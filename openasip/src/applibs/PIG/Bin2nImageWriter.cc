/*
  Copyright (c) 2002-2019 Tampere University of Technology.

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
 * @file Bin2nImageWriter.cc
 *
 * Implementation of Bin2nImageWriter class.
 *
 * @author Topi Leppanen 2019 (topi.leppanen-no.spam-tuni.fi)
 * @note rating: red
 */


#include <string>

#include "Bin2nImageWriter.hh"
#include "BitVector.hh"


Bin2nImageWriter::Bin2nImageWriter(const BitVector& bits,
                                   int rowLength) :
    bits_(bits), rowLength_(rowLength), nextBitIndex_(0) {
}

Bin2nImageWriter::~Bin2nImageWriter(){
}

/**
 * Writes the data bits to the given stream. Reverses the byte endianness
 *
 * @param stream The output stream.
 */
void
Bin2nImageWriter::writeImage(std::ostream& stream) const {

    unsigned int size = bits_.size();

    /// Bitvector-size has to always be exactly divisible by rowlength
    unsigned int numOfRows = size / rowLength_;

    for (unsigned int i = 0; i < numOfRows; i++){
        writeSequence(stream, rowLength_);

    }
}

/**
 * Returns the byte of the given bit vector that starts at the given index
 * and is 'length' bits long. Pads from the left to size 8.
 *
 * @param bits The bit vector.
 * @param startIndex The start index.
 * @param length How many indexes forward is read. 0-8.
 * @return The byte.
 */
char
Bin2nImageWriter::character(const BitVector& bits, unsigned int startIndex, int length) {
    unsigned int vectorSize = bits.size();
    char character = 0;

    /// Pad from the left with zeros.
    character = character << (8 - length);

    /// Read the first bit.
    if (bits[startIndex]  && length > 0){
        character++;
    }

    /// Read the rest of the bits.
    for (int i = 1; i < length; i++) {
        character = character << 1;
        if (startIndex + i < vectorSize && bits[startIndex + i]) {
            character++;
        }
    }
    return character;
}


/**
 * Writes a sequence of bits to the given stream.
 *
 * When this method is called sequentially, the first bit to be written is
 * the next bit to the last bit written in the previous method call.
 * Pads the sequence from the left with zeroes up to the nearest multiple of 2.
 * After that, reverses the string before writing it to the stream.
 *
 * @param stream The output stream.
 * @param length The length of the sequence to be written.
 */
void
Bin2nImageWriter::writeSequence(std::ostream& stream, unsigned int length) const{

    if (length == 0){
        return;
    }

    unsigned int lastIndex = nextBitIndex_ + length -1;
    unsigned int imem_width = nextPowerOf2(length);
    std::string sequence_word = "";

    /// Pad from the left the correct amount of "0 bytes".
    for (unsigned int i = 0;i < (imem_width-length)/8; i++){
        char zero = 0;
        sequence_word.push_back(zero);
    }

    /// There might be a partial byte, which needs to be padded from the left
    unsigned int partial_start_index = nextBitIndex_;
    unsigned int partial_char_len = length % 8;

    /// If there is no partial byte, we don't want to insert anything
    if (partial_char_len != 0){
        sequence_word.push_back(character(bits_, partial_start_index, partial_char_len));
        partial_start_index += partial_char_len;
    }

    /// Create the full bytes.
    for (unsigned int index = partial_start_index; index <= lastIndex; index +=8){

        sequence_word.push_back(character(bits_, index, 8));

    }

    /// Update this for the next iteration of the function.
    nextBitIndex_ = lastIndex + 1;

    /// Reverse the byte endianness and write the stream.
    for (std::string::reverse_iterator iter = sequence_word.rbegin();
         iter != sequence_word.rend(); iter++){

        stream << *iter;
    }
}

const BitVector&
Bin2nImageWriter::bits() const {
    return bits_;
}

/**
 * Returns the next power of 2 of the given parameter
 *
 * @param n
 * @return p the next power of 2 of n
 */
unsigned int
Bin2nImageWriter::nextPowerOf2(unsigned int n) const{

    unsigned int p = 1;

    if (!(n & (n - 1)))
        return n;

    while(p < n)  {
        p <<= 1;
    }

    return p;
}
