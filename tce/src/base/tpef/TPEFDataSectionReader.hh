/**
 * @file TPEFDataSectionReader.hh
 *
 * Declaration of TPEFDataSectionReader.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_DATA_SECTION_READER_HH
#define TTA_TPEF_DATA_SECTION_READER_HH

#include "TPEFSectionReader.hh"
#include "BinaryReader.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"
#include "Section.hh"

namespace TPEF {

/**
 * Reads string section from TPEF binary file.
 */
class TPEFDataSectionReader : public TPEFSectionReader {
protected:
    TPEFDataSectionReader();
    virtual ~TPEFDataSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

private:
    /// Copying not allowed.
    TPEFDataSectionReader(const TPEFDataSectionReader&);
    /// Assignment not allowed.
    TPEFDataSectionReader& operator=(TPEFDataSectionReader&);

    /// Prototype instance of TPEFDataSectionReader to be registered to
    /// SectionReader.
    static TPEFDataSectionReader proto_;
};
}

#endif
