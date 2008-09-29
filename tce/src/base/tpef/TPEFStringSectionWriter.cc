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
 * @file TPEFStringSectionWriter.cc
 *
 * Definition of TPEFStringSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFStringSectionWriter.hh"
#include "StringSection.hh"
#include "SafePointer.hh"
#include "SectionSizeReplacer.hh"
#include "ReferenceKey.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::FileOffsetKey;


const TPEFStringSectionWriter TPEFStringSectionWriter::instance_;

/**
 * Constructor.
 *
 * Registers itself to SectionWriter.
 */
TPEFStringSectionWriter::TPEFStringSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFStringSectionWriter::~TPEFStringSectionWriter() {
}

/**
 * Returns the type of the section this writer can write.
 *
 * @return The type of the section writer can write.
 */
Section::SectionType
TPEFStringSectionWriter::type() const {
    return Section::ST_STRTAB;
}

/**
 * Writes the data of the section to the stream.
 *
 * @param stream The stream to be written.
 * @param sect The section which data is written to stream.
 */
void
TPEFStringSectionWriter::actualWriteData(
    BinaryStream& stream,
    const Section* sect) const {

    const StringSection* stringSection =
        dynamic_cast<const StringSection*>(sect);

    assert(stringSection != NULL);

    FileOffset startOffset = stream.writePosition();

    // file offset to data of section
    SafePointer::addObjectReference(FileOffsetKey(startOffset),
                                    sect->chunk(0));

    SectionOffset sectOffset = 0;
    SectionKey sKey = SafePointer::sectionKeyFor(stringSection);
    SectionId id = sKey.sectionId();

    // first byte
    assert(stringSection->byte(stringSection->chunk(0)) == 0);

    // last byte must be also zero
    assert(stringSection->byte(
               stringSection->chunk(
                   stringSection->length() - 1)) == 0);

    for (unsigned int i = 0; i < stringSection->length(); i++) {
        sectOffset = stream.writePosition() - startOffset;

        Chunk* chunk = NULL;

        try {
            chunk = stringSection->chunk(sectOffset);
        } catch (const NotChunkable& n) {
            bool sectionToBeWrittenIsNotChunkable = false;
            assert(sectionToBeWrittenIsNotChunkable);
        }

        if (SafePointer::isReferenced(chunk)) {

            SafePointer::addObjectReference(
                SectionOffsetKey(id, sectOffset), chunk);
        }

        stream.writeByte(stringSection->byte(chunk));
    }

    SectionSizeReplacer::setSize(
        stringSection, stream.writePosition() - startOffset);
}

/**
 * Returns size of char.
 *
 * @return Size of char.
 */
Word
TPEFStringSectionWriter::elementSize(const Section* /*section*/) const {
    return sizeof(char);
}

}
