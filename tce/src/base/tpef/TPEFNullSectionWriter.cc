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
 * @file TPEFNullSectionWriter.cc
 *
 * Definitions of TPEFNullSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFNullSectionWriter.hh"
#include "Exception.hh"
#include "SectionSizeReplacer.hh"
#include "TPEFBaseType.hh"
#include "ReferenceKey.hh"
#include "SafePointer.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::FileOffsetKey;

const TPEFNullSectionWriter TPEFNullSectionWriter::instance_;

/**
 * Constructor.
 *
 * Registers itself to SectionWriter.
 */
TPEFNullSectionWriter::TPEFNullSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFNullSectionWriter::~TPEFNullSectionWriter() {
}

/**
 * Returns which type of sections can be written out by this writer.
 *
 * @return The type of section that writer can write out.
 */
Section::SectionType
TPEFNullSectionWriter::type() const {
    return Section::ST_NULL;
}

/**
 * Returns the size of section elements. The null section has no elements,
 * so the size of elements is undefined. Returns always zero.
 *
 * @param section Section whose element size is to be written out (not used).
 * @return Zero.
 */
Word
TPEFNullSectionWriter::elementSize(const Section*) const {
    return 0;
}

/**
 * Returns the identification code of the section.
 *
 * Zero is the numeric code reserved for identifying null sections.
 *
 * @return Zero.
 */
SectionId
TPEFNullSectionWriter::getSectionId() const {
    return 0;
}

/**
 * Writes the size of a null section into an output stream.
 *
 * The size of a null section is always zero.
 *
 * @param stream Output stream.
 * @param sect Section whose size has to be written out (not used).
 */
void
TPEFNullSectionWriter::writeSize(BinaryStream& stream,
    const Section*) const {

    stream.writeWord(0);
}

}
