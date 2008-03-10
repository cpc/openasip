/**
 * @file TPEFWriter.hh
 *
 * Declaration of TPEFWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pjaaskel@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_WRITER_HH
#define TTA_TPEF_WRITER_HH

#include "BinaryWriter.hh"

#include "Binary.hh"
#include "Section.hh"
#include "BinaryStream.hh"
#include "TPEFHeaders.hh"

namespace TPEF {

/**
 * Class for writing TPEF binary files.
 */
class TPEFWriter : public BinaryWriter {
public:
    static const BinaryWriter& instance();

protected:
    TPEFWriter();
    ~TPEFWriter();

    virtual void actualWriteBinary(
        BinaryStream& stream,
        const Binary* bin) const;
    
private:
    /// Instance which is used for writing binary file.
    static BinaryWriter* instance_;
};
}

#endif
