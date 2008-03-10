/**
 * @file RawImageWriter.hh
 *
 * Declaration of RawImageWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_RAW_IMAGE_WRITER_HH
#define TTA_RAW_IMAGE_WRITER_HH

#include "BitImageWriter.hh"

class BitVector;

/**
 * Bit image writer that writes a real binary image.
 */
class RawImageWriter : public BitImageWriter {
public:
    RawImageWriter(const BitVector& bits);
    virtual ~RawImageWriter();

    virtual void writeImage(std::ostream& stream) const;

private:
    static char character(const BitVector& bits, unsigned int startIndex);

    /// The bits to be written.
    const BitVector& bits_;
};

#endif
