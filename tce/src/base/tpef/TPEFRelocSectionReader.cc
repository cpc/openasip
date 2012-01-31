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
 * @file TPEFRelocSectionReader.cc
 *
 * Definition of TPEFRelocSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <cmath> // ceil()

#include "boost/format.hpp"

#include "TPEFRelocSectionReader.hh"
#include "SafePointer.hh"
#include "Exception.hh"
#include "ReferenceKey.hh"
#include "SectionReader.hh"
#include "TPEFBaseType.hh"
#include "BinaryStream.hh"
#include "RelocSection.hh"
#include "RelocElement.hh"
#include "ImmediateElement.hh"

#include "TPEFHeaders.hh"
#include "ASpaceSection.hh"
#include "CodeSection.hh"
#include "DataSection.hh"

#include "TPEFBaseType.hh"
#include "Locator.hh"

#include "Swapper.hh"

#include "MathTools.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;
using ReferenceManager::SectionIndexKey;
using ReferenceManager::SectionOffsetKey;

TPEFRelocSectionReader TPEFRelocSectionReader::proto_;
SectionId TPEFRelocSectionReader::refSectionId_ = 0;

/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
TPEFRelocSectionReader::TPEFRelocSectionReader() : TPEFSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
TPEFRelocSectionReader::~TPEFRelocSectionReader() {
}

/**
 * Returns the type of section which the reader can read.
 *
 * @return The type of section which reader can read.
 */
Section::SectionType
TPEFRelocSectionReader::type() const {
    return Section::ST_RELOC;
}

/**
 * Reads section data from TPEF binary file.
 *
 * @param stream Stream to be read from.
 * @param section Section where the information is to be stored.
 * @exception UnreachableStream If reading of section fails.
 * @exception KeyAlreadyExists Key was in use when trying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some of read values were out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue Ift here was unexpected value when reading.
 */
void
TPEFRelocSectionReader::readData(
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

    // base classes implementation must be called with these.
    TPEFSectionReader::readData(stream, section);

    RelocSection* relocSection = dynamic_cast<RelocSection*>(section);
    assert(relocSection != NULL);

    // check that link section is defined properly
    assert(header().linkId != 0);

    if (!section->noBits()) {

        // store start of first element
        SectionOffset elementStart = header().bodyOffset;
        SectionIndex id = 0;

        while (elementStart + header().elementSize <=
               header().bodyOffset + header().bodyLength) {

            RelocElement *elem = new RelocElement();

            SectionIndexKey sectionIndexKey(header().sectionId, id);
            SafePointer::addObjectReference(sectionIndexKey, elem);

            // r_offset is source element section offset
            Word rOffset =  stream.readWord();
            SectionOffsetKey sKey(refSectionId_, rOffset);
            elem->setLocation(CREATE_SAFEPOINTER(sKey));

            // r_symbol
            Word rSymbol = stream.readWord();
            SectionIndexKey indexKey(header().linkId, rSymbol);
            elem->setSymbol(CREATE_SAFEPOINTER(indexKey));

            // r_type
            Byte rType = stream.readByte();
            elem->setType(
                static_cast<RelocElement::RelocType>(
                    rType&TPEFHeaders::STF_RELOCATION_TYPE_MASK));

            elem->setChunked(rType & TPEFHeaders::STF_CHUNK);

            // r_asp
            Byte rASP = stream.readByte();
            SectionIndexKey aSpaceIndexKey(
                dynamic_cast<TPEFReader*>(parent())->aSpaceId(), rASP);
            elem->setASpace(CREATE_SAFEPOINTER(aSpaceIndexKey));

            // r_size
            elem->setSize(stream.readByte());

            // r_bitpos skipped for now
            elem->setBitOffset(stream.readByte());

            section->addElement(elem);

            elementStart += header().elementSize;
            stream.setReadPosition(elementStart);

            id++;
        }
    }
}

/**
 * Finalizer method for TPEF relocation sections.
 *
 * Resolves destination fields of relocation elements.
 *
 * @param section Section to finalize.
 */
