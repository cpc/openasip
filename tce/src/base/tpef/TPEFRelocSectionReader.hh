/**
 * @file TPEFRelocSectionReader.hh
 *
 * Declaration of TPEFRelocSectionReader.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_RELOC_SECTION_READER_HH
#define TTA_TPEF_RELOC_SECTION_READER_HH

#include "TPEFSectionReader.hh"
#include "Section.hh"
#include "BinaryReader.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Reads relocation section from TPEF binary file.
 */
class TPEFRelocSectionReader : public TPEFSectionReader {
protected:
    TPEFRelocSectionReader();
    virtual ~TPEFRelocSectionReader();

    virtual void finalize(Section* section) const;

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

    void readInfo(BinaryStream& stream, Section* sect) const;

private:
    /// Copying not allowed.
    TPEFRelocSectionReader(const TPEFRelocSectionReader&);
    /// Assignment not allowed.
    TPEFRelocSectionReader& operator=(TPEFRelocSectionReader&);

    /// Prototype instance of TPEFRelocSectionReader to be registered to
    /// SectionReader.
    static TPEFRelocSectionReader proto_;
    
    /// Section id of last referenced section.
    static SectionId refSectionId_;
};
}

#endif
