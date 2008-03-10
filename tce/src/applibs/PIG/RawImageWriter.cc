/**
 * @file RawImageWriter.cc
 *
 * Implementation of RawImageWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
            
    
