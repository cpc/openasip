/**
 * @file TPEFDebugSectionReader.hh
 *
 * Declaration of TPEFDebugSectionReader.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_DEBUG_SECTION_READER_HH
#define TTA_TPEF_DEBUG_SECTION_READER_HH

#include "TPEFSectionReader.hh"
#include "Section.hh"
#include "BinaryReader.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Reads debug section from TPEF binary file.
 */
class TPEFDebugSectionReader : public TPEFSectionReader {
protected:
    TPEFDebugSectionReader();
    virtual ~TPEFDebugSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

private:
    /// Copying not allowed.
    TPEFDebugSectionReader(const TPEFDebugSectionReader&);
    /// Assignment not allowed.
    TPEFDebugSectionReader& operator=(TPEFDebugSectionReader&);

    /// Prototype instance of TPEFDebugSectionReader to be registered to
    /// SectionReader.
    static TPEFDebugSectionReader proto_;
};
}

#endif
