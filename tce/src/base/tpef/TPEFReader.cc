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
 * @file TPEFReader.cc
 *
 * Definitions of class TPEFReader.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <list>
#include <cmath>

#include "TPEFReader.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "SectionReader.hh"
#include "CodeSection.hh"
#include "BinaryStream.hh"
#include "Binary.hh"
#include "ASpaceSection.hh"
#include "TPEFHeaders.hh"

namespace TPEF {

using std::list;
using ReferenceManager::SafePointer;

using ReferenceManager::FileOffsetKey;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::SectionKey;
using ReferenceManager::SectionIndexKey;

TPEFReader* TPEFReader::proto_ = NULL;

/**
 * Constructor.
 *
 * Creates Header and registers itself to BinaryReader.
 */
TPEFReader::TPEFReader() : BinaryReader(), aSpaceId_(0), strTableId_(0) {
    BinaryReader::registerBinaryReader(this);
}

/**
 * Destructor.
 */
TPEFReader::~TPEFReader() {
    if (proto_ != NULL) {
        auto proto = proto_;
        proto_ = NULL;
        delete proto;
    }
}

/**
 * Does the actual work of reading binary file from stream.
 *
 * @param stream Stream to be read from.
 * @return Pointer to the Binary object created.
 * @exception UnreachableStream If reading of section fails.
 * @exception KeyAlreadyExists Key was in use whent rying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some of read values were out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue If there was unexpected value when reading.
 */
Binary*
TPEFReader::readData(BinaryStream& stream) const {
    binary_ = new Binary();
    sectionSizes_.clear();

    // let's read file header stuff
    FileOffset startOffset = stream.readPosition();

    // skip file identification code and version
    stream.setReadPosition(startOffset + TPEFHeaders::FH_ARCH);
    binary_->setArch(
        static_cast<Binary::FileArchitecture>(stream.readByte()));

    stream.setReadPosition(startOffset + TPEFHeaders::FH_TYPE);
    binary_->setType(
        static_cast<Binary::FileType>(stream.readByte()));

    stream.setReadPosition(startOffset + TPEFHeaders::FH_SHOFF);
    FileOffset offsetToHeader = stream.readWord();

    // skip size of file header

    stream.setReadPosition(startOffset + TPEFHeaders::FH_SHSIZE);
    HalfWord sectionHeaderSize = stream.readHalfWord();

    stream.setReadPosition(startOffset + TPEFHeaders::FH_SHNUM);
    HalfWord sectionHeaderCount = stream.readHalfWord();

    // string table of section names
    stream.setReadPosition(startOffset + TPEFHeaders::FH_SHSTRTAB);
    Word stringTableFileOffset = stream.readWord();

    if (stringTableFileOffset != 0) {
        FileOffsetKey fKey(stringTableFileOffset);
        binary_->setStrings(CREATE_SAFEPOINTER(fKey));
    } else {
        binary_->setStrings(&SafePointer::null);
    }

    // get identification code of string table section of binary
    if (stringTableFileOffset != 0) {
        stream.setReadPosition(stringTableFileOffset + TPEFHeaders::SH_ID);
        SectionId stringTableSectionId = stream.readHalfWord();
        strTableId_ = stringTableSectionId;
    } else {
        strTableId_ = 0;
    }

    // get identification code of address space section of binary
    SectionId aSpaceSectionId = 0;
    bool programSectionWasFound = false;
    for (Word i = offsetToHeader;
         i < offsetToHeader + (sectionHeaderCount*sectionHeaderSize);
         i += sectionHeaderSize) {

        // go to start of section header
        stream.setReadPosition(i + TPEFHeaders::SH_TYPE);

        Section::SectionType sectionTypeToCheck =
            static_cast<Section::SectionType>(stream.readByte());

        // if section is ASpaceSection
        if (sectionTypeToCheck == Section::ST_ADDRSP) {
            bool brokenBinaryMoreThanOneASpaceSection =
                (aSpaceSectionId == 0);
            assert(brokenBinaryMoreThanOneASpaceSection);

            stream.setReadPosition(i + TPEFHeaders::SH_ID);
            aSpaceSectionId = stream.readHalfWord();

        } else if(Section::isProgramSection(sectionTypeToCheck)) {
            programSectionWasFound = true;
        }
    }

    // there must be address space
    assert(aSpaceSectionId != 0);
    aSpaceId_ = aSpaceSectionId;

    // let's do some serious section-reading...
    for (HalfWord i = 0; i < sectionHeaderCount; i++) {

        stream.setReadPosition(offsetToHeader + TPEFHeaders::SH_TYPE);

        Section* sect = Section::createSection(
            static_cast<Section::SectionType>(stream.readByte()));

        FileOffsetKey sectionFileOffsetKey(offsetToHeader);
        SafePointer::addObjectReference(sectionFileOffsetKey, sect);

        stream.setReadPosition(offsetToHeader);

        SectionReader::readSection(stream, sect, proto_);

        // next header
        offsetToHeader += sectionHeaderSize;
        binary_->addSection(sect);
    }

    // NOTE: This check might be better to do in Binary classes addSection
    //       method. Because it's Binary classes job to prevent illegal
    //       section set. This check only prevents adding too many of these
    //       section while reading.

    // make sure that binary has needed sctions and do not have too many
    // resource or address space sections
    assert(binary_->sectionCount(Section::ST_ADDRSP) == 1);
    assert(binary_->sectionCount(Section::ST_NULL) == 1);
    assert((binary_->sectionCount(Section::ST_MR) == 1) ||
           (binary_->sectionCount(Section::ST_MR) == 0 &&
            programSectionWasFound == false));

    return binary_;
}

/**
 * Checks if this reader is capable of reading the given stream.
 *
 * This method does not modify the stream pointer.
 *
 * @param stream The stream to test.
 * @return True if stream contains data in expected format, false otherwise.
 * @exception UnreachableStream If there occurs an exception with stream.
 */
bool
TPEFReader::isMyStreamType(BinaryStream& stream) const {
    unsigned long startPos = stream.readPosition();

    try {
        if (stream.readByte() == 0x7f &&
            stream.readByte() == 'T' &&
            stream.readByte() == 'T' &&
            stream.readByte() == 'A' &&
            stream.readByte() == '-' &&
            stream.readByte() == 'P' &&
            stream.readByte() == 'F' &&
            stream.readByte() == 0x00) {

            stream.setReadPosition(startPos);
            return true;
        }
    } catch (EndOfFile& e) {
        stream.setReadPosition(startPos);
        return false;
    }

    stream.setReadPosition(startPos);
    return false;
}

/**
 * Returns instance of reader.
 *
 * @return Instance used for reading.
 */
BinaryReader*
TPEFReader::instance() {
    if (proto_ == NULL) {
        proto_ = new TPEFReader();
    }
    return proto_;
}

/**
 * Resolves, which section is in given address in last read binary file.
 *
 * @param aSpace Address space in which section resides in memory image.
 * @param address Address whose section is resolved.
 * @return Section to which given offset refers.
 */
Section*
TPEFReader::sectionOfAddress(
    const ASpaceElement* aSpace,
    AddressImage address) const {
    
    assert(binary_ != NULL);
    
    for (Word i = 0; i < binary_->sectionCount(); i++) {

        Section *sect = binary_->section(i);

        // if section have same aSpace that in parameter
        if (sect->aSpace() == aSpace) {

            Word lastSectionAddress = sect->startingAddress();

            if (sect->type() == Section::ST_CODE) {
                lastSectionAddress += 
                    dynamic_cast<CodeSection*>(sect)->instructionCount();
                
            } else {
                lastSectionAddress += 
                    dynamic_cast<RawSection*>(sect)->lengthInMAUs();
            }

            // test if address is inside base + sh_size
            if (address < lastSectionAddress &&
                address >= sect->startingAddress()) {

                return sect;
            }
        }
    }

    return NULL;
}

/**
 * Stores size of a section as bytes.
 *
 * @param sect Section whose size is stored.
 * @param length Section data size in addresses.
 */
void
TPEFReader::addSectionSize(const Section* sect, Word length) {
    sectionSizes_[sect] = length;
}

/**
 * Returns the identification code of the address space table section.
 *
 * @return Identification code of the address space table section.
 */
SectionId
TPEFReader::aSpaceId() {
    return aSpaceId_;
}

/**
 * Returns the identification code of string table section that contains the
 * names of the sections.
 *
 * @return Identification code of a string table section.
 */
SectionId
TPEFReader::strTableId() {
    return strTableId_;
}

/**
 * Returns address space of file if already read.
 *
 * @return Address space of binary. NULL if there is no ASpace.
 */
ASpaceSection*
TPEFReader::aSpaceSection() const {
    for (unsigned int i = 0; i < binary_->sectionCount(); i++) {
        if (binary_->section(i)->type() == Section::ST_ADDRSP) {
            return dynamic_cast<ASpaceSection*>(binary_->section(i));
        }
    }
    return NULL;
}

}
