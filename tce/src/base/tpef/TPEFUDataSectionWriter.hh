/**
 * @file TPEFUDataSectionWriter.hh
 *
 * Declaration of TPEFUDataSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 18.12.2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_UDATA_SECTION_WRITER_HH
#define TTA_TPEF_UDATA_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "Section.hh"

namespace TPEF {

/**
 * Writes unitialized data section in to stream.
 *
 * Actually implements only needed fall through methods
 * for setting element size and section size to section header and
 * for creating section offset keys for referred data chunks.
 */
class TPEFUDataSectionWriter : public TPEFSectionWriter {
protected:
    TPEFUDataSectionWriter();
    virtual ~TPEFUDataSectionWriter();

    virtual Section::SectionType type() const;

    virtual void createKeys(const Section* sect) const;

    virtual Word elementSize(const Section* section) const;

    virtual void writeSize(BinaryStream& stream, const Section* sect) const;

private:
    /// Copying not allowed.
    TPEFUDataSectionWriter(const TPEFUDataSectionWriter&);
    /// Assignment not allowed.
    TPEFUDataSectionWriter operator=(const TPEFUDataSectionWriter&);

    /// Unique instance of a class.
    static const TPEFUDataSectionWriter instance_;
};
}

#endif
