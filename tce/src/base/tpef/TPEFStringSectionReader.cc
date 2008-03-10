/**
 * @file TPEFStringSectionReader.cc
 *
 * Definition of TPEFStringSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFStringSectionReader.hh"
#include "SectionReader.hh"
#include "TPEFBaseType.hh"
#include "SafePointer.hh"

#include "StringSection.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionIndexKey;
using ReferenceManager::FileOffsetKey;
using std::string;

TPEFStringSectionReader TPEFStringSectionReader::proto_;

/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
TPEFStringSectionReader::TPEFStringSectionReader() : TPEFSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
TPEFStringSectionReader::~TPEFStringSectionReader() {
}

/**
 * Returns the type of section it is meant to read.
 *
 * @return The type of section it can read.
 */
Section::SectionType
TPEFStringSectionReader::type() const {
    return Section::ST_STRTAB;
}

/**
 * Reads section data from TPEF binary file.
 *
 * @param stream Stream to be read from.
 * @param section Section where the information is to be stored.
 * @exception UnreachableStream If reading of section fails.
 * @exception KeyAlreadyExists Key was in use when trying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some of read values were out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue If there was unexpected value when reading.
 */
void
TPEFStringSectionReader::readData(
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

    // base classes implementation must be called
    TPEFSectionReader::readData(stream, section);

    StringSection* stringSection = dynamic_cast<StringSection*>(section);
    assert(stringSection != NULL);

    // check that link section is defined properly
    assert(header().linkId == 0);

    if (!section->noBits()) {
        while (stream.readPosition() <
               header().bodyOffset + header().bodyLength) {

            stringSection->addByte(stream.readByte());
        }

        assert(stringSection->byte(stringSection->chunk(0)) == 0);
    }

    // if nobits flag was set, add zero string to start of section to make
    // references valid (many sections needs string section 
    // e.g. symbol section).
    if (stringSection->empty()) {
        stringSection->addByte(0);
    }
}

}
