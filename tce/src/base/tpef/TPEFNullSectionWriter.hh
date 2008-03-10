/**
 * @file TPEFNullSectionWriter.hh
 *
 * Declaration of TPEFNullSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_NULL_SECTION_WRITER_HH
#define TTA_TPEF_NULL_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "Section.hh"

namespace TPEF {

/**
 * Writes Null section in to stream.
 */
class TPEFNullSectionWriter : public TPEFSectionWriter {
protected:
    TPEFNullSectionWriter();
    virtual ~TPEFNullSectionWriter();

    virtual Section::SectionType type() const;

    virtual Word elementSize(const Section* section) const;

    virtual SectionId getSectionId() const;

    virtual void writeSize(BinaryStream& stream, const Section* sect) const;

private:
    /// Copying not allowed.
    TPEFNullSectionWriter(const TPEFNullSectionWriter&);
    /// Assignment not allowed.
    TPEFNullSectionWriter operator=(const TPEFNullSectionWriter&);

    /// Unique instance of a class.
    static const TPEFNullSectionWriter instance_;
};
}

#endif
