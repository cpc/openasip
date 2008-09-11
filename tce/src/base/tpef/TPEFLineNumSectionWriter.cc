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
 * @file TPEFLineNumSectionWriter.cc
 *
 * Definition of TPEFLineNumSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <list>

#include "TPEFLineNumSectionWriter.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "SectionElement.hh"
#include "LineNumSection.hh"
#include "LineNumProcedure.hh"
#include "LineNumElement.hh"
#include "SectionSizeReplacer.hh"
#include "SectionOffsetReplacer.hh"
#include "SectionIndexReplacer.hh"
#include "SectionIdReplacer.hh"

namespace TPEF {

using std::list;

using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;
using ReferenceManager::FileOffsetKey;
using ReferenceManager::SectionIndexKey;

const TPEFLineNumSectionWriter TPEFLineNumSectionWriter::instance_;
const Word TPEFLineNumSectionWriter::elementSize_ = 6;

/**
 * Constructor.
 *
 * Registers itself to SectionWriter.
 */
TPEFLineNumSectionWriter::TPEFLineNumSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFLineNumSectionWriter::~TPEFLineNumSectionWriter() {
}

/**
 * Returns the section type this writer can write.
 *
 * @return The section type writer can write.
 */
Section::SectionType
TPEFLineNumSectionWriter::type() const {
    return Section::ST_LINENO;
}

/**
 * Writes the data of the section in to stream.
 *
 * @param stream The stream to be written to.
 * @param sect The section to be written.
 */
void
TPEFLineNumSectionWriter::actualWriteData(
    BinaryStream& stream,
    const Section* sect) const {

    FileOffset startOffset = stream.writePosition();
    // file offset to data of section
    SafePointer::addObjectReference(
        FileOffsetKey(startOffset), sect->element(0));

    for (Word i = 0; i < sect->elementCount(); i++) {
        LineNumProcedure* elem =
            dynamic_cast<LineNumProcedure*>(sect->element(i));
        assert(elem != NULL);

        if (elem->procedureSymbol() != NULL) {
            SectionIndexReplacer
                replacer(elem->procedureSymbol(), sizeof(Word));
            replacer.resolve();
        } else {
            bool procedureSymbolMustBeSet = false;
            assert(procedureSymbolMustBeSet);
        }

        // line number zero represents procedure
        stream.writeHalfWord(0);

        // write line nums...
        for(Word lineIndex = 0; lineIndex < elem->lineCount(); lineIndex++) {

            const LineNumElement* lineElem = elem->line(lineIndex);

            // write each line
            SectionOffsetReplacer replacer(lineElem->instruction());
            replacer.resolve();
            stream.writeHalfWord(lineElem->lineNumber());
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
TPEFLineNumSectionWriter::elementSize(const Section* /*section*/) const {
    return elementSize_;
}

/**
 * Writes Info field of section header.
 *
 * @param stream The stream where to write.
 * @param sect Section to write.
 */
void
TPEFLineNumSectionWriter::writeInfo(
    BinaryStream& stream, const Section* sect) const {

    const Section* codeSection =
        dynamic_cast<const LineNumSection*>(sect)->codeSection();

    SectionIdReplacer idReplacer(codeSection);
    idReplacer.resolve();

    stream.writeHalfWord(0);
}

}
