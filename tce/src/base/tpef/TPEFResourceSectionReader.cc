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
 * @file TPEFResourceSectionReader.cc
 *
 * Definition of TPEFResourceSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFResourceSectionReader.hh"
#include "ResourceSection.hh"
#include "SafePointer.hh"
#include "Exception.hh"
#include "ReferenceKey.hh"
#include "SectionReader.hh"
#include "TPEFBaseType.hh"
#include "ResourceElement.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;
using ReferenceManager::SectionIndexKey;
using ReferenceManager::SectionOffsetKey;

TPEFResourceSectionReader TPEFResourceSectionReader::proto_;

/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
TPEFResourceSectionReader::TPEFResourceSectionReader() :
    TPEFSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
TPEFResourceSectionReader::~TPEFResourceSectionReader() {
}

/**
 * Returns the type of section which reader can read.
 *
 * @return The type of section which reader can read.
 */
Section::SectionType
TPEFResourceSectionReader::type() const {
    return Section::ST_MR;
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
TPEFResourceSectionReader::readData(
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

    // base classes implementation must be called with these
    TPEFSectionReader::readData(stream, section);

    ResourceSection* resourceSection =
        dynamic_cast<ResourceSection*>(section);
    assert(resourceSection != NULL);

    // check that link section is defined properly
    assert(header().linkId != 0);

    if (!section->noBits()) {
        // store start of first element
        SectionOffset elementStart = header().bodyOffset;

        while (elementStart + header().elementSize <=
               header().bodyOffset + header().bodyLength) {

            ResourceElement *elem = new ResourceElement();

            // TODO: check that OP, SR and PORT share the same id space and
            // don't collide

            elem->setId(stream.readHalfWord());

            elem->setType(
                static_cast<ResourceElement::ResourceType>
                (stream.readByte()));

            SectionOffsetKey
                nameSectOffset(header().linkId, stream.readWord());

            elem->setName(CREATE_SAFEPOINTER(nameSectOffset));

            elem->setInfo(stream.readWord());

            section->addElement(elem);

            elementStart += header().elementSize;
            stream.setReadPosition(elementStart);
        }
    }
}

}