void
TPEFRelocSectionReader::finalize(Section* section) const {

    Section *refSection =
        dynamic_cast<RelocSection*>(section)->referencedSection();

    TPEFReader* tpefReader = dynamic_cast<TPEFReader*>(parent());

    for (Word i = 0; i < section->elementCount(); i++) {

        RelocElement *elem =
            dynamic_cast<RelocElement*>(section->element(i));

        assert(elem->location() != NULL);

        // file offset of destination element to find
        AddressImage address = 0;

        // should we dig up code or data section
        if (refSection->type() == Section::ST_CODE) {

            // value of immediate element, is an address to that element,
            // which to immediate refers
            ImmediateElement *imm =
                dynamic_cast<ImmediateElement*>(elem->location());

            // relocation must have Immediate or Chunk element as location
            assert(imm != NULL);
            assert(imm->length() <= sizeof(address));

            // read destination address of relocation
            unsigned int neededBits = MathTools::requiredBits(imm->word());
            if (neededBits > elem->size()) {
                throw OutOfRange(
                    __FILE__, __LINE__, __func__,
                    (boost::format(
                        "Destination address width %d of relocation is "
                        "bigger than the field size %d.")
                     % neededBits % elem->size()).str());
            }

            address = imm->word();


        } else if (refSection->type() == Section::ST_DATA) {
            
            try {
                DataSection *dataSect =
                    dynamic_cast<DataSection*>(refSection);

                int startIndex = 
                    dataSect->chunkToMAUIndex(
                        dynamic_cast<Chunk*>(elem->location()));

                int mauSize = dataSect->aSpace()->MAU(); 

                for (int i = 0; i < elem->size() / mauSize; i++) {
                    address = address << mauSize;
                    address = address |  dataSect->MAU(startIndex+i);
                }

            } catch (const OutOfRange &e) {
                bool requestedChunkWasOutOfRange = false;
                assert(requestedChunkWasOutOfRange);
            }

        } else {
            bool referencedSectionMustBeEitherCodeOrData = false;
            assert(referencedSectionMustBeEitherCodeOrData);
        }

        // if unresolved relocation leave destination to undefined
        if (tpefReader->aSpaceSection()->isUndefined(elem->aSpace())) {
            continue;
        }

        // absolute address depending on relocation type and value
        AddressImage absoluteAddress =
            Locator::absoluteAddress(address, elem->type());

        // here we need to resolve section of section referenced
        // by value of location
        Section *destSection =
            tpefReader->sectionOfAddress(elem->aSpace(), absoluteAddress);
        
        assert(destSection != NULL);

        // do the magic!

        // find section identification code
        SectionId destSectionId =
            SafePointer::sectionKeyFor(destSection).sectionId();
        
        SectionOffset sectOffsetOfElement;
        
         // if code section check instructionsize and encoding from 
        // refSection...
        if (destSection->type() == Section::ST_CODE) {
            CodeSection *codeSection = 
                dynamic_cast<CodeSection*>(destSection);
            
            // find instruction number of absolute address
            Word instructionNumber = 
                absoluteAddress - codeSection->startingAddress();
            
            // every index is stored for every instruction element that is
            // start of instruction
            SectionIndexKey sectIndexKey(destSectionId, instructionNumber);
            elem->setDestination(CREATE_SAFEPOINTER(sectIndexKey));
            
        } else {
            
            Word mauInBytes =
                static_cast<Word>(
                    ceil(static_cast<double>(elem->aSpace()->MAU()) / 
                         static_cast<double>(BYTE_BITWIDTH)));
            
            // else find just section offset
            sectOffsetOfElement =
                (absoluteAddress - destSection->startingAddress()) *
                mauInBytes;

            SectionOffsetKey sectOffKey(destSectionId, sectOffsetOfElement);
            elem->setDestination(CREATE_SAFEPOINTER(sectOffKey));
        }
    }
}

/**
 * Reads info field of section header.
 *
 * Read position of stream will be moved 4 bytes forward.
 *
 * @param stream Stream where from info word is read.
 * @param section Reloc section whose info field is read.
 */
void
TPEFRelocSectionReader::readInfo(BinaryStream& stream,
                                 Section* sect) const {

    // referenced section identifier is in first 2 bytes of info field
    refSectionId_ = stream.readHalfWord();

    if (refSectionId_ != 0) {
        SectionKey sKey(refSectionId_);
        dynamic_cast<RelocSection*>(sect)->setReferencedSection(
            CREATE_SAFEPOINTER(sKey));
    } else {
        bool referencedSectionOfRelocationMustNotBeNullSection = false;
        assert(referencedSectionOfRelocationMustNotBeNullSection);
    }

    // skip rest 2 of 4 bytes
    stream.readHalfWord();
}

}
