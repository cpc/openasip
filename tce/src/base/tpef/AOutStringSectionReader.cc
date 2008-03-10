/**
 * @file AOutStringSectionReader.cc
 *
 * Definition of AOutStringSectionReader class.
 *
 * @author Ari Metsähalme (ari.metsahalme@tut.fi)
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 * @note reviewed 17 October 2003 by kl, pj, am, rm
 *
 * @note rating: yellow
 */

#include "Section.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "StringSection.hh"
#include "AOutStringSectionReader.hh"
#include "AOutSymbolSectionReader.hh"

namespace TPEF {

using std::string;
using ReferenceManager::SectionKey;
using ReferenceManager::SafePointer;

AOutStringSectionReader AOutStringSectionReader::proto_;

/**
 * Constructor.
 *
 * Registers to SectionReader.
 */
AOutStringSectionReader::AOutStringSectionReader() : AOutSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
AOutStringSectionReader::~AOutStringSectionReader() {
}

/**
 * Returns the type of section that this class can read.
 *
 * @return Type of section to read.
 */
Section::SectionType
AOutStringSectionReader::type() const {
    return Section::ST_STRTAB;
}

/**
 * Reads string section from a.out binary file.
 *
 * Reads section from stream and makes sure that section starts withs
 * zero.
 *
 * @param stream Stream to be read from.
 * @param section Section where to the information is stored.
 * @exception UnreachableStream If reading of section fails.
 * @exception KeyAlreadyExists Key was in use when trying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some of read values were out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue If there was unexpected value when reading.
 */
void
AOutStringSectionReader::readData(
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

    AOutReader* aOutReader = dynamic_cast<AOutReader*>(parent());

    StringSection* strSect = dynamic_cast<StringSection*>(section);
    assert(strSect != NULL);

    for (unsigned int i = 0;
         i < aOutReader->header().sectionSizeString(); i++) {
        strSect->addByte(stream.readByte());
    }

    // add zero byte if no data
    if (strSect->length() == 0) {
        strSect->addByte(0);
    }

    // section must start with zero.
    assert(strSect->byte(strSect->chunk(0)) == 0);
}

}
