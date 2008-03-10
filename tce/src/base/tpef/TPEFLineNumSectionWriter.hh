/**
 * @file TPEFLineNumSectionWriter.hh
 *
 * Declaration of TPEFLineNumSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_LINE_NUM_SECTION_WRITER_HH
#define TTA_TPEF_LINE_NUM_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Writes TPEF line number section to stream.
 */
class TPEFLineNumSectionWriter : public TPEFSectionWriter {
protected:
    TPEFLineNumSectionWriter();
    virtual ~TPEFLineNumSectionWriter();

    virtual Section::SectionType type() const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* section) const;

    virtual Word elementSize(const Section* section) const;

    virtual void writeInfo(BinaryStream& stream, const Section* sect) const;

private:
    /// Copying not allowed.
    TPEFLineNumSectionWriter(const TPEFLineNumSectionWriter&);
    /// Assignment not allowed.
    TPEFLineNumSectionWriter operator=(const TPEFLineNumSectionWriter&);

    /// An unique instance of class.
    static const TPEFLineNumSectionWriter instance_;
    /// The fixed size address space element.
    static const Word elementSize_;
};
}

#endif
