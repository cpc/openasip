/**
 * @file TPEFDataSectionWriter.hh
 *
 * Declaration of TPEFDataSectionWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_DATA_SECTION_WRITER_HH
#define TTA_TPEF_DATA_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"

#include "BinaryStream.hh"
#include "Section.hh"
#include "Exception.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Writes data section to TPEF stream.
 */
class TPEFDataSectionWriter : public TPEFSectionWriter {
protected:
    virtual ~TPEFDataSectionWriter();
    TPEFDataSectionWriter();

    virtual Section::SectionType type() const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* sect) const;

    virtual Word elementSize(const Section* section) const;

private:
    /// No copies.
    TPEFDataSectionWriter(const TPEFDataSectionWriter&);
    /// No copies.
    TPEFDataSectionWriter operator=(const TPEFDataSectionWriter&);

    /// Only existing instance of class.
    static const TPEFDataSectionWriter instance_;
};
}

#endif
