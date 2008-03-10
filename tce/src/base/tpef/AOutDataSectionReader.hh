/**
 * @file AOutDataSectionReader.hh
 *
 * Declaration of AOutDataSectionReader class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 * @note reviewed 7 October 2003 by jn, ml, tr, ll
 *
 * @note rating: yellow
 */

#ifndef TTA_AOUT_DATA_SECTION_READER_HH
#define TTA_AOUT_DATA_SECTION_READER_HH

#include "AOutSectionReader.hh"
#include "Section.hh"
#include "BinaryReader.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Reads data section from a.out binary file.
 */
class AOutDataSectionReader : public AOutSectionReader {
public:
    virtual ~AOutDataSectionReader();

protected:
    AOutDataSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

private:
    /// Copying not allowed.
    AOutDataSectionReader(const AOutDataSectionReader&);

    /// Prototype instance of AOutDataSectionReader to be registered to
    /// SectionReader.
    static AOutDataSectionReader proto_;
};
}

#endif
