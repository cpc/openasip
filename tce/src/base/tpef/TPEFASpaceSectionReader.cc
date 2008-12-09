/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file TPEFASpaceSectionReader.cc
 *
 * Definition of TPEFASpaceSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFASpaceSectionReader.hh"
#include "ASpaceSection.hh"
#include "SafePointer.hh"
#include "Exception.hh"
#include "ReferenceKey.hh"
#include "SectionReader.hh"
#include "TPEFBaseType.hh"
#include "ASpaceElement.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionIndexKey;
using ReferenceManager::SectionOffsetKey;

TPEFASpaceSectionReader TPEFASpaceSectionReader::proto_;

/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
TPEFASpaceSectionReader::TPEFASpaceSectionReader() : TPEFSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
TPEFASpaceSectionReader::~TPEFASpaceSectionReader() {
}

/**
 * Returns the type of section it is meant to read.
 *
 * @return The type of section it can read.
 */
Section::SectionType
TPEFASpaceSectionReader::type() const {
    return Section::ST_ADDRSP;
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
TPEFASpaceSectionReader::readData(
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

    // base classes implementation must be called with these.
    TPEFSectionReader::readData(stream, section);

    // just being paranoid...
    ASpaceSection* aSpaceSection =
        dynamic_cast<ASpaceSection*>(section);
    assert(aSpaceSection != NULL);

    std::set<Byte> alreadyUsedIds;

    bool undefFound = false;

    // check that link section is defined properly
    assert(header().linkId != 0);

    if (!section->noBits()) {
        // start of first element
        SectionOffset elementStart = header().bodyOffset;

        while (elementStart + header().elementSize <=
               header().bodyOffset + header().bodyLength) {

            ASpaceElement *elem = new ASpaceElement();

            // first byte of address space element is a unique identifier
            Byte id = stream.readByte();

            // check that the identifier is really unique
            if (alreadyUsedIds.find(id) != alreadyUsedIds.end()) {
                bool twoASpaceElementsWithSameId = false;
                assert(twoASpaceElementsWithSameId);
            } else {
                alreadyUsedIds.insert(id);
            }

            SectionIndexKey sectionIndexKey(
                dynamic_cast<TPEFReader*>(parent())->aSpaceId(), id);
            SafePointer::addObjectReference(sectionIndexKey, elem);

            elem->setMAU(stream.readByte());
            elem->setAlign(stream.readByte());
            elem->setWordSize(stream.readByte());

            SectionOffsetKey nameSectionOffset(
                header().linkId, stream.readWord());

            elem->setName(CREATE_SAFEPOINTER(nameSectionOffset));

            // check undef aspace
            if (id == 0) {
                assert(elem->MAU() == 0);
                assert(elem->align() == 0);
                assert(elem->wordSize() == 0);
                assert(nameSectionOffset.offset() == 0);
                aSpaceSection->setUndefinedASpace(elem);
                undefFound = true;
            }

            section->addElement(elem);

            elementStart += header().elementSize;
            stream.setReadPosition(elementStart);
        }
    }

    // if there was no undef aspace create it (if nobits flag is set)
    if (!undefFound) {
        ASpaceElement *elem = new ASpaceElement();
        elem->setMAU(0);
        elem->setAlign(0);
        elem->setWordSize(0);
        section->addElement(elem);
        aSpaceSection->setUndefinedASpace(elem);
    }
}

}
