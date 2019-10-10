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
 * @file AOutReader.cc
 *
 * Implementation of class AOutReader.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 7 October 2003 by jn, ml, tr, ll
 *
 * @note rating: yellow
 */

#include <cassert>

#include "ASpaceSection.hh"
#include "ASpaceElement.hh"
#include "AOutReader.hh"
#include "AOutSymbolSectionReader.hh"
#include "Section.hh"
#include "RelocSection.hh"
#include "SectionReader.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "CodeSection.hh"
#include "ResourceSection.hh"
#include "NullSection.hh"
#include "DebugSection.hh"
#include "CodeSection.hh"

namespace TPEF {

using ReferenceManager::SectionKey;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::SafePointer;

//////////////////////////////////////////////////////////////////////////
/// AOutReader class
//////////////////////////////////////////////////////////////////////////


AOutReader* AOutReader::proto_ = NULL;
ResourceSection* AOutReader::resourceTable_ = NULL;
NullSection* AOutReader::nullSection_ = NULL;
DebugSection* AOutReader::debugSection_ = NULL;
StringSection* AOutReader::stringSection_ = NULL;
CodeSection* AOutReader::textSection_ = NULL;

AOutReader::Header AOutReader::header_;

const HalfWord AOutReader::OMAGIC = 0x0107;
const Byte AOutReader::FILE_HEADER_SIZE = 8 * sizeof(Word);

const Byte AOutReader::AOUT_BITS_PER_MAU = 8;
const Byte AOutReader::AOUT_WORD_ALIGN   = 4;
const Byte AOutReader::AOUT_WORD_SIZE    = 4;
const Byte AOutReader::AOUT_INSTRUCTION_SIZE = 8;
const char* AOutReader::AOUT_CODE_ASPACE_NAME = "universal_instructions";
const char* AOutReader::AOUT_DATA_ASPACE_NAME = "universal_data";

const Word AOutReader::INT_REGISTERS = 1024;
const Word AOutReader::FP_REGISTERS  = 1024;
const Word AOutReader::BOOL_REGISTERS = 1;

const Word
AOutReader::FIRST_INT_REGISTER = 0;
const Word
AOutReader::FIRST_FP_REGISTER = FIRST_INT_REGISTER + INT_REGISTERS;
const Word
AOutReader::FIRST_BOOL_REGISTER = FIRST_FP_REGISTER + FP_REGISTERS;
const Word
AOutReader::FIRST_FU_REGISTER = FIRST_BOOL_REGISTER + BOOL_REGISTERS;

/**
 * Constructor.
 *
 * Creates Header and registers itself to BinaryReader.
 */
AOutReader::AOutReader() : BinaryReader(),
    codeASpace_(NULL), dataASpace_(NULL) {

    BinaryReader::registerBinaryReader(this);
}

/**
 * Destructor.
 */
AOutReader::~AOutReader() {

    if (proto_ != NULL) {
        auto proto = proto_;
        proto_ = NULL;
        delete proto;
    }
}

/**
 * Does the actual work of reading binary file from stream.
 *
 * Reads a.out binary file and creates valid TPEF object hierarchy out of it.
 *
 * @param stream Stream to be read from.
 * @return Pointer to the Binary object created.
 * @exception UnreachableStream If reading of section fails.
 * @exception KeyAlreadyExists Key was in use when trying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some of read values were out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue If there was unexpected value when reading.
 */
Binary*
AOutReader::readData(BinaryStream& stream) const {
    Binary* newBin = NULL;
    newBin = new Binary();

    // read section sizes and other file header data
    readHeader(stream);

    // create all a.out sections and
    nullSection_ =
        dynamic_cast<NullSection*>(
            Section::createSection(Section::ST_NULL));
    nullSection_->setLink(nullSection_);

    textSection_ = dynamic_cast<CodeSection*>(
        Section::createSection(Section::ST_CODE));

    DataSection* dataSection = dynamic_cast<DataSection*>(
        Section::createSection(Section::ST_DATA));

    UDataSection* uDataSection = dynamic_cast<UDataSection*>(
        Section::createSection(Section::ST_UDATA));

    stringSection_ = dynamic_cast<StringSection*>(
        Section::createSection(Section::ST_STRTAB));
    stringSection_->setLink(nullSection_);

    Section* symbolTable = Section::createSection(Section::ST_SYMTAB);

    RelocSection* textRelocSection = dynamic_cast<RelocSection*>(
        Section::createSection(Section::ST_RELOC));

    RelocSection* dataRelocSection = dynamic_cast<RelocSection*>(
        Section::createSection(Section::ST_RELOC));

    debugSection_ = dynamic_cast<DebugSection*>(
        Section::createSection(Section::ST_DEBUG));
    debugSection_->setLink(stringSection_);


    // create TPEF needed sections (NullSection and ASpaceSection)
    // and processors resource table
    resourceTable_ = dynamic_cast<ResourceSection*>
        (Section::createSection(Section::ST_MR));

    ASpaceSection* aSpaceSection = dynamic_cast<ASpaceSection*>(
        Section::createSection(Section::ST_ADDRSP));

    newBin->setStrings(stringSection_);
    SectionOffsetKey nullString(ST_STRING, 0);

    undefASpace_ = new ASpaceElement();

    codeASpace_ = new ASpaceElement();
    codeASpace_->setMAU(AOUT_BITS_PER_MAU);
    codeASpace_->setAlign(AOUT_WORD_ALIGN);
    codeASpace_->setWordSize(AOUT_WORD_SIZE);

    dataASpace_ = new ASpaceElement();
    dataASpace_->setMAU(AOUT_BITS_PER_MAU);
    dataASpace_->setAlign(AOUT_WORD_ALIGN);
    dataASpace_->setWordSize(AOUT_WORD_SIZE);    

    aSpaceSection->setUndefinedASpace(undefASpace_);
    aSpaceSection->addElement(undefASpace_);
    aSpaceSection->addElement(codeASpace_);
    aSpaceSection->addElement(dataASpace_);
    aSpaceSection->setLink(stringSection_);

    // set addresses and address spaces

    // code section starts from address zero
    textSection_->setASpace(codeASpace_);
    textSection_->setStartingAddress(0);
    textSection_->setLink(resourceTable_);

    // after that comes data section
    dataSection->setASpace(dataASpace_);
    dataSection->setStartingAddress(header_.sectionSizeText());
    dataSection->setLink(nullSection_);

    // and at last uninitilized data section after that
    uDataSection->setASpace(dataASpace_);
    uDataSection->setStartingAddress(
        (header_.sectionSizeData() + header_.sectionSizeText()));
    uDataSection->setLink(nullSection_);

    //  undefined address spaces for all aux sections
    nullSection_->setASpace(undefASpace_);
    debugSection_->setASpace(undefASpace_);
    aSpaceSection->setASpace(undefASpace_);
    stringSection_->setASpace(undefASpace_);
    symbolTable->setASpace(undefASpace_);
    textRelocSection->setASpace(undefASpace_);
    dataRelocSection->setASpace(undefASpace_);

    resourceTable_->setASpace(undefASpace_);
    resourceTable_->setLink(stringSection_);

    FileOffset textSection_Start = stream.readPosition();

    FileOffset dataSectionStart =
        textSection_Start + header_.sectionSizeText();

    FileOffset textRelocationTableStart =
        dataSectionStart + header_.sectionSizeData();

    FileOffset dataRelocationTableStart =
        textRelocationTableStart + header_.sectionSizeTextReloc();

    FileOffset symbolTableStart =
        dataRelocationTableStart + header_.sectionSizeDataReloc();

    FileOffset stringTableStart =
        symbolTableStart + header_.sectionSizeSymbol();

    // reading sections and creating reference keys
    SafePointer::addObjectReference(SectionKey(ST_UDATA), uDataSection);
    uDataSection->setDataLength(header_.sectionSizeUData());

    SafePointer::addObjectReference(SectionKey(ST_STRING), stringSection_);
    readSection(stream, stringTableStart, stringSection_,
                header_.sectionSizeString());

    SafePointer::addObjectReference(SectionKey(ST_TEXT), textSection_);
    readSection(stream, textSection_Start,
                textSection_, header_.sectionSizeText());

    SafePointer::addObjectReference(SectionKey(ST_DATA), dataSection);
    readSection(stream, dataSectionStart,
                dataSection, header_.sectionSizeData());

    SafePointer::addObjectReference(SectionKey(ST_SYMBOL), symbolTable);
    symbolTable->setLink(stringSection_);
    readSection(stream, symbolTableStart,
                symbolTable, header_.sectionSizeSymbol());

    // relocation sections are not referenced anywhere by SectionId
    textRelocSection->setLink(symbolTable);
    textRelocSection->setReferencedSection(textSection_);
    readSection(stream, textRelocationTableStart,
                textRelocSection, header_.sectionSizeTextReloc());

    dataRelocSection->setLink(symbolTable);
    dataRelocSection->setReferencedSection(dataSection);
    readSection(stream, dataRelocationTableStart,
                dataRelocSection, header_.sectionSizeDataReloc());

    // add sections necessary sections and sections that contained elements
    newBin->addSection(nullSection_);
    newBin->addSection(aSpaceSection);

    // set names for sections and address spaces
    symbolTable->setName(stringSection_->string2Chunk("Symbols"));
    resourceTable_->setName(
        stringSection_->string2Chunk("Universal resources"));
    dataSection->setName(stringSection_->string2Chunk("Initialized data"));
    textSection_->setName(stringSection_->string2Chunk("Code"));
    aSpaceSection->setName(stringSection_->string2Chunk("Address spaces"));
    nullSection_->setName(stringSection_->string2Chunk("Null section"));
    uDataSection->setName(stringSection_->string2Chunk("Uninitilized data"));
    textRelocSection->setName(
        stringSection_->string2Chunk("Relocated immediates"));
    dataRelocSection->setName(stringSection_->string2Chunk("Relocated data"));
    stringSection_->setName(stringSection_->string2Chunk("Strings of TPEF"));
    debugSection_->setName(stringSection_->string2Chunk("A.out stabs"));

    undefASpace_->setName(stringSection_->string2Chunk(""));
    codeASpace_->setName(stringSection_->string2Chunk(AOUT_CODE_ASPACE_NAME));
    dataASpace_->setName(stringSection_->string2Chunk(AOUT_DATA_ASPACE_NAME));

    addOrDeleteSection(textSection_, newBin);
    addOrDeleteSection(textRelocSection, newBin);
    addOrDeleteSection(symbolTable, newBin);
    addOrDeleteSection(dataSection, newBin);
    addOrDeleteSection(dataRelocSection, newBin);
    addOrDeleteSection(uDataSection, newBin);
    addOrDeleteSection(resourceTable_, newBin);
    addOrDeleteSection(debugSection_, newBin);

    // there should be always at least one starting zero
    assert(stringSection_->length() > 0);
    newBin->addSection(stringSection_);

    // can't be sure about these, but external clients will fix
    // values if needed
    newBin->setType(Binary::FT_OBJSEQ);
    newBin->setArch(Binary::FA_TTA_TUT);

    return newBin;
}

/**
 * Checks whether AOutReader can read from the given stream.
 *
 * The test simply consists of checking that file size is at least the size
 * of a.out header. Then it checks whether the third and fourth Byte of
 * stream match the magic number of a.out files. Only 'old impure format'
 * files (not read-only text and not page-aligned for on demand load) are
 * accepted.
 *
 * This method does not modify the stream position.
 *
 * @param stream The stream to test.
 * @return True if stream contains data in a.out format, false otherwise.
 * @exception UnreachableStream If there occurs an exception with stream.
 */
bool
AOutReader::isMyStreamType(BinaryStream& stream) const {
    unsigned long startPos = stream.readPosition();

    try {
        if (stream.sizeOfFile() < FILE_HEADER_SIZE) {
            return false;
        } else {
            stream.readHalfWord();
            Word magic = stream.readHalfWord();

            stream.setReadPosition(startPos);

            if(magic == OMAGIC) {
                return true;
            } else {
                return false;
            }
        }

    } catch (EndOfFile& e) {
        stream.setReadPosition(startPos);
        return false;
    }
}

/**
 * Reads file header from the stream and stores it to a specific object.
 *
 * @param stream Stream to be read from.
 * @exception UnreachableStream If there occurs a problem with stream.
 * @exception EndOfFile If end of file were reached where it shouldn't.
 */
void
AOutReader::readHeader(BinaryStream& stream) const {
    // test magic number again just in case...
    stream.readHalfWord();
    assert(stream.readHalfWord() == OMAGIC);

    header_.setSectionSizeText(stream.readWord());
    header_.setSectionSizeData(stream.readWord());
    header_.setSectionSizeUData(stream.readWord());
    header_.setSectionSizeSymbol(stream.readWord());

    // virtual address of the entry point of the program is ignored
    assert(stream.readWord() == 0);

    header_.setSectionSizeTextReloc(stream.readWord());
    header_.setSectionSizeDataReloc(stream.readWord());

    SectionOffset currentPosition = stream.readPosition();

    // calculate the size of string table
    SectionOffset beginOfStrTable = currentPosition +
        header_.sectionSizeText() + header_.sectionSizeData() +
        header_.sectionSizeSymbol() + header_.sectionSizeTextReloc() +
        header_.sectionSizeDataReloc();

    header_.setSectionSizeString(stream.sizeOfFile() -
                                 beginOfStrTable);
}

//////////////////////////////////////////////////////////////////////////
/// Header class
//////////////////////////////////////////////////////////////////////////


/**
 * Constructor
 */
AOutReader::Header::Header() {
}


/**
 * Destructor
 */
AOutReader::Header::~Header() {
}

}
