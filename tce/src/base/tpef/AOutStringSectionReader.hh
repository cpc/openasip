/**
 * @file AOutStringSectionReader.hh
 *
 * Declaration of AOutStringSectionReader class.
 *
 * @author Ari Metsähalme (ari.metsahalme@tut.fi)
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 * @note reviewed 17 October 2003 by kl, pj, am, rm
 *
 * @note rating: yellow
 */

#ifndef TTA_AOUT_STRING_SECTION_READER_HH
#define TTA_AOUT_STRING_SECTION_READER_HH

#include "AOutSectionReader.hh"
#include "BinaryReader.hh"
#include "Exception.hh"
#include "ReferenceKey.hh"

namespace TPEF {

/**
 * Reads string section from a.out binary file.
 */
class AOutStringSectionReader : public AOutSectionReader {
public:
    virtual ~AOutStringSectionReader();

protected:
    AOutStringSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

private:
    /// Copying not allowed.
    AOutStringSectionReader(const AOutStringSectionReader&);
    /// Assignment not allowed.
    AOutStringSectionReader& operator=(const AOutStringSectionReader&);

    /// Static attribute enables registering to SectionReader
    /// before main function is executed.
    static AOutStringSectionReader proto_;
};
}

#endif
