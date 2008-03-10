/**
 * @file TPEFStringSectionReader.hh
 *
 * Declaration of TPEFStringSectionReader.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_STRING_SECTION_READER_HH
#define TTA_TPEF_STRING_SECTION_READER_HH

#include "TPEFSectionReader.hh"
#include "BinaryReader.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"
#include "Section.hh"

namespace TPEF {

/**
 * Reads string section from TPEF binary file.
 */
class TPEFStringSectionReader : public TPEFSectionReader {
protected:
    TPEFStringSectionReader();
    virtual ~TPEFStringSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

private:
    /// Copying not allowed.
    TPEFStringSectionReader(const TPEFStringSectionReader&);
    /// Assignment not allowed.
    TPEFStringSectionReader& operator=(TPEFStringSectionReader&);

    /// Prototype instance of TPEFStringSectionReader to be registered to
    /// SectionReader.
    static TPEFStringSectionReader proto_;

};
}

#endif
