/**
 * @file TPEFCodeSectionWriter.hh
 *
 * Declaration of TPEFCodeSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 18.12.2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_CODE_SECTION_WRITER_HH
#define TTA_TPEF_CODE_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"
#include "TPEFHeaders.hh"

namespace TPEF {

/**
 * Writes TPEF code section to file.
 */
class TPEFCodeSectionWriter : public TPEFSectionWriter {
protected:
    virtual ~TPEFCodeSectionWriter();
    TPEFCodeSectionWriter();

    virtual Section::SectionType type() const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* sect) const;

    virtual void writeInfo(BinaryStream& stream, const Section* sect) const;

private:
    /// Copying not allowed.
    TPEFCodeSectionWriter(const TPEFCodeSectionWriter&);
    /// Assignment not allowed.
    TPEFCodeSectionWriter& operator=(const TPEFCodeSectionWriter&);

    void writeAttributeField(
        BinaryStream& stream,
        SectionElement* elem,
        bool isEnd) const;

    void writeDataField(BinaryStream& stream, SectionElement* elem) const;

    void writeAnnotations(BinaryStream& stream, SectionElement* elem) const;

    /// Unique instance of class.
    static const TPEFCodeSectionWriter instance_;
    /// Maximum number of bytes that single immediate can contain.
    static const Byte IMMEDIATE_VALUE_MAX_BYTES;
};
}

#endif
