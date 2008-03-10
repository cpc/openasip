/**
 * @file BinaryWriter.hh
 *
 * Declaration of BinaryWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pjaaskel@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_BINARY_WRITER_HH
#define TTA_BINARY_WRITER_HH

#include "Binary.hh"
#include "Section.hh"
#include "BinaryStream.hh"

namespace TPEF {

/**
 * Interface for actual binary writer classes and for clients
 * to write binary object representation.
 *
 * Provides a way for calling the concrete writers with better error checking.
 */
class BinaryWriter {
public:
    virtual ~BinaryWriter();

    void writeBinary(BinaryStream& stream, const Binary* bin) const;

protected:
    BinaryWriter();

    /// Does real writing of binary.
    virtual void actualWriteBinary(
        BinaryStream& stream,
        const Binary* bin) const = 0;

    void setWriter(const BinaryWriter* writer);

private:
    /// Contains concrete writer instance that is used for writing binary.
    const BinaryWriter* writerToUse_;
};
}

#endif
