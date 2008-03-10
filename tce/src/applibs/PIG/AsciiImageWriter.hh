/**
 * @file AsciiImageWriter.hh
 *
 * Declaration of AsciiBinaryWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ASCII_IMAGE_WRITER_HH
#define TTA_ASCII_IMAGE_WRITER_HH

#include "BitImageWriter.hh"
#include "Exception.hh"

class BitVector;

/**
 * Writes the bit image with ASCII 1's and 0's.
 */
class AsciiImageWriter : public BitImageWriter {
public:
    AsciiImageWriter(const BitVector& bits, int rowLength);
    virtual ~AsciiImageWriter();

    virtual void writeImage(std::ostream& stream) const;

protected:
    const BitVector& bits() const;
    int rowLength() const;
    void writeSequence(std::ostream& stream, int length) const
        throw (OutOfRange);

private:
    /// The bits to be written.
    const BitVector& bits_;
    /// The length of a row in the output.
    int rowLength_;
    /// The index of the next bit to be written.
    mutable unsigned int nextBitIndex_;
};

#endif
