/**
 * @file TPEFSectionWriter.hh
 *
 * Declaration of TPEFSectionWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_SECTION_WRITER_HH
#define TTA_TPEF_SECTION_WRITER_HH

#include "SectionWriter.hh"

#include "BinaryWriter.hh"
#include "Section.hh"
#include "Exception.hh"
#include "TPEFBaseType.hh"
#include "TPEFHeaders.hh"

namespace TPEF {

/**
 * Base class for all TPEF filetype writer classes.
 */
class TPEFSectionWriter : public SectionWriter {
protected:
    TPEFSectionWriter();
    virtual ~TPEFSectionWriter();

    virtual const BinaryWriter& parent() const;

    virtual void actualWriteHeader(
        BinaryStream& stream,
        const Section* sect) const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* sect) const;

    /// Returns SectionTypet hat actual reader or writer instance can handle.
    virtual Section::SectionType type() const = 0;

    virtual void createKeys(const Section* sect) const;

    virtual void writeSize(BinaryStream& stream, const Section* sect) const;

    virtual void writeInfo(BinaryStream& stream, const Section* sect) const;

    virtual SectionId getSectionId() const;

    virtual Word elementSize(const Section* section) const;

private:
    TPEFSectionWriter(const TPEFSectionWriter&);

    void writeBodyStartOffset(
        BinaryStream& stream,
        const Section* sect) const;
};
}

#endif
