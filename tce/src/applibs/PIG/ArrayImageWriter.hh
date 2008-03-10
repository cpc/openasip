/**
 * @file ArrayImageWriter.hh
 *
 * Declaration of ArrayImageWriter class
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ARRAY_IMAGE_WRITER_HH
#define TTA_ARRAY_IMAGE_WRITER_HH

#include "AsciiImageWriter.hh"

/**
 * Writes the bit image with ASCII 1's and 0's such that each line is 
 * enveloped by " marks. Can be used to initialize an VHDL array, for
 * example.
 */
class ArrayImageWriter : public AsciiImageWriter {
public:
    ArrayImageWriter(const BitVector& bits, int rowLength);
    virtual ~ArrayImageWriter();

    virtual void writeImage(std::ostream& stream) const;    
};

#endif
