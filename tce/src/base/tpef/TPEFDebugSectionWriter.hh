/**
 * @file TPEFDebugSectionWriter.hh
 *
 * Declaration of TPEFDebugSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_DEBUG_SECTION_WRITER_HH
#define TTA_TPEF_DEBUG_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Writes TPEF symbol table to stream.
 */
class TPEFDebugSectionWriter : public TPEFSectionWriter {
protected:
    TPEFDebugSectionWriter();
    virtual ~TPEFDebugSectionWriter();

    virtual Section::SectionType type() const;

    virtual void createKeys(const Section* sect) const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* section) const;

    virtual Word elementSize(const Section* section) const;

private:
    /// Copying not allowed.
    TPEFDebugSectionWriter(const TPEFDebugSectionWriter&);
    /// Assignment not allowed.
    TPEFDebugSectionWriter operator=(const TPEFDebugSectionWriter&);

    /// A unique instance of this class.
    static const TPEFDebugSectionWriter instance_;
};

}

#endif
