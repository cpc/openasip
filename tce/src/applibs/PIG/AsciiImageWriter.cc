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
 * @file AsciiImageWriter.cc
 *
 * Implementation of AsciiImageWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>

#include "AsciiImageWriter.hh"
#include "BitVector.hh"

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
AsciiImageWriter::writeSequence(std::ostream& stream, int length) const
    throw (OutOfRange) {

    unsigned int lastIndex = nextBitIndex_ + length - 1;

    if (lastIndex >= bits_.size()) {
        const string procName = "AsciiImageWriter::writeRow";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    for (unsigned int index = nextBitIndex_; index <= lastIndex; index++) {
        stream << bits_[index];
    }

    nextBitIndex_ = lastIndex + 1;
}
        
