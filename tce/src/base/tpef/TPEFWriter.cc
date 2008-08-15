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
 * @file TPEFWriter.cc
 *
 * Definition of TPEFWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <list>

#include "TPEFWriter.hh"
#include "SectionWriter.hh"
#include "Binary.hh"
#include "Section.hh"
#include "ValueReplacer.hh"
#include "FileOffsetReplacer.hh"
#include "SectionSizeReplacer.hh"

namespace TPEF {

using std::list;

BinaryWriter* TPEFWriter::instance_ = NULL;

/**
 * Constructor.
 */
TPEFWriter::TPEFWriter() : BinaryWriter() {
    setWriter(this);
}

/**
 * Destructor.
 */
TPEFWriter::~TPEFWriter() {
}

/**
 * Returns an instance of this class (singleton).
 *
 * @return Instance of class.
 */
const BinaryWriter& TPEFWriter::instance() {
    if (instance_ == NULL) {
        instance_ = new TPEFWriter();
    }
    return *instance_;
}

/**
 * Writes binary object model in to a binary stream.
 *
 * @param stream Stream where to write.
 * @param bin Binary to write.
 */
void
TPEFWriter::actualWriteBinary(
    BinaryStream& stream,
    const Binary* bin) const {

    // check that there is only one resource section and null section
    if (bin->sectionCount(Section::ST_ADDRSP) > 1 ||
        bin->sectionCount(Section::ST_MR) > 1) {
        
        bool onlyOneAddressSpaceAndResourceSectionIsAllowed = false;
        assert(onlyOneAddressSpaceAndResourceSectionIsAllowed);
    }
    
    FileOffset startOffset = stream.writePosition();

    // file header must be written first
    stream.setWritePosition(startOffset + TPEFHeaders::FH_ID);
    for (Word i = 0; i < TPEFHeaders::FH_ID_SIZE; i++) {
        stream.writeByte(TPEFHeaders::FH_ID_BYTES[i]);
    }
    
    stream.setWritePosition(startOffset + TPEFHeaders::FH_ARCH);
    stream.writeByte(bin->arch());
    
    stream.setWritePosition(startOffset + TPEFHeaders::FH_TYPE);
    stream.writeByte(bin->type());

    // offset to the first section header
    stream.setWritePosition(startOffset + TPEFHeaders::FH_SHOFF);
    stream.writeWord(TPEFHeaders::FH_HEADER_SIZE);

    // size of file header
    stream.setWritePosition(startOffset + TPEFHeaders::FH_SIZE);
    stream.writeHalfWord(TPEFHeaders::FH_HEADER_SIZE);

    stream.setWritePosition(startOffset + TPEFHeaders::FH_SHSIZE);
    stream.writeHalfWord(TPEFHeaders::SH_HEADER_SIZE);

    // number of sections
    stream.setWritePosition(startOffset + TPEFHeaders::FH_SHNUM);
    stream.writeHalfWord(bin->sectionCount());

    // file offset to the header of string table section
    stream.setWritePosition(startOffset + TPEFHeaders::FH_SHSTRTAB);
    if (bin->strings() != NULL) {
        FileOffsetReplacer replacer(bin->strings());
        replacer.resolve();
    } else {
        stream.writeWord(0);
    }

    // offset of first header
    FileOffset sectionHeaderOffset = TPEFHeaders::FH_HEADER_SIZE;

    Section* section = NULL;

    for (Word i = 0; i < bin->sectionCount(); i++) {
        section = bin->section(i);
        try {
            stream.setWritePosition(startOffset + sectionHeaderOffset);
            SectionWriter::writeHeader(stream, section, this);
            sectionHeaderOffset += TPEFHeaders::SH_HEADER_SIZE;

        } catch (const InstanceNotFound& i) {
            std::cerr << "Writer for a section header not found"
                      << std::endl;
            continue;
        }
    }

    // then the section bodies
    for (Word i = 0; i < bin->sectionCount(); i++) {
        section = bin->section(i);
        try {
            if (!section->noBits()) {
                SectionWriter::writeData(stream, section, this);
            } else {
                // TODO: If we are in this branch verify that there is
                //       only undef elements in section.
                //       i.e. verify that section is empty...
            }
        } catch(const InstanceNotFound& i) {
            abortWithError("Writer for a section body not found");
        }
    }
}

}
