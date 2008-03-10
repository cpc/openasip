/**
 * @file ArrayImageWriter.cc
 *
 * Implementation of ArrayImageWriter class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "ArrayImageWriter.hh"
#include "BitVector.hh"

/**
 * The constructor.
 *
 * @param bits The bits to write.
 * @param rowLength Length of the rows to write.
 */
ArrayImageWriter::ArrayImageWriter(const BitVector& bits, int rowLength) :
    AsciiImageWriter(bits, rowLength) {
}


/**
 * The destructor.
 */
ArrayImageWriter::~ArrayImageWriter() {
}


/**
 * Writes the bit image to the given stream.
 *
 * @param stream The stream to write.
 */
void
ArrayImageWriter::writeImage(std::ostream& stream) const {
    int lineCount = bits().size() / rowLength();
    for (int line = 1; line < lineCount; line++) {
        stream << "\"";
        writeSequence(stream, rowLength());
        stream << "\"," << std::endl;
    }
    stream << "\"";
    writeSequence(stream, rowLength());
    stream << "\"";
}
