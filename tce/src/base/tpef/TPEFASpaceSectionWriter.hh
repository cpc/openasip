/**
 * @file TPEFASpaceSectionWriter.hh
 *
 * Declaration of TPEFASpaceSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_ASPACE_SECTION_WRITER_HH
#define TTA_TPEF_ASPACE_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Writes TPEF address space section to stream.
 */
class TPEFASpaceSectionWriter : public TPEFSectionWriter {
protected:
    TPEFASpaceSectionWriter();
    virtual ~TPEFASpaceSectionWriter();

    virtual Section::SectionType type() const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* section) const;

    virtual Word elementSize(const Section* section) const;

private:
    /// Copying not allowed.
    TPEFASpaceSectionWriter(const TPEFASpaceSectionWriter&);
    /// Assignment not allowed.
    TPEFASpaceSectionWriter operator=(const TPEFASpaceSectionWriter&);

    void writeElement(
        BinaryStream &stream,
        const ASpaceElement* elem,
        SectionId id,
        Word index) const;

    /// An unique instance of class.
    static const TPEFASpaceSectionWriter instance_;
    /// The fixed size address space element.
    static const Word elementSize_;
};
}

#endif
