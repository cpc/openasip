/**
 * @file TPEFCodeSectionReader.hh
 *
 * Declaration of TPEFCodeSectionReader.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_CODE_SECTION_READER_HH
#define TTA_TPEF_CODE_SECTION_READER_HH

#include "TPEFSectionReader.hh"
#include "BinaryReader.hh"
#include "ReferenceKey.hh"
#include "BaseType.hh"
#include "Section.hh"
#include "InstructionElement.hh"

namespace TPEF {

/**
 * Reads code section from TPEF binary file.
 */
class TPEFCodeSectionReader : public TPEFSectionReader {
protected:
    TPEFCodeSectionReader();
    virtual ~TPEFCodeSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

    void readInfo(BinaryStream& stream, Section* sect) const;

private:
    /// Copying not allowed.
    TPEFCodeSectionReader(const TPEFCodeSectionReader&);
    /// Assignment not allowed.
    TPEFCodeSectionReader& operator=(TPEFCodeSectionReader&);

    void readAnnotations(
        BinaryStream& stream, InstructionElement* elem) const;

    /// Prototype instance of TPEFCodeSectionReader to be registered to
    /// SectionReader.
    static TPEFCodeSectionReader proto_;
};
}

#endif
