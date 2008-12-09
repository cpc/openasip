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
 * @file RawImageWriter.cc
 *
 * Implementation of RawImageWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "RawImageWriter.hh"
#include "BitVector.hh"

/**
 * The constructor.
 *
 * @param bits The bits to be written.
 */
RawImageWriter::RawImageWriter(const BitVector& bits) : bits_(bits) {
}


/**
 * The destructor.
 */
RawImageWriter::~RawImageWriter() {
}


/**
 * Writes the bit image to the given output stream.
 *
 * @param stream The output stream.
 */
void
RawImageWriter::writeImage(std::ostream& stream) const {
    unsigned int size = bits_.size();
    for (unsigned int i = 0; i < size; i += 8) {
        stream << character(bits_, i);
    }
}


/**
 * Returns the byte of the given bit vector that starts at the given index.
 *
 * @param bits The bit vector.
 * @param startIndex The start index.
 * @return The byte.
 */
char
RawImageWriter::character(const BitVector& bits, unsigned int startIndex) {
    unsigned int vectorSize = bits.size();
    char character = 0;
    if (bits[startIndex]) {
        character++;
    }
    for (int i = 1; i < 8; i++) {
        character = character << 1;
        if (startIndex + i < vectorSize && bits[startIndex + i]) {
            character++;
        }
    }
    return character;
}
            
    
