/**
 * @file TPEFSectionReader.hh
 *
 * Declaration of TPEFSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_SECTION_READER_HH
#define TTA_TPEF_SECTION_READER_HH

#include "SectionReader.hh"
#include "TPEFReader.hh"

namespace TPEF {

/**
 * Abstract base class for all TPEFSectionReaders.
 *
 * Class implements default definitions of virtual methods declared in
 * SectionReader class and reading headers of TPEF sections.
 */
class TPEFSectionReader : public SectionReader {
public:
    virtual ~TPEFSectionReader();

protected:
    TPEFSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual BinaryReader* parent() const;

    virtual void readInfo(BinaryStream& stream, Section *sect) const;

    /**
     * Stores data of TPEF section header.
     */
    struct Header {
        SectionId sectionId;
        SectionId linkId;
        Word sectionSize;
        Word elementSize;
        Word bodyOffset;
        Word bodyLength;
    };

    static const Header& header();

private:
    TPEFSectionReader(const TPEFSectionReader&);

    /// Stores values that are needed in actual SectionReader classes.
    static Header header_;
};
}

#endif
