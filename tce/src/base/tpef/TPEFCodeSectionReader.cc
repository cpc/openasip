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
 * @file TPEFCodeSectionReader.cc
 *
 * Definition of TPEFCodeSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFCodeSectionReader.hh"
#include "SectionReader.hh"
#include "TPEFBaseType.hh"
#include "SafePointer.hh"

#include "CodeSection.hh"
#include "InstructionElement.hh"
#include "MoveElement.hh"
#include "ImmediateElement.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::SectionIndexKey;
using std::string;

TPEFCodeSectionReader TPEFCodeSectionReader::proto_;

/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
TPEFCodeSectionReader::TPEFCodeSectionReader() : TPEFSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
TPEFCodeSectionReader::~TPEFCodeSectionReader() {
}

/**
 * Returns the type of section it is meant to read.
 *
 * @return The type of section it can read.
 */
Section::SectionType
TPEFCodeSectionReader::type() const {
    return Section::ST_CODE;
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
 * @exception UnexpectedValue If there was unexpected value when reading.
 */
void
TPEFCodeSectionReader::readData(
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

    // base classes implementation must be called with these.
    TPEFSectionReader::readData(stream, section);

    CodeSection* codeSection = dynamic_cast<CodeSection*>(section);
    assert(codeSection != NULL);

    bool nextIsBeginning = true;
    Word sectionIndex = 0;

    // check that link section is defined properly
    // (should point to machine resource section)
    assert(header().linkId != 0);

    if (!section->noBits()) {

        while (stream.readPosition() <
               header().bodyLength + header().bodyOffset) {

            SectionOffsetKey
                sOffsetKey(header().sectionId,
                           stream.readPosition() - header().bodyOffset);

            Byte iAttr = stream.readByte();

            InstructionElement* newInstrElement = NULL;

            //if instruction is immediate
            if (iAttr & TPEFHeaders::IA_TYPE) {

                ImmediateElement* newElem = new ImmediateElement();
                newInstrElement = dynamic_cast<InstructionElement*>(newElem);

                newElem->setDestinationUnit(stream.readByte());
                newElem->setDestinationIndex(stream.readByte());

                //iAttr mask 0xF0 number of bytes of immediate
                Byte size = (iAttr >> (BYTE_BITWIDTH / 2));

                for (Byte i = 0; i < size; i++) {
                    newElem->addByte(stream.readByte());
                }

            } else {

                MoveElement* newElem = new MoveElement();
                newInstrElement = dynamic_cast<InstructionElement*>(newElem);

                newElem->setBus(stream.readByte());

                Byte fieldTypes = stream.readByte();

                if (iAttr & TPEFHeaders::IA_EMPTY) {
                    newElem->setEmpty(true);

                } else {
                    newElem->setEmpty(false);

                    switch (fieldTypes & TPEFHeaders::IE_SRC_TYPE_MASK) {
                    case TPEFHeaders::MVS_RF:
                        newElem->setSourceType(MoveElement::MF_RF);
                        break;
                    case TPEFHeaders::MVS_IMM:
                        newElem->setSourceType(MoveElement::MF_IMM);
                        break;
                    case TPEFHeaders::MVS_UNIT:
                        newElem->setSourceType(MoveElement::MF_UNIT);
                        break;
                    default:
                        std::cerr << "field types: " 
                                  << std::hex << (int)fieldTypes
                                  << std::dec << std::endl;
                        assert(false);
                    }

                    switch (fieldTypes & TPEFHeaders::IE_DST_TYPE_MASK) {
                    case TPEFHeaders::MVD_RF:
                        newElem->setDestinationType(MoveElement::MF_RF);
                        break;
                    case TPEFHeaders::MVD_UNIT:
                        newElem->setDestinationType(MoveElement::MF_UNIT);
                        break;
                    default:
                        std::cerr << "field types: " 
                                  << std::hex << (int)fieldTypes
                                  << std::dec << std::endl;
                        assert(false);
                    }


                    if (iAttr & TPEFHeaders::IA_MGUARD) {
                        newElem->setGuarded(true);

                        switch (fieldTypes & 
                                TPEFHeaders::IE_GUARD_TYPE_MASK) {

                        case TPEFHeaders::MVG_UNIT:
                            newElem->setGuardType(MoveElement::MF_UNIT);
                            break;
                        case TPEFHeaders::MVG_RF:
                            newElem->setGuardType(MoveElement::MF_RF);
                            break;
                        default:
                            std::cerr << "field types: " << std::hex
                                      << (int)fieldTypes 
                                      << std::dec << std::endl;
                            assert(false);
                        }

                    } else {
                        newElem->setGuarded(false);
                    }
                }

                newElem->setSourceUnit(stream.readByte());
                newElem->setSourceIndex(stream.readHalfWord());

                newElem->setDestinationUnit(stream.readByte());
                newElem->setDestinationIndex(stream.readHalfWord());

                newElem->setGuardUnit(stream.readByte());
                newElem->setGuardIndex(stream.readHalfWord());

                // guard extra parameters
                if (fieldTypes & TPEFHeaders::IE_GUARD_INV_MASK) {
                    newElem->setGuardInverted(true);
                } else {
                    newElem->setGuardInverted(false);
                }
            }

            newInstrElement->setBegin(nextIsBeginning);
            nextIsBeginning = (iAttr & TPEFHeaders::IA_END);

            // create annotations for instruction if there are any
            if (iAttr & TPEFHeaders::IA_ANNOTE) {
                readAnnotations(stream, newInstrElement);
            }

            // store reference to number of instruction for
            // relocation destination resolving
            if (newInstrElement->begin()) {
                SectionIndexKey
                    sIndexKey(header().sectionId, sectionIndex);
                SafePointer::addObjectReference(sIndexKey, newInstrElement);
                sectionIndex++;
            }

            SafePointer::addObjectReference(sOffsetKey, newInstrElement);
            codeSection->addElement(newInstrElement);
        }

        // add section size to parent
        dynamic_cast<TPEFReader*>(
            parent())->addSectionSize(section, sectionIndex);
    }
}


/**
 * Reads the info field of code section header.
 *
 * The `info' field of code sections normally contains the size of
 * instruction word in memory image (in MAUs) and the instruction encoding
 * type identifier. This information is ignored by the TUT_TTA architecture.
 * The read position of the stream is moved 4 bytes forward.
 *
 * @todo Convert assertion onnstruction encoder identifier into a test with
 *       warning or error message.
 *
 * @param stream Stream from which the field is read.
 * @param sect Target section. Unused. 
*/
void
TPEFCodeSectionReader::readInfo(
    BinaryStream& stream,
    Section*) const {

    stream.readHalfWord(); // ignored
    assert(stream.readByte() == 0); // instruction encoding ID
    stream.readByte(); // padding - should warn if nonzero
}


/**
 * Reads annotation fields and adds them to instruction.
 *
 * TODO: move to inline file...
 *
 * @param stream Stream pointing to start of annotation.
 * @param elem Instruction where read annotations are added.
 */
void
TPEFCodeSectionReader::readAnnotations(BinaryStream& stream,
                                       InstructionElement *elem) const {
    bool continuation = true;

    while (continuation) {
        Byte sizeAndContinuation = stream.readByte();

        // if continuation bit is down
        if ((sizeAndContinuation & TPEFHeaders::IANNOTE_CONTINUATION) == 0) {
            continuation = false;
        }


        Byte payloadSize = sizeAndContinuation & TPEFHeaders::IANNOTE_SIZE;

        // read three byte wide value (from big endian format)
        Word id = stream.readByte() |
            (static_cast<Word>(stream.readByte()) << (BYTE_BITWIDTH))|
            (static_cast<Word>(stream.readByte()) << (BYTE_BITWIDTH*2));

        InstructionAnnotation *newAnnotation =
            new InstructionAnnotation(id);

        for (int i = 0; i < payloadSize; i++) {
            newAnnotation->addByte(stream.readByte());
        }

        elem->addAnnotation(newAnnotation);
    }
}

}
