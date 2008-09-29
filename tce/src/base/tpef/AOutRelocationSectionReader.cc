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
 * @file AOutRelocationSectionReader.cc
 *
 * Implementation of AOutRelocationSectionReader class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 7 October 2003 by jn, ml, tr, ll
 *
 * @note rating: yellow
 */

#include "AOutRelocationSectionReader.hh"
#include "RelocElement.hh"
#include "ReferenceKey.hh"
#include "SafePointer.hh"
#include "AOutReader.hh"
#include "TPEFBaseType.hh"
#include "RelocSection.hh"
#include "CodeSection.hh"
#include "DataSection.hh"
#include "SectionReader.hh"
#include "AOutSymbolSectionReader.hh"
#include "AOutTextSectionReader.hh"
#include "ImmediateElement.hh"
#include "Swapper.hh"

namespace TPEF {

using std::string;

using ReferenceManager::SectionOffsetKey;
using ReferenceManager::SectionIndexKey;
using ReferenceManager::SafePointer;

AOutRelocationSectionReader AOutRelocationSectionReader::proto_;
const Byte AOutRelocationSectionReader::RELOCATION_TYPE_MASK = 0x1F;

/**
 * Constructor.
 *
 * Registers itself to AOutSectionReader.
 */
AOutRelocationSectionReader::AOutRelocationSectionReader() :
    AOutSectionReader() {

    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
AOutRelocationSectionReader::~AOutRelocationSectionReader() {
}

/**
 * Returns the type of section that reader can read.
 *
 * @return The type of section that reader can read.
 */
Section::SectionType
AOutRelocationSectionReader::type() const {
    return Section::ST_RELOC;
}

/**
 * Reads relocation sections from a.out binary file.
 *
 * @param stream The stream to be read from.
 * @param section Section where to information is stored.
 * @exception UnreachableStream If reading of section fails.
 * @exception KeyAlreadyExists Key was in use when trying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some of read values were out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue If there was unexpected value when reading.
 */
void
AOutRelocationSectionReader::readData(
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

    FileOffset offset = stream.readPosition();

    AOutReader* aOutReader = dynamic_cast<AOutReader*>(parent());
    assert(aOutReader != NULL);

    // find out whether this is for reading text relocation or data
    // relocation
    RelocSection* relocSection = dynamic_cast<RelocSection*>(section);
    assert(relocSection != NULL);

    const Section* refSection = relocSection->referencedSection();
    assert(refSection != NULL);

    SectionId refSectionID;

    Word length = 0;
    if (refSection->type() == Section::ST_CODE) {
        refSectionID = AOutReader::ST_TEXT;
        length = aOutReader->header().sectionSizeTextReloc();

    } else if (refSection->type() == Section::ST_DATA) {
        refSectionID = AOutReader::ST_DATA;
        length = aOutReader->header().sectionSizeDataReloc();

    } else {
        string msg = "Relocating something else than text or data section";
        throw WrongSubclass(__FILE__, __LINE__, __func__, msg);
    }

    // all relocation information is read; to offset 'offset+length'
    for (; stream.readPosition() < offset + length;) {

        // create and read a new relocation element
        RelocElement* elem = new RelocElement();
        initializeRelocElement(stream, elem, refSectionID, aOutReader);

        relocSection->addElement(elem);
    }
}

/**
 * Initializes one RelocElement object with correct values.
 *
 * @param stream The stream to be read from.
 * @param elem Element to be initialized.
 * @param refSectionID Identification code of the section that holds
 *                     relocation source.
 * @param reader The base reader for a.out.
 * @exception UnexpectedValue If extern bit is not zero, or if relocation
 *            target is not text or data section.
 * @exception UnreachableStream If stream can't be read.
 * @exception OutOfRange Section offset of an address is invalid.
 */
void
AOutRelocationSectionReader::initializeRelocElement(
    BinaryStream& stream,
    RelocElement* elem,
    SectionId refSectionID,
    AOutReader* reader) const
    throw (UnexpectedValue, UnreachableStream, OutOfRange) {

    Word r_address = stream.readWord();

    // if referenced section is text section fix r_address to point
    // starting element of instruction
    // (r_address pointed in the middle of move to immediate value before)
    if (refSectionID == AOutReader::ST_TEXT) {
        r_address -= AOutTextSectionReader::OFFSET_TO_IMMEDIATE_VALUE;
    }

    SectionOffsetKey sKey = SectionOffsetKey(refSectionID, r_address);
    elem->setLocation(CREATE_SAFEPOINTER(sKey));

    Word secondWord = stream.readWord();

    Word r_symbolnum = (secondWord >> BYTE_BITWIDTH);

    // addend is value that is stored in element in location pointed by
    // r_address
    AddressImage r_addend = stream.readWord();

    // It seems that extern flag and addend field is used to tell does
    // relocation have symbol or section index in symbolnum field.
    // If there is symbol it means that relocation is unresolved.

    // NOTE: above doesn't work if there is relocation to the 
    //       first instruction of program in this case secondWord seems to
    //       be always 1026... 
    if (r_addend == 0 &&  checkIfExtern(secondWord) && secondWord != 1026) {
        SectionIndexKey indexKey(AOutReader::ST_SYMBOL, r_symbolnum + 1);
        elem->setSymbol(CREATE_SAFEPOINTER(indexKey));

    } else {
        // if we are here r_symbolnum is a section id, not a symbol index
        SectionIndexKey undefSym(AOutReader::ST_SYMBOL,0);
        elem->setSymbol(CREATE_SAFEPOINTER(undefSym));

        Word r_offset = reader->sectionOffsetOfAddress(r_addend);

        SectionOffsetKey offKey =
            SectionOffsetKey(r_symbolnum, r_offset);
            
        elem->setDestination(CREATE_SAFEPOINTER(offKey));
    }

    // convert and set relocation type
    RelocType r_type =
        static_cast<RelocType>(secondWord & RELOCATION_TYPE_MASK);

    elem->setType(aOutToTPEFRelocType(r_type));

    elem->setSize(sizeof(Word)*BYTE_BITWIDTH);
}

/**
 * Finalizer method for AOut relocation sections.
 *
 * Fixes values of elements pointed by location(), MOVE linker does not
 * update values of elements referred from relocation. Also sets address
 * spaces for relocation elements.
 *
 * @param section Section to finalize.
 */
void
AOutRelocationSectionReader::finalize(Section* section) const {

    Section *refSection =
        dynamic_cast<RelocSection*>(section)->referencedSection();

    AOutReader *aOutReader = dynamic_cast<AOutReader*>(parent());
    assert(aOutReader != NULL);

    for (Word i = 0;
         i != section->elementCount();
         i++) {

        RelocElement *elem = dynamic_cast<RelocElement*>(section->element(i));

        // address space of section where element is or undef aSpace
        elem->setASpace(
            dynamic_cast<AOutReader*>
            (parent())->aSpaceOfElement(elem->destination()));

        // relocation is unresolved.
        if (elem->destination() == NULL) continue;

        AddressImage destAddress =
            aOutReader->addressOfElement(elem->destination());

        // fix value of immediate or chunk
        if (refSection->type() == Section::ST_CODE) {

            // value of immediate element, is file offset to
            // that element, which to immediate refers
            ImmediateElement *imm =
                dynamic_cast<ImmediateElement*>(elem->location());

            // relocation must have Immediate or Chunk element as location
            assert(imm != NULL);

            // fix the address value
            imm->setWord(destAddress);

        } else if (refSection->type() == Section::ST_DATA) {
            Chunk* dataChunk = dynamic_cast<Chunk*>(elem->location());
            DataSection* dataSection = dynamic_cast<DataSection*>(refSection);

            // check that section is long enough
            if (dataChunk->offset() + sizeof(destAddress) >
                dataSection->length()) {
                bool requestedChunkWasOutOfRange = false;
                assert(requestedChunkWasOutOfRange);
            }

            // fix address of chunk
            Word mauIndex = dataSection->chunkToMAUIndex(dataChunk);

            dataSection->writeValue(
                mauIndex, 4, static_cast<unsigned long>(destAddress));

        } else {
            bool referencedSectionMustBeEitherCodeOrData = false;
            assert(referencedSectionMustBeEitherCodeOrData);
        }
    }
}

/**
 * Converts a.out relocation type into TPEF relocation.
 *
 * @param aOutRelocType Identification code of the relocation type to convert
 *                      to TPEF format.
 * @return TPEF relocation type.
 */
RelocElement::RelocType
AOutRelocationSectionReader::aOutToTPEFRelocType(
    RelocType aOutRelocType) const {

    // NOTE: check this conversion.

    switch (aOutRelocType) {
    case RELOC_8:
    case RELOC_16:
    case RELOC_32:
        return RelocElement::RT_SELF;
    case NO_RELOC:
        return RelocElement::RT_NOREL;
    default: {
        bool unknownAOutRelocationType = false;
        assert(unknownAOutRelocationType);
    }
    }

    // to prevent compile warnings
    return RelocElement::RT_NOREL;
}

}
