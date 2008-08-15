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
 * @file TPEFUDataSectionWriter.cc
 *
 * Definition of TPEFUDataSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <cmath>

#include "TPEFUDataSectionWriter.hh"
#include "Exception.hh"
#include "SectionSizeReplacer.hh"
#include "TPEFBaseType.hh"
#include "ReferenceKey.hh"
#include "SafePointer.hh"
#include "UDataSection.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::FileOffsetKey;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::SectionKey;

const TPEFUDataSectionWriter TPEFUDataSectionWriter::instance_;

/**
 * Constructor.
 *
 * Registers itself to SectionWriter.
 */
TPEFUDataSectionWriter::TPEFUDataSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFUDataSectionWriter::~TPEFUDataSectionWriter() {
}

/**
 * Returns the type of section, that writer can write.
 *
 * @return The type of the section, that writer can write.
 */
Section::SectionType
TPEFUDataSectionWriter::type() const {
    return Section::ST_UDATA;
}

/**
 * Returns the fixed size of section elements in bytes.
 *
 * Size of element depends on address space of section.
 *
 * @param section Section which is written.
 * @return Size of uninitialized data element in bytes.
 */
Word
TPEFUDataSectionWriter::elementSize(const Section* section) const {
    return static_cast<Word>(
        ceil(static_cast<double>(section->aSpace()->MAU()) /
             BYTE_BITWIDTH));
}

/**
 * Writes section's size into stream.
 *
 * @param stream Stream to write.
 * @param sect Section which size to write.
 */
void
TPEFUDataSectionWriter::writeSize(BinaryStream& stream,
                                  const Section* sect) const {

    const UDataSection* uSect = dynamic_cast<const UDataSection*>(sect);
    stream.writeWord(uSect->length());
}

/**
 * Creates keys to make able to reference UData chunks with section offsets.
 *
 * @param sect Section which for keys will be created.
 */
void
TPEFUDataSectionWriter::createKeys(const Section* sect) const {
    const UDataSection* uDSection = dynamic_cast<const UDataSection*>(sect);

    assert(uDSection != NULL);

    SectionOffset sectOffset = 0;
    SectionKey sKey = SafePointer::sectionKeyFor(uDSection);
    SectionId id = sKey.sectionId();

    // key for first element is always set
    Chunk* chunk = uDSection->chunk(0);

    SafePointer::addObjectReference(
        SectionOffsetKey(id, sectOffset), chunk);

    // go through requested chunks..
    for (unsigned int i = 0; i < uDSection->referredChunkCount(); i++) {
        try {
            chunk = uDSection->referredChunk(i);
        } catch (const NotChunkable& n) {
            bool sectionToWriteIsNotChunkable = false;
            assert(sectionToWriteIsNotChunkable);
        }

        sectOffset = chunk->offset();

        // add section offset to element if really referenced element
        if (SafePointer::isReferenced(chunk) && sectOffset != 0) {
            SafePointer::addObjectReference(
                SectionOffsetKey(id, sectOffset), chunk);
        }
    }
}

}
