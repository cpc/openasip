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
            
    
