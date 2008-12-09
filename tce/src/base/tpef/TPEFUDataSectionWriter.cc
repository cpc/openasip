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
 * @file TPEFUDataSectionWriter.cc
 *
 * Definition of TPEFUDataSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
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
