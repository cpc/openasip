/**
 * @file TPEFReader.hh
 *
 * Declaration of TPEFReader.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_READER_HH
#define TTA_TPEF_READER_HH

#include "BinaryReader.hh"
#include "Binary.hh"
#include "Exception.hh"
#include "TPEFBaseType.hh"
#include "Section.hh"
#include "BinaryStream.hh"
#include "TPEFHeaders.hh"
#include "ASpaceSection.hh"

namespace TPEF {

/**
 * Reads binary file that is in TPEF binary format.
 */
class TPEFReader : public BinaryReader {
public:
    Section* sectionOfAddress(
        const ASpaceElement* aSpaceId,
        AddressImage address) const;

    static BinaryReader* instance();

    SectionId aSpaceId();
    SectionId strTableId();

    void addSectionSize(const Section* sect, Word length);

    ASpaceSection* aSpaceSection() const;

protected:
    TPEFReader(const TPEFReader&);
    TPEFReader& operator=(const TPEFReader&);

    TPEFReader();
    virtual ~TPEFReader();

    virtual Binary* readData(BinaryStream& stream) const
        throw (InstanceNotFound, UnreachableStream, KeyAlreadyExists,
               EndOfFile, OutOfRange, WrongSubclass, UnexpectedValue);

    virtual bool isMyStreamType(BinaryStream& stream) const
        throw (UnreachableStream);

private:
    Section* readSection(
        BinaryStream& stream,
        FileOffset headerOffset,
        Byte stringTableId,
        Byte aSpaceSectionId) const;


    /// Prototype instance registered into BinaryReader.
    static TPEFReader* proto_;

    /// Last or currently read binary.
    mutable Binary *binary_;
    /// Identification code of address space section (there is only
    /// one at maximum).
    mutable SectionId aSpaceId_;
    /// Identification code of string table containing section name strings.
    mutable SectionId strTableId_;
    /// Cache of sizes in addresses of read sections.
    mutable std::map<const Section*, Word> sectionSizes_;
};
}

#endif
