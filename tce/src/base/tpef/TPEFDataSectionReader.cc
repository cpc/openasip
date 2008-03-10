/**
 * @file TPEFDataSectionReader.cc
 *
 * Definition of TPEFDataSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFDataSectionReader.hh"
#include "SectionReader.hh"
#include "TPEFBaseType.hh"
#include "SafePointer.hh"

#include "DataSection.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionIndexKey;
using ReferenceManager::FileOffsetKey;
using std::string;

TPEFDataSectionReader TPEFDataSectionReader::proto_;

/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
TPEFDataSectionReader::TPEFDataSectionReader() : TPEFSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
TPEFDataSectionReader::~TPEFDataSectionReader() {
}

/**
 * Returns the type of section it is meant to read.
 *
 * @return The type of section it can read.
 */
Section::SectionType
TPEFDataSectionReader::type() const {
    return Section::ST_DATA;
}

/**
 * Reads data section of TPEF binary file.
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
TPEFDataSectionReader::readData(
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

    // base classes implementation must be called with these.
    TPEFSectionReader::readData(stream, section);

    DataSection* dataSection = dynamic_cast<DataSection*>(section);
    assert(dataSection != NULL);

    // check that link section is defined properly
    assert(header().linkId == 0);

    if (!section->noBits()) {
        while (stream.readPosition() <
               header().bodyOffset + header().bodyLength) {
            dataSection->addByte(stream.readByte());
        }

        dynamic_cast<TPEFReader*>(
            parent())->addSectionSize(section, header().bodyLength);
    }
}

}
