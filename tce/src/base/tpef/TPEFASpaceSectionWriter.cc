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
 * @file TPEFASpaceSectionWriter.cc
 *
 * Definition of TPEFASpaceSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
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
