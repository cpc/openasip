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
 * @file TPEFWriter.cc
 *
 * Definition of TPEFWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
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
#include "BinaryStream.hh"
#include "TPEFHeaders.hh"

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
