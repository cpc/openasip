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
 * @file TPEFCodeSectionWriter.cc
 *
 * Definition of TPEFCodeSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <list>

#include "TPEFCodeSectionWriter.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "Section.hh"
#include "SectionElement.hh"
#include "MoveElement.hh"
#include "ImmediateElement.hh"
#include "SectionSizeReplacer.hh"
#include "CodeSection.hh"
#include "BinaryStream.hh"

namespace TPEF {

using std::list;
using ReferenceManager::SafePointer;
using ReferenceManager::FileOffsetKey;
using ReferenceManager::SectionKey;
using ReferenceManager::SectionOffsetKey;

const TPEFCodeSectionWriter TPEFCodeSectionWriter::instance_;
const Byte TPEFCodeSectionWriter::IMMEDIATE_VALUE_MAX_BYTES = 16;

/**
 * Constructor.
 *
 * Registers itself to SectionWriter.
 */
TPEFCodeSectionWriter::TPEFCodeSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFCodeSectionWriter::~TPEFCodeSectionWriter() {
}

/**
 * Returns the type of section that this writer writes.
 *
 * @return The type of section that this writer writes.
 */
Section::SectionType
TPEFCodeSectionWriter::type() const {
    return Section::ST_CODE;
}

/**
 * Writes the data of the section to stream.
 *
 * @param stream The stream to be written to.
 * @param sect The section to be written.
 */
void
TPEFCodeSectionWriter::actualWriteData(
    BinaryStream& stream,
    const Section* sect) const {

    // file offset to data of section
    FileOffset startOffset = stream.writePosition();

    SectionOffset sectOffset = 0;

    SectionKey sKey = SafePointer::sectionKeyFor(sect);
    SectionId id = sKey.sectionId();

    for (Word i = 0; i < sect->elementCount(); i++) {
        InstructionElement *elem =
            dynamic_cast<InstructionElement*>(sect->element(i));

        assert(elem != NULL);

        sectOffset = stream.writePosition() - startOffset;

        if (SafePointer::isReferenced(elem) || i == 0) {
            SafePointer::addObjectReference(
                SectionOffsetKey(id, sectOffset), elem);
        }

        // check if this is ending move of instruction
        bool isEnd = false;
        i++;
        if (i == sect->elementCount() ||
            dynamic_cast<InstructionElement*>(sect->element(i))->begin()) {

            isEnd = true;
        }
        i--;

        writeAttributeField(stream, elem, isEnd);
        writeDataField(stream, elem);
        writeAnnotations(stream, elem);
    }

    // section body offset
    SafePointer::addObjectReference(
        FileOffsetKey(startOffset), sect->element(0));

    SectionSizeReplacer::setSize(sect, stream.writePosition() - startOffset);
}

/**
 * Writes the attribute field of either move or immediate.
 *
 * @param stream The stream to be written to.
 * @param elem The element to be written out.
 */
void
TPEFCodeSectionWriter::writeAttributeField(
    BinaryStream& stream,
    SectionElement* elem,
    bool isEnd) const {

    Byte attribute = 0;

    if (dynamic_cast<InstructionElement*>(elem)->isMove()) {

        MoveElement* move = dynamic_cast<MoveElement*>(elem);

        if (move->isGuarded()) {
            attribute = attribute | TPEFHeaders::IA_MGUARD;
        }

        if (move->isEmpty()) {
            attribute = attribute | TPEFHeaders::IA_EMPTY;
        }

    } else if (dynamic_cast<InstructionElement*>(elem)->isImmediate()) {

        ImmediateElement* imm = dynamic_cast<ImmediateElement*>(elem);

        // immediate type flag
        attribute = attribute |  TPEFHeaders::IA_TYPE;

        // length of immediate is stored in 4 first bits of byte.
        Byte immLength = imm->length();
        Byte immBits = (immLength << (BYTE_BITWIDTH / 2));

        attribute = attribute | immBits;

    } else {
        bool impossibleInstructionElementType = false;
        assert(impossibleInstructionElementType);
    }

    if (dynamic_cast<InstructionElement*>(elem)->annotationCount() > 0) {
        attribute = attribute |  TPEFHeaders::IA_ANNOTE;
    }

    if (isEnd) {
        attribute = attribute |  TPEFHeaders::IA_END;
    }

    stream.writeByte(attribute);
}

/**
 * Writes the data portion of either move or immediate element.
 *
 * @param stream The stream to be written to.
 * @param elem The element to be written.
 */
void
TPEFCodeSectionWriter::writeDataField(
    BinaryStream& stream,
    SectionElement* elem) const {

    if (dynamic_cast<InstructionElement*>(elem)->isMove()) {
        MoveElement* move = dynamic_cast<MoveElement*>(elem);

        writeId(stream, move->bus());

        Byte fieldTypes = 0;

        if (!move->isEmpty()) {

            switch(move->sourceType()) {
            case MoveElement::MF_RF:
                fieldTypes |= TPEFHeaders::MVS_RF;
                break;
            case MoveElement::MF_IMM:
                fieldTypes |= TPEFHeaders::MVS_IMM;
                break;
            case MoveElement::MF_UNIT:
                fieldTypes |= TPEFHeaders::MVS_UNIT;
                break;
            default:
                std::cerr << "move source type: " << std::hex
                          << (int)move->sourceType() << std::dec << std::endl;
                assert(false);
            }

            switch(move->destinationType()) {
            case MoveElement::MF_RF:
                fieldTypes |= TPEFHeaders::MVD_RF;
                break;
            case MoveElement::MF_UNIT:
                fieldTypes |= TPEFHeaders::MVD_UNIT;
                break;
            default:
                std::cerr << "move dest type: "
                          << (int)move->destinationType()
                          << "\tdest unit: "
                          << (int)move->destinationUnit()
                          << "\tindex: "
                          << (int)move->destinationIndex()
                          << std::endl;

                assert(false);
            }

            if (move->isGuarded()) {
                switch(move->guardType()) {
                case MoveElement::MF_RF:
                    fieldTypes |= TPEFHeaders::MVG_RF;
                    break;
                case MoveElement::MF_UNIT:
                    fieldTypes |= TPEFHeaders::MVG_UNIT;
                    break;
                default:
                    std::cerr << "move guarde type: " << std::hex
                              << (int)move->guardType() 
                              << std::dec << std::endl;
                    assert(false);
                }

                if (move->isGuardInverted() != 0) {
                    fieldTypes |= TPEFHeaders::IE_GUARD_INV_MASK;

                }
            }
        }

        stream.writeByte(fieldTypes);

        writeId(stream, move->sourceUnit());
        stream.writeHalfWord(move->sourceIndex());

        writeId(stream, move->destinationUnit());
        stream.writeHalfWord(move->destinationIndex());

        writeId(stream, move->guardUnit());
        stream.writeHalfWord(move->guardIndex());

    } else {
        ImmediateElement* imm = dynamic_cast<ImmediateElement*>(elem);
        Byte dstUnit  = imm->destinationUnit();
        Byte dstIndex = imm->destinationIndex();

        stream.writeByte(dstUnit);
        stream.writeByte(dstIndex);

        assert(imm->length() <= IMMEDIATE_VALUE_MAX_BYTES);

        for (unsigned int i = 0; i < imm->length(); i++) {
            stream.writeByte(imm->byte(i));
        }
    }
}

/**
 * Writes annotations to stream if there are any.
 *
 * @param stream Stream to write.
 * @param elem Element, whose annotations are written.
 */
void
TPEFCodeSectionWriter::writeAnnotations(
    BinaryStream& stream,
    SectionElement* elem) const {

    InstructionElement *instr = dynamic_cast<InstructionElement*>(elem);

    for (Word i = 0; i < instr->annotationCount(); i++) {

        InstructionAnnotation* annotation = instr->annotation(i);

        Byte sizeAndContinuation =
            annotation->size() & TPEFHeaders::IANNOTE_SIZE;

        // if not last annotation
        if (i != (instr->annotationCount() - 1)) {
            sizeAndContinuation =
                sizeAndContinuation | TPEFHeaders::IANNOTE_CONTINUATION;
        }

        stream.writeByte(sizeAndContinuation);

        // write identification code in big endian format
        // NOTE: should there be function in
        //       BinaryStream to write 3 byte "word"s?
        stream.writeByte(annotation->id() & 0xff);
        stream.writeByte((annotation->id() >> (BYTE_BITWIDTH)) & 0xff);
        stream.writeByte((annotation->id() >> (BYTE_BITWIDTH*2)) & 0xff);

        // write payload
        for (Word j = 0; j < annotation->size(); j++) {
            stream.writeByte(annotation->byte(j));
        }
    }
}


/**
 * Writes the `info' field of code section header.
 *
 * The `info' field of code sections normally contains information related
 * to instruction size and encoding. The TUT_TTA architecture does not use
 * this field and ignores its contents.
 *
 * @param stream Stream to which the data is written.
 * @param sect Input section. Unused.
 */
void
TPEFCodeSectionWriter::writeInfo(
    BinaryStream& stream,
    const Section*) const {

    stream.writeHalfWord(0); // unused field
    stream.writeByte(0); // unused field - must be set to zero!
    stream.writeByte(0); // padding
}


/**
 * Writes Bus, FU or RF id according to TPEF version.
 *
 * Original TPEF version 1 supports only less than 256 buses, FUs and RFs.
 * Version 2 fixes that issue and we need to check the stream version for proper
 * amount of bytes to write.
 *
 * @param stream Stream to which the data is written.
 * @param sect id of the component.
 */
void
TPEFCodeSectionWriter::writeId(BinaryStream& stream, HalfWord id) const {

    TPEFHeaders::TPEFVersion version = stream.TPEFVersion();

    if (version == TPEFHeaders::TPEFVersion::TPEF_V1) {
        stream.writeByte(id);
    } else {
        stream.writeHalfWord(id);
    }
}

} // namespace TPEF
