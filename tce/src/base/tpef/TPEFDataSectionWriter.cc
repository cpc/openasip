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
 * @file TPEFDataSectionWriter.cc
 *
 * Definition of TPEFDataSectionWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <cmath>

#include "TPEFDataSectionWriter.hh"
#include "DataSection.hh"
#include "Chunk.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"
#include "Exception.hh"
#include "SectionSizeReplacer.hh"
#include "ASpaceElement.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::FileOffsetKey;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::SectionKey;

const TPEFDataSectionWriter TPEFDataSectionWriter::instance_;

/**
 * Constructor.
 */
TPEFDataSectionWriter::TPEFDataSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFDataSectionWriter::~TPEFDataSectionWriter() {
}

/**
 * Returns section type which for writer is implemented.
 *
 * @return Section type which of section that class can write.
 */
Section::SectionType
TPEFDataSectionWriter::type() const {
    return Section::ST_DATA;
}

/**
 * Writes section's data into stream.
 *
 * @param stream Stream where to write.
 * @param sect Section to be written.
 */
void
TPEFDataSectionWriter::actualWriteData(
    BinaryStream& stream,
    const Section* sect) const {

    const DataSection* dSection = dynamic_cast<const DataSection*>(sect);

    assert(dSection != NULL);

    FileOffset startOffset = stream.writePosition();

    SectionOffset sectOffset = 0;
    SectionKey sKey = SafePointer::sectionKeyFor(dSection);
    SectionId id = sKey.sectionId();

    for (; sectOffset < dSection->length(); sectOffset++) {

        Chunk* chunk = NULL;

        try {
            chunk = dSection->chunk(sectOffset);
        } catch (const NotChunkable& n) {
            bool sectionToBeWrittenIsNotChunkable = false;
            assert(sectionToBeWrittenIsNotChunkable);
        }

        // if referenced or first element of section
        if (SafePointer::isReferenced(chunk)) {
            SafePointer::addObjectReference(
                SectionOffsetKey(id, sectOffset), chunk);
        }

        stream.writeByte(dSection->byte(chunk));
    }

    // needed for writing section header
    SafePointer::addObjectReference(
        FileOffsetKey(startOffset), dSection->chunk(0));

    SectionSizeReplacer::setSize(
        dSection, stream.writePosition() - startOffset);
}

/**
 * Returns the fixed size of section elements, in this case smallest
 * amount of bytes that is needed to store one MAU of address space.
 *
 * @param section Section which is written.
 * @return Size of uninitialized data element in bytes.
 */
Word
TPEFDataSectionWriter::elementSize(const Section* section) const {
    return static_cast<Word>(
        ceil(static_cast<double>(section->aSpace()->MAU()) /
             static_cast<double>(BYTE_BITWIDTH)));
}

}
