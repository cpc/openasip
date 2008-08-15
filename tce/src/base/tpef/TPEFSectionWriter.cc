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
 * @file TPEFSectionWriter.cc
 *
 * Definitions of TPEFSectionWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <list>

#include "TPEFSectionWriter.hh"
#include "SectionOffsetReplacer.hh"
#include "SectionWriter.hh"
#include "BinaryWriter.hh"
#include "TPEFWriter.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "SectionSizeReplacer.hh"
#include "DataSection.hh"
#include "FileOffsetReplacer.hh"
#include "SectionElement.hh"
#include "StringSection.hh"
#include "SectionIdReplacer.hh"
#include "SectionIndexReplacer.hh"
#include "RelocSection.hh"

namespace TPEF {

using std::list;
using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;
using ReferenceManager::FileOffsetKey;

/**
 * Constructor.
 */
TPEFSectionWriter::TPEFSectionWriter() : SectionWriter() {
}

/**
 * Destructor.
 */
TPEFSectionWriter::~TPEFSectionWriter() {
}

/**
 * Returns binary writer which for section writers are created.
 *
 * @return Binary writer which for section writers are created.
 */
const BinaryWriter&
TPEFSectionWriter::parent() const {
    return TPEFWriter::instance();
}

/**
 * Writes section's header into stream.
 *
 * @param stream Stream where to write.
 * @param sect Section to be written.
 */
void
TPEFSectionWriter::actualWriteHeader(
    BinaryStream& stream,
    const Section* sect) const {

    // inform reference manager about the section
    SectionOffset sectionOffset = stream.writePosition();

    // getSectionId must be overridden for special section identifiers
    // like NullSection's
    SectionId id = getSectionId();

    // create reference keys for section class
    SafePointer::addObjectReference(SectionKey(id), sect);
    SafePointer::addObjectReference(FileOffsetKey(sectionOffset), sect);

    // start writing header fields
    stream.setWritePosition(sectionOffset + TPEFHeaders::SH_NAME);
    Chunk* name = sect->name();

    if (name != NULL) {
        SectionOffsetReplacer nameReplacer(name);
        nameReplacer.resolve();
    } else {
        stream.writeWord(0);
    }

    stream.setWritePosition(sectionOffset + TPEFHeaders::SH_TYPE);
    stream.writeByte(sect->type());

    stream.setWritePosition(sectionOffset + TPEFHeaders::SH_FLAGS);
    stream.writeByte(sect->flags());

    stream.setWritePosition(sectionOffset + TPEFHeaders::SH_ADDR);

    if (sect->isProgramSection()) {
        stream.writeWord(sect->startingAddress());
    } else {
        stream.writeWord(0);
    }

    // offset of the section data
    stream.setWritePosition(sectionOffset + TPEFHeaders::SH_OFFSET);
    writeBodyStartOffset(stream, sect);

    // size of the section
    stream.setWritePosition(sectionOffset + TPEFHeaders::SH_SIZE);
    writeSize(stream, sect);

    // section identification code
    stream.setWritePosition(sectionOffset + TPEFHeaders::SH_ID);
    stream.writeHalfWord(id);


    stream.setWritePosition(sectionOffset + TPEFHeaders::SH_ASPACE);
    if (sect->aSpace() != NULL) {
        // section address space identifier
        SectionIndexReplacer indexReplacer(sect->aSpace(), sizeof(Byte));
        indexReplacer.resolve();
    } else {
        bool addressSpaceCantBeNull = false;
        assert(addressSpaceCantBeNull);
    }

    stream.setWritePosition(sectionOffset + TPEFHeaders::SH_PADDING);
    stream.writeByte(0);

    stream.setWritePosition(sectionOffset + TPEFHeaders::SH_LINK);
    Section* linkSect = sect->link();

    if (linkSect != NULL) {
        SectionIdReplacer linkReplacer(linkSect);
        linkReplacer.resolve();
    } else {
        stream.writeHalfWord(0);
    }

    // by deafult info field is not used.
    stream.setWritePosition(sectionOffset + TPEFHeaders::SH_INFO);
    writeInfo(stream, sect);

    // section elements size is asked from actual section writer
    stream.setWritePosition(sectionOffset + TPEFHeaders::SH_ENTSIZE);

    // check that if element size is zero then vLen flag is set or
    // section is null section
    assert((elementSize(sect) == 0) ==
           (sect->vLen() || (sect->type() == Section::ST_NULL)));
    stream.writeWord(elementSize(sect));

    // create keys for section elements if nobits flag is set.
    if (sect->noBits()) {
        createKeys(sect);
    }
}

/**
 * Empty default implementation of method.
 *
 * This is used if Section does not contain any data part.
 *
 * @param stream Where to write.
 * @param sect Data to write.
 */
void
TPEFSectionWriter::actualWriteData(BinaryStream& /*stream*/,
                                   const Section* /*sect*/) const {
}

/**
 * Returns fixed element size of section.
 *
 * @param sect Data to write.
 * @return Element size of section.
 */
Word
TPEFSectionWriter::elementSize(const Section* /*sect*/) const {
    return 0;
}

/**
 * Writes section size field in to stream.
 *
 * Default implementation, which uses replacer class to write
 * size.
 *
 * @param stream Stream, where to write.
 * @param sect Section which size to write.
 */
void
TPEFSectionWriter::writeSize(BinaryStream& /*stream*/,
                             const Section* sect) const {
    SectionSizeReplacer replacer(sect);
    replacer.resolve();
}

/**
 * Writes section info field into stream.
 *
 * Default implementation writes, there zero word.
 *
 * @param stream Stream where to write.
 * @param sect Section which info field to write.
 */
void
TPEFSectionWriter::writeInfo(BinaryStream& stream,
                             const Section* /*sect*/) const {
    stream.writeWord(0);
}

/**
 * Writes Section's data area offset into stream.
 *
 * @param stream Stream where to write.
 * @param sect Section which data's offset to write.
 */
void
TPEFSectionWriter::writeBodyStartOffset(BinaryStream& stream,
                                        const Section* sect) const {

    if (sect->noBits()) {
        stream.writeWord(0);

    } else {
        if (sect->isChunkable()) {
            FileOffsetReplacer replacer(sect->chunk(0));
            replacer.resolve();
        } else {
            FileOffsetReplacer replacer(sect->element(0));
            replacer.resolve();
        }
    }
}

/**
 * Get section identification code for the section.
 *
 * @return Valid section identification code.
 */
SectionId
TPEFSectionWriter::getSectionId() const {
    return SectionWriter::getUniqueSectionId();
}

/**
 * Fall through method for creating reference keys.
 *
 * This method is used for those sections, whose elements can be
 * referred, but there is no section body stored in TPEF binary
 * (if section has no bits flag set, then this method is invoked
 * instead writeData).
 *
 * Empty implementation for sections that don't need keys created if
 * section do not have body.
 *
 * @param sect Section which for keys will be created.
 */
void
TPEFSectionWriter::createKeys(const Section* /*sect*/) const {
}

}
