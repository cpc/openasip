/**
 * @file TPEFUDataSectionReader.cc
 *
 * Definition of TPEFUDataSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFUDataSectionReader.hh"
#include "SectionReader.hh"
#include "BaseType.hh"
#include "SafePointer.hh"

#include "UDataSection.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionIndexKey;
using std::string;

TPEFUDataSectionReader TPEFUDataSectionReader::proto_;

/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
TPEFUDataSectionReader::TPEFUDataSectionReader() : TPEFSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
TPEFUDataSectionReader::~TPEFUDataSectionReader() {
}

/**
 * Returns the type of section it is meant to read.
 *
 * @return The type of section it can read.
 */
Section::SectionType
TPEFUDataSectionReader::type() const {
    return Section::ST_UDATA;
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
TPEFUDataSectionReader::readData(
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

    // base classes implementation must be called with these TPEF readers
    TPEFSectionReader::readData(stream, section);

    UDataSection* uDataSection = dynamic_cast<UDataSection*>(section);
    assert(uDataSection != NULL);

    // check that link section is defined properly
    assert(header().linkId == 0);

    dynamic_cast<TPEFReader*>(
        parent())->addSectionSize(section, header().bodyLength);

    uDataSection->setDataLength(header().bodyLength);
}

}
