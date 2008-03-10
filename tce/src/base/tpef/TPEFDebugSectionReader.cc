/**
 * @file TPEFDebugSectionReader.cc
 *
 * Definition of TPEFDebugSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFDebugSectionReader.hh"
#include "SafePointer.hh"
#include "Exception.hh"
#include "ReferenceKey.hh"
#include "SectionReader.hh"
#include "TPEFBaseType.hh"
#include "DebugSection.hh"
#include "DebugStabElem.hh"


namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionIndexKey;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::SectionKey;

TPEFDebugSectionReader TPEFDebugSectionReader::proto_;

/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
TPEFDebugSectionReader::TPEFDebugSectionReader() : TPEFSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
TPEFDebugSectionReader::~TPEFDebugSectionReader() {
}

/**
 * Returns the type of section it is meant to read.
 *
 * @return The type of section it can read.
 */
Section::SectionType
TPEFDebugSectionReader::type() const {
    return Section::ST_DEBUG;
}

/**
 * Reads section data from TPEF binary file.
 *
 * @param stream Stream to be read from.
 * @param section Section where the information is to be stored.
 * @exception UnreachableStream If reading of section fails.
 * @exception KeyAlreadyExists Key was in use when trying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some of read value were out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue If there was unexpected value when reading.
 */
void
TPEFDebugSectionReader::readData(
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

    // size taken by fields common to any type of debug section element
    const std::size_t BASE_ELEM_SIZE = 10;

    // base classes implementation must be called with these.
    TPEFSectionReader::readData(stream, section);

    DebugSection* debugSection =
        dynamic_cast<DebugSection*>(section);
    assert(debugSection != NULL);

    // check that link section is defined properly
    assert(header().linkId != 0);

    if (!section->noBits()) {
        // start of first element
        SectionOffset currElement = header().bodyOffset;

        while (currElement < header().bodyOffset + header().bodyLength) {

            HalfWord type = stream.readHalfWord();
            Word stringOffset = stream.readWord();
            Word size = stream.readWord();

            std::vector<Byte> data;

            for (unsigned i = 0; i < size; i++) {
                data.push_back(stream.readByte());
            }

            DebugElement* newElem = NULL;

            switch (static_cast<DebugElement::ElementType>(type)) {
            case DebugElement::DE_STAB:
                newElem = new DebugStabElem(data);
                break;
            default:
                throw UnexpectedValue(
                    __FILE__, __LINE__, __func__,
                    "Unknown debug element type: " +
                    Conversion::toString(type));
            }

            // set name string for element
            SectionOffsetKey sOffKey(header().linkId, stringOffset);
            newElem->setDebugString(CREATE_SAFEPOINTER(sOffKey));

            section->addElement(newElem);

            currElement += size + BASE_ELEM_SIZE;
            stream.setReadPosition(currElement);
        }
    }
}

} // namespace TPEF
