/**
 * @file TPEFResourceSectionWriter.hh
 *
 * Declaration of TPEFResourceSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_RESOURCE_SECTION_WRITER_HH
#define TTA_TPEF_RESOURCE_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Writes TPEF Processor Resource Table section  to stream.
 */
class TPEFResourceSectionWriter : public TPEFSectionWriter {
protected:

    TPEFResourceSectionWriter();
    virtual ~TPEFResourceSectionWriter();

    virtual Section::SectionType type() const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* section) const;

    virtual Word elementSize(const Section* section) const;

private:
    /// Copying not allowed.
    TPEFResourceSectionWriter(const TPEFResourceSectionWriter&);
    /// Assignment not allowed.
    TPEFResourceSectionWriter operator=(const TPEFResourceSectionWriter&);

    /// An unique instance of class.
    static const TPEFResourceSectionWriter instance_;
    /// The fixed size element.
    static const Word elementSize_;
};
}

#endif
