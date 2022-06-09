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
 * @file AOutTextSectionReader.cc
 *
 * Definition of AOutTextSectionReader class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 23 October 2003 by pj, am, ll, jn
 *
 * @note rating: yellow
 */

#include "AOutTextSectionReader.hh"
#include "SafePointer.hh"
#include "AOutSymbolSectionReader.hh"
#include "ReferenceKey.hh"
#include "SectionReader.hh"
#include "Conversion.hh"
#include "ResourceSection.hh"
#include "MoveElement.hh"
#include "ImmediateElement.hh"

namespace TPEF {

using std::string;
using std::vector;
using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::SectionIndexKey;

const int AOutTextSectionReader::OFFSET_TO_IMMEDIATE_VALUE = 4;
AOutTextSectionReader AOutTextSectionReader::proto_;


/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
AOutTextSectionReader::AOutTextSectionReader() : AOutSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
AOutTextSectionReader::~AOutTextSectionReader() {
}

/**
 * Returns section type which reader can read.
 *
 * @return The type of the section that reader can read.
 */
Section::SectionType
AOutTextSectionReader::type() const {
    return Section::ST_CODE;
}

/**
 * Reads all Moves from stream and stores them in to Section.
 *
 * One move consists of 4 fields: guard (Byte), immediate (Byte),
 * destination index (HalfWord), and source index (Word). If immediate
 * is 1 then the source field of the move contains an immediate value.
 *
 * @param stream Stream to be read from.
 * @param section Pointer to section in which information is stored.
 * @exception UnreachableStream If reading of section fails.
 * @exception KeyAlreadyExists Key was in use when trying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some of read values were out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue If there was unexpected value when reading.
 */
void
AOutTextSectionReader::readData(BinaryStream& stream, Section* section) const {
    AOutReader* aOutReader = dynamic_cast<AOutReader*>(parent());

    FileOffset offset = stream.readPosition();
    Word length = aOutReader->header().sectionSizeText();

    while (stream.readPosition() < offset + length) {

        SectionOffset sectionOffset = stream.readPosition() - offset;

        // we must first discover whether instruction is immediate or not
        Byte guard = stream.readByte();
        Byte imm = stream.readByte();

        MoveElement* move = new MoveElement();
        ImmediateElement* immediate = NULL;

        // guard register is the only boolean register of a.out binaries
        assert((guard == 0) || (guard == '!') || (guard == '?'));
        move->setGuardUnit(ResourceElement::BOOL_RF);
        move->setGuardIndex(0);
        move->setGuardType(MoveElement::MF_RF);
        move->setGuarded(guard != 0);
        move->setGuardInverted(guard == '!');

        // always use universal bus
        move->setBus(ResourceElement::UNIVERSAL_BUS);

        if (imm == 1) {
            immediate = new ImmediateElement();
            initializeImmediateMove(stream, move, immediate);
            immediate->setBegin(true);
            section->addElement(immediate);
            section->addElement(move);

            // set reference pointing immediate, since move is never
            // referenced
            setReference(immediate, sectionOffset, AOutReader::ST_TEXT);

        } else {
            initializeMove(stream, move);
            move->setBegin(true);
            section->addElement(move);
            setReference(move, sectionOffset, AOutReader::ST_TEXT);
        }
    }
}

/**
 * The attributes of move that has immediate in its source field are
 * initialized.
 *
 * Immediate value of a move is modeled as a separate object.
 *
 * @param stream Stream to be read from.
 * @param move Move to be initialized
 * @param immediate Immediate to be initialized.
 * @exception OutOfRange If destination register index is too large.
 * @exception UnreachableStream If there occurs problems with stream.
 */
void
AOutTextSectionReader::initializeImmediateMove(
    BinaryStream& stream, MoveElement* move,
    ImmediateElement* immediate) const {
    move->setSourceType(MoveElement::MF_IMM);

    // mark every immediate to be inline encoded
    immediate->setDestinationUnit(ResourceElement::INLINE_IMM);
    immediate->setDestinationIndex(0);

    // mark move source to match immediate
    move->setSourceUnit(ResourceElement::INLINE_IMM);
    move->setSourceIndex(0);

    HalfWord dest = stream.readHalfWord();
    updateMoveDestination(move, dest);

    immediate->setWord(stream.readWord());
}

/**
 * Initializes an 'ordinary' Move.
 *
 * @param stream Stream to be read from.
 * @param move Move to be initialized.
 * @exception OutOfRange If destination or source register index is too big.
 * @exception UnreachableStream If there occurs problems with stream.
 */
void
AOutTextSectionReader::initializeMove(
    BinaryStream& stream, MoveElement* move) const {
    HalfWord dest = stream.readHalfWord();
    updateMoveDestination(move, dest);

    Word src = stream.readWord();
    updateMoveSource(move, src);
}

/**
 * Convert an a.out register index into a TPEF register index.
 *
 * Service helper method.  The callers are responsible for ensuring that the
 * given register index is within the allowed range.
 *
 * @param reg A.out index of the register.
 * @return The TPEF index of the same register.
 */
Word
AOutTextSectionReader::convertAOutIndexToTPEF(Word reg) const {

    // NOTE: This implementation depends on AOutSymbolSectionReader
    //       implementation  (Where resource section is read).

    // offset to first index of given register class
    int regOffset = 0;

    // int reg
    if (reg < AOutReader::FIRST_FP_REGISTER) {
        regOffset = AOutReader::FIRST_INT_REGISTER;

    // fp reg
    } else if (reg < AOutReader::FIRST_BOOL_REGISTER) {
        regOffset = AOutReader::FIRST_FP_REGISTER;

    // bool register
    } else if (reg < AOutReader::FIRST_FU_REGISTER) {
        regOffset = AOutReader::FIRST_BOOL_REGISTER;

    // fu or special register
    } else {
        regOffset = 0;
    }

    return reg - regOffset;
}

/**
 * Convert an a.out register index to a TPEF register index and update the
 * destination of the given move to reflect the given register index.
 *
 * Possible values of move destination field are indices of: integer
 * registers, floating point registers, Boolean registers, or function unit
 * registers. The type of the move destination is also set.
 *
 * @param move The Move to be updated.
 * @param dest Index of the destination register.
 * @exception OutOfRange If destination index is too large.
 */
void
AOutTextSectionReader::updateMoveDestination(
    MoveElement* move, const HalfWord dest) const {
    if (dest < AOutReader::FIRST_FP_REGISTER) {
        move->setDestinationUnit(ResourceElement::INT_RF);
        move->setDestinationType(MoveElement::MF_RF);

    } else if (dest < AOutReader::FIRST_BOOL_REGISTER) {
        move->setDestinationUnit(ResourceElement::FP_RF);
        move->setDestinationType(MoveElement::MF_RF);

    } else if (dest < AOutReader::FIRST_FU_REGISTER) {
        move->setDestinationUnit(ResourceElement::BOOL_RF);
        move->setDestinationType(MoveElement::MF_RF);

    } else {
        // for special registers move type fields is same that for fu
        move->setDestinationUnit(ResourceElement::UNIVERSAL_FU);
        move->setDestinationType(MoveElement::MF_UNIT);
    }

    move->setDestinationIndex(convertAOutIndexToTPEF(dest));
}

/**
 * Convert an a.out register index to a TPEF register index and update the
 * source of the given move to reflect the given register index.
 *
 * Possible values of move source field are indices of: integer registers,
 * floating point registers, Boolean registers, or function unit
 * registers. The type of the move source is also set.
 *
 * @param move The Move to be updated.
 * @param src Index of the source register.
 * @exception OutOfRange If source index is too large.
 */
void
AOutTextSectionReader::updateMoveSource(
    MoveElement* move, const Word src) const {
    if (src < AOutReader::FIRST_FP_REGISTER) {
        move->setSourceUnit(ResourceElement::INT_RF);
        move->setSourceType(MoveElement::MF_RF);

    } else if (src < AOutReader::FIRST_BOOL_REGISTER) {
        move->setSourceUnit(ResourceElement::FP_RF);
        move->setSourceType(MoveElement::MF_RF);

    } else if (src < AOutReader::FIRST_FU_REGISTER) {
        move->setSourceUnit(ResourceElement::BOOL_RF);
        move->setSourceType(MoveElement::MF_RF);

    } else {
        // for special registers move type fields is same that for fu
        move->setSourceUnit(ResourceElement::UNIVERSAL_FU);
        move->setSourceType(MoveElement::MF_UNIT);
    }

    move->setSourceIndex(convertAOutIndexToTPEF(src));
}

/**
 * Sets references to ReferenceManager to enable referencing if
 * it's needed later.
 *
 * @param elem Element to be registered to ReferenceManager.
 * @param sectionOffset The offset of the element in the section.
 * @param sectionID Identification code of the referenced section.
 * @exception KeyAlreadyExists If registration fails because of existing key.
 */
void
AOutTextSectionReader::setReference(
    InstructionElement* elem, SectionOffset sectionOffset,
    SectionId sectionID) const {
    SectionOffsetKey offKey = SectionOffsetKey(sectionID, sectionOffset);
    SafePointer::addObjectReference(offKey, elem);
}
}
