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
        
