/**
 * @file TPEFUDataSectionReader.hh
 *
 * Declaration of TPEFUDataSectionReader.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_UDATA_SECTION_READER_HH
#define TTA_TPEF_UDATA_SECTION_READER_HH

#include "TPEFSectionReader.hh"
#include "BinaryReader.hh"
#include "ReferenceKey.hh"
#include "BaseType.hh"
#include "Section.hh"

namespace TPEF {

/**
 * Reads uninitialized data section from TPEF binary file.
 */
class TPEFUDataSectionReader : public TPEFSectionReader {
protected:
    TPEFUDataSectionReader();
    virtual ~TPEFUDataSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

private:
    /// Prototype instance of TPEFUDataSectionReader to be registered to
    /// SectionReader.
    static TPEFUDataSectionReader proto_;

    /// Copying not allowed.
    TPEFUDataSectionReader(const TPEFUDataSectionReader&);
    /// Assignment not allowed.
    TPEFUDataSectionReader& operator=(TPEFUDataSectionReader&);

};
}

#endif
