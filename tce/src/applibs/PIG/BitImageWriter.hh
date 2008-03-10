/**
 * @file BitImageWriter.hh
 *
 * Declaration of BitImageWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BIT_IMAGE_WRITER_HH
#define TTA_BIT_IMAGE_WRITER_HH

#include <ostream>

/**
 * Interface that is implemented by different bit image writers.
 */
class BitImageWriter {
public:
    /**
     * Writes the bit image to the given output stream.
     *
     * @param stream The output stream.
     */
    virtual void writeImage(std::ostream& stream) const = 0;

    virtual ~BitImageWriter() {}
};

#endif
