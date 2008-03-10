/**
 * @file TPEFASpaceSectionReader.hh
 *
 * Declaration of TPEFASpaceSectionReader.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_ASPACE_SECTION_READER_HH
#define TTA_TPEF_ASPACE_SECTION_READER_HH

#include "TPEFSectionReader.hh"
#include "Section.hh"
#include "BinaryReader.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Reads address space section from TPEF binary file.
 */
class TPEFASpaceSectionReader : public TPEFSectionReader {
protected:
    TPEFASpaceSectionReader();
    virtual ~TPEFASpaceSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

private:
    /// Copying not allowed.
    TPEFASpaceSectionReader(const TPEFASpaceSectionReader&);
    /// Assignment not allowed.
    TPEFASpaceSectionReader& operator=(TPEFASpaceSectionReader&);

    /// Prototype instance of TPEFASpaceSectionReader to be registered to
    /// SectionReader.
    static TPEFASpaceSectionReader proto_;
};
}

#endif
