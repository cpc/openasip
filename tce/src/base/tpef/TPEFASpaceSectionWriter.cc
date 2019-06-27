/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file TPEFASpaceSectionWriter.cc
 *
 * Definition of TPEFASpaceSectionWriter class.
 *
 * @author Jussi Nyk�nen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepist� 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <list>

#include "TPEFASpaceSectionWriter.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "SectionElement.hh"
#include "ASpaceSection.hh"
#include "ASpaceElement.hh"
#include "SectionSizeReplacer.hh"
#include "SectionOffsetReplacer.hh"
#include "BinaryStream.hh"

namespace TPEF {

using std::list;
using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;
using ReferenceManager::FileOffsetKey;
using ReferenceManager::SectionIndexKey;

const TPEFASpaceSectionWriter TPEFASpaceSectionWriter::instance_;
const Word TPEFASpaceSectionWriter::elementSize_ = 8;

/**
 * Constructor.
 *
 * Registers itself to SectionWriter.
 */
TPEFASpaceSectionWriter::TPEFASpaceSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFASpaceSectionWriter::~TPEFASpaceSectionWriter() {
}

/**
 * Returns the section type this writer can write.
 *
 * @return The section type writer can write.
 */
Section::SectionType
TPEFASpaceSectionWriter::type() const {
    return Section::ST_ADDRSP;
}

/**
 * Writes the data of the section in to stream.
 *
 * @param stream The stream to be written to.
 * @param sect The section to be written.
 */
void
TPEFASpaceSectionWriter::actualWriteData(
    BinaryStream& stream,
    const Section* sect) const {

    FileOffset startOffset = stream.writePosition();

    // file offset to data of section
    SafePointer::addObjectReference(
        FileOffsetKey(startOffset), sect->element(0));

    // resolve identification code of currently written section
    SectionKey sectionKey = SafePointer::sectionKeyFor(sect);

    SectionId sectionId = sectionKey.sectionId();

    // index zero is reserved for undefined element.
    Byte index = 1;
    bool undefASpaceWritten = false;

    for (unsigned int i = 0; i < sect->elementCount(); i++) {

        ASpaceElement* elem = dynamic_cast<ASpaceElement*>(sect->element(i));
        assert(elem != NULL);

        // references to undefined address space will be written out as
        // identification code zero
        if (dynamic_cast<const ASpaceSection*>(sect)->isUndefined(elem)) {
            assert(elem->MAU() == 0);
            assert(elem->wordSize() == 0);
            assert(elem->align() == 0);
            undefASpaceWritten = true;
            writeElement(stream, elem, sectionId, 0);
        } else {
            writeElement(stream, elem, sectionId, index);
        }

        index++;
    }

    assert(undefASpaceWritten);

    SectionSizeReplacer::setSize(sect, stream.writePosition() - startOffset);
}

/**
 * Writes address space element to stream.
 *
 * @param stream Stream where to write.
 * @param elem Element to write.
 * @param id Section identification code of address space section.
 * @param index Identification code of address space element.
 */
void
TPEFASpaceSectionWriter::writeElement(
    BinaryStream &stream,
    const ASpaceElement* elem,
    SectionId id,
    Word index) const {

    // add index of element for index replacer
    SafePointer::addObjectReference(
        SectionIndexKey(id, index), elem);

    stream.writeByte(index);
    stream.writeByte(elem->MAU());
    stream.writeByte(elem->align());
    stream.writeByte(elem->wordSize());

    SectionOffsetReplacer replacer(elem->name());
    replacer.resolve();
}

/**
 * Returns the fixed size length of section elements.
 *
 * @return The fixed size length of section elements.
 */
Word
TPEFASpaceSectionWriter::elementSize(const Section* /*section*/) const {
    return elementSize_;
}

}
