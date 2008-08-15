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
 * @file TPEFDebugSectionWriter.cc
 *
 * Definition of TPEFDebugSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <list>

#include "TPEFDebugSectionWriter.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "SectionElement.hh"
#include "SectionSizeReplacer.hh"
#include "SectionOffsetReplacer.hh"
#include "SectionIdReplacer.hh"
#include "DebugElement.hh"
#include "SectionSymElement.hh"

namespace TPEF {

using std::list;
using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;
using ReferenceManager::FileOffsetKey;
using ReferenceManager::SectionIndexKey;

const TPEFDebugSectionWriter TPEFDebugSectionWriter::instance_;

/**
 * Constructor.
 *
 * Registers itself to SectionWriter.
 */
TPEFDebugSectionWriter::TPEFDebugSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFDebugSectionWriter::~TPEFDebugSectionWriter() {
}

/**
 * Returns the section type this writer can write.
 *
 * @return The section type writer can write.
 */
Section::SectionType
TPEFDebugSectionWriter::type() const {
    return Section::ST_DEBUG;
}

/**
 * Writes the data of the section in to stream.
 *
 * @param stream The stream to be written to.
 * @param sect The section to be written.
 */
void
TPEFDebugSectionWriter::actualWriteData(
    BinaryStream& stream,
    const Section* sect) const {

    FileOffset startOffset = stream.writePosition();

    // file offset to data of section
    SafePointer::addObjectReference(
        FileOffsetKey(startOffset), sect->element(0));

    for (Word i = 0; i < sect->elementCount(); i++) {

        DebugElement* elem = dynamic_cast<DebugElement*>(sect->element(i));
        assert(elem != NULL);

        // write type
        stream.writeHalfWord(static_cast<HalfWord>(elem->type()));

        // debug string
        assert(elem->debugString() != NULL);
        SectionOffsetReplacer sOffReplacer(elem->debugString());
        sOffReplacer.resolve();

        // write variable data
        stream.writeWord(elem->length());

        for (unsigned int j = 0; j < elem->length(); j++) {
            stream.writeByte(elem->byte(j));
        }
    }
    SectionSizeReplacer::setSize(sect, stream.writePosition() - startOffset);
}

/**
 * Returns the fixed size length of section elements.
 *
 * @return The fixed size length of section elements.
 */
Word
TPEFDebugSectionWriter::elementSize(const Section* /*section*/) const {
    return 0;
}

/**
 * Creates needed keys for section if noBits flag is set.
 *
 * @param sect Section which for keys will be created.
 */
void
TPEFDebugSectionWriter::createKeys(const Section* sect) const {
    SectionSizeReplacer::setSize(sect, 0x0);
}

}
