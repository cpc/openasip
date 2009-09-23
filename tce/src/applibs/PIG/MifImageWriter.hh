/**
 * @file MifImageWriter.hh
 *
 * Declaration of MifImageWriter class
 *
 * @author Otto Esko 2009 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_MIF_IMAGE_WRITER_HH
#define TTA_MIF_IMAGE_WRITER_HH

#include "AsciiImageWriter.hh"

class BitVector;

class MifImageWriter : public AsciiImageWriter {
public:
    MifImageWriter(const BitVector& bits, int rowLength);
    virtual ~MifImageWriter();

    virtual void writeImage(std::ostream& stream) const;

private:
    void writeHeader(std::ostream& stream) const;
};

#endif
