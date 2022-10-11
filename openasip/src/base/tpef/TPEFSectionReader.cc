/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file TPEFSectionReader.cc
 *
 * Definitions of TPEFSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFSectionReader.hh"
#include "BinaryStream.hh"
#include "TPEFHeaders.hh"

namespace TPEF {

TPEFSectionReader::Header TPEFSectionReader::header_;

using ReferenceManager::SectionKey;
using ReferenceManager::SectionIndexKey;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::SafePointer;

/**
 * Constructor
 */
TPEFSectionReader::TPEFSectionReader() : SectionReader() {
}

/**
 * Destructor
 */
TPEFSectionReader::~TPEFSectionReader() {
}

/**
 * Returns binary reader instance which uses TPEFSectionReader classes.
 *
 * @return Binary reader which uses TPEFSectionReader classes.
 */
BinaryReader*
TPEFSectionReader::parent() const {
    return TPEFReader::instance();
}

/**
 * Reads section data from TPEF binary file.
 *
 * Base implementation for all TPEF section readers. Reads and keeps section
 * header data for later use by concrete section readers.  Header data is
 * accessed by readData methods of subclasses.
 *
 * @param stream Stream to be read from.
 * @param section Section where the information is to be stored.
 * @exception UnreachableStream If reading of section fails.
 * @exception KeyAlreadyExists Key was in use when trying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some of read values were out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue If there was unexpected value when reading.
 */
void
TPEFSectionReader::readData(BinaryStream& stream, Section* section) const {
    TPEFReader* tpefReader = dynamic_cast<TPEFReader*>(parent());

    FileOffset startOffset = stream.readPosition();
    stream.setReadPosition(startOffset + TPEFHeaders::SH_FLAGS);
    Byte sectionFlags = stream.readByte();

    // if section vLen flag is not equal to the read value
    if(section->vLen() != ((sectionFlags & Section::SF_VLEN) != 0)) {
        throw UnexpectedValue(
            __FILE__, __LINE__, __func__,
            "read SF_VLEN flag doesn't match for section");
    }

    section->setFlags(sectionFlags);

    stream.setReadPosition(startOffset + TPEFHeaders::SH_ADDR);
    section->setStartingAddress(stream.readWord());

    stream.setReadPosition(startOffset + TPEFHeaders::SH_LINK);
    header_.linkId = stream.readHalfWord();

    SectionKey linkKey(header_.linkId);
    section->setLink(CREATE_SAFEPOINTER(linkKey));

    // set address space reference
    stream.setReadPosition(startOffset + TPEFHeaders::SH_ASPACE);
    Byte aSpaceIndex = stream.readByte();
    SectionIndexKey aSpaceKey(tpefReader->aSpaceId(), aSpaceIndex);
    section->setASpace(CREATE_SAFEPOINTER(aSpaceKey));

    // set name
    stream.setReadPosition(startOffset + TPEFHeaders::SH_NAME);
    Word sectionOffsetOfName = stream.readWord();

    if (tpefReader->strTableId() != 0) {
        SectionOffsetKey nameKey(tpefReader->strTableId(),
                                 sectionOffsetOfName);

        section->setName(CREATE_SAFEPOINTER(nameKey));
    } else {
        section->setName(&SafePointer::null);
    }

    // add section key for new section
    stream.setReadPosition(startOffset + TPEFHeaders::SH_ID);
    header_.sectionId = stream.readHalfWord();

    SectionKey sectionKey(header_.sectionId);
    SafePointer::addObjectReference(sectionKey, section);

    stream.setReadPosition(startOffset + TPEFHeaders::SH_INFO);
    readInfo(stream, section);

    stream.setReadPosition(startOffset + TPEFHeaders::SH_ENTSIZE);
    header_.elementSize = stream.readWord();

    // read offset of section body
    stream.setReadPosition(startOffset + TPEFHeaders::SH_OFFSET);
    header_.bodyOffset = stream.readWord();

    stream.setReadPosition(startOffset + TPEFHeaders::SH_SIZE);
    header_.bodyLength = stream.readWord();

    // actual class does reading of section body
    stream.setReadPosition(header_.bodyOffset);
}

/**
 * Reads info field of section header.
 *
 * Read position of stream will be moved 4 bytes forward.
 *
 * @param stream Stream where from info word is read.
 */
void
TPEFSectionReader::readInfo(BinaryStream& stream,
                            Section* /*sect*/) const {
    // move four bytes forward by default
    stream.readWord();
}

/**
 * Returns headers of section that we are currently reading.
 *
 * @return Headers of section that we are currently reading.
 */
const TPEFSectionReader::Header&
TPEFSectionReader::header() {
    return header_;
}

}
