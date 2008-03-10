/**
 * @file TPEFNullSectionReader.hh
 *
 * Declaration of TPEFNullSectionReader.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_NULL_SECTION_READER_HH
#define TTA_TPEF_NULL_SECTION_READER_HH

#include "TPEFSectionReader.hh"
#include "BinaryReader.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"
#include "Section.hh"

namespace TPEF {

/**
 * Reads null section section from TPEF binary file.
 */
class TPEFNullSectionReader : public TPEFSectionReader {
protected:
    TPEFNullSectionReader();
    virtual ~TPEFNullSectionReader();
    virtual Section::SectionType type() const;

private:
    /// Copying not allowed.
    TPEFNullSectionReader(const TPEFNullSectionReader&);
    /// Assignment not allowed.
    TPEFNullSectionReader& operator=(TPEFNullSectionReader&);

    /// Prototype instance of TPEFNullSectionReader to be registered to
    /// SectionReader.
    static TPEFNullSectionReader proto_;
};
}

#endif
