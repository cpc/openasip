/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file TPEFDebugSectionReader.cc
 *
 * Definition of TPEFDebugSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
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
