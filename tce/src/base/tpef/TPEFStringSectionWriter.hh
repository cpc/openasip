/**
 * @file TPEFStringSectionWriter.hh
 *
 * Declaration of TPEFStringSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 18.12.2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_STRING_SECTION_WRITER_HH
#define TTA_TPEF_STRING_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Writes the contents of TPEF string section to stream.
 */
class TPEFStringSectionWriter : public TPEFSectionWriter {
protected:
    TPEFStringSectionWriter();
    virtual ~TPEFStringSectionWriter();

    virtual Section::SectionType type() const;
    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* sect) const;

    virtual Word elementSize(const Section* section) const;

private:
    /// Copying not allowed.
    TPEFStringSectionWriter(const TPEFStringSectionWriter&);
    /// Assignment not allowed.
    TPEFStringSectionWriter& operator=(const TPEFStringSectionWriter&);

    /// Unique instance of class.
    static const TPEFStringSectionWriter instance_;
};
}

#endif
