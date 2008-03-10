/**
 * @file TPEFLineNumSectionReader.hh
 *
 * Declaration of TPEFLineNumSectionReader.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_LINE_NUM_SECTION_READER_HH
#define TTA_TPEF_LINE_NUM_SECTION_READER_HH

#include "TPEFSectionReader.hh"
#include "Section.hh"
#include "BinaryReader.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Reads line number section from TPEF binary file.
 */
class TPEFLineNumSectionReader : public TPEFSectionReader {
protected:
    TPEFLineNumSectionReader();
    virtual ~TPEFLineNumSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

    virtual void readInfo(BinaryStream& stream, Section* sect) const;

private:
    /// Prototype instance of TPEFLineNumSectionReader to be registered to
    /// SectionReader.
    static TPEFLineNumSectionReader proto_;

    /// Identification code of text section which is referenced from
    /// currently read section.
    static SectionId codeSectionId_;

    /// Copying not allowed.
    TPEFLineNumSectionReader(const TPEFLineNumSectionReader&);
    /// Assignment not allowed.
    TPEFLineNumSectionReader& operator=(TPEFLineNumSectionReader&);

};
}

#endif
