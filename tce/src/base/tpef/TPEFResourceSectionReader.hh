/**
 * @file TPEFResourceSectionReader.hh
 *
 * Declaration of TPEFResourceSectionReader.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_RESOURCE_SECTION_READER_HH
#define TTA_TPEF_RESOURCE_SECTION_READER_HH

#include "TPEFSectionReader.hh"
#include "Section.hh"
#include "BinaryReader.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Reads Processor Resource Table section from TPEF binary file.
 */
class TPEFResourceSectionReader : public TPEFSectionReader {
protected:
    TPEFResourceSectionReader();
    virtual ~TPEFResourceSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

private:
    /// Copying not allowed.
    TPEFResourceSectionReader(const TPEFResourceSectionReader&);
    /// Assignment not allowed.
    TPEFResourceSectionReader& operator=(TPEFResourceSectionReader&);

    /// Prototype instance to be registered.
    static TPEFResourceSectionReader proto_;
};
}

#endif
