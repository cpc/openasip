/**
 * @file TPEFRelocSectionWriter.hh
 *
 * Declaration of TPEFRelocSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 10.12.2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_RELOC_SECTION_WRITER_HH
#define TTA_TPEF_RELOC_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "TPEFBaseType.hh"
#include "Section.hh"

namespace TPEF {

/**
 * Writes TPEF relocation section to stream.
 */
class TPEFRelocSectionWriter : public TPEFSectionWriter {
protected:
    TPEFRelocSectionWriter();
    virtual ~TPEFRelocSectionWriter();

    virtual Section::SectionType type() const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* section) const;

    virtual Word elementSize(const Section* section) const;

    virtual void writeInfo(BinaryStream& stream, const Section* sect) const;

private:
    /// Copying not allowed.
    TPEFRelocSectionWriter(const TPEFRelocSectionWriter&);
    /// Assignment not allowed.
    TPEFRelocSectionWriter operator=(const TPEFRelocSectionWriter&);

    /// An unique instance of class.
    static const TPEFRelocSectionWriter instance_;
    /// The fixed size of reloc element.
    static const Word elementSize_;
};
}

#endif
