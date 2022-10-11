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
 * @file TPEFDisassembler.cc
 *
 * Implementation of TPEFDisassembler class.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <map>

#include "TPEFDisassembler.hh"
#include "Binary.hh"
#include "CodeSection.hh"

#include "DisassemblyInstruction.hh"
#include "DisassemblyImmediate.hh"
#include "DisassemblyMove.hh"
#include "DisassemblyIntRegister.hh"
#include "DisassemblyBoolRegister.hh"
#include "DisassemblyFPRegister.hh"
#include "DisassemblyOperand.hh"
#include "DisassemblyGuard.hh"
#include "DisassemblyFUPort.hh"
#include "DisassemblyFUOpcodePort.hh"
#include "DisassemblyRegister.hh"
#include "DisassemblyImmediateRegister.hh"
#include "DisassemblyImmediateAssignment.hh"
#include "DisassemblyReturnAddressRegister.hh"
#include "DisassemblyNOP.hh"
#include "DisassemblyFUOperand.hh"
#include "MoveElement.hh"
#include "ImmediateElement.hh"

#include "ResourceElement.hh"
#include "ResourceSection.hh"
#include "StringSection.hh"
#include "SimValue.hh"

using namespace TPEF;

/**
 * Constructor.
 *
 * @param aTpef TPEF hierarchy whose code sections are disassembled.
 */
TPEFDisassembler::TPEFDisassembler(const Binary &aTpef) :
    tpef_(&aTpef) {
}

/**
 * Destructor.
 */
TPEFDisassembler::~TPEFDisassembler() {
}

/**
 * Creates dynamically allocated disassembler instruction.
 *
 * @param instructionIndex Number of instruction to return.
 * @return Dynamically allocated disassembler instruction.
 */
DisassemblyInstruction*
TPEFDisassembler::createInstruction(Word instructionIndex) const {

    CodeSection *theCodeSection =
        dynamic_cast<CodeSection*>(tpef_->section(Section::ST_CODE,0));

    assert(theCodeSection != NULL);

    if (instructionStartCache_.empty()) {
        initCache();
    }

    Word currIndex = instructionStartCache_[instructionIndex];

    InstructionElement *currElement =
        dynamic_cast<InstructionElement*>(
            theCodeSection->element(currIndex));

    DisassemblyInstruction *newInstruction = new DisassemblyInstruction();

    // scan all elements in first pass, because we want to have also
    // immediates when we build instructions
    ImmediateMap immediates;
    std::vector<MoveElement*> moves;

    do {
        if (currElement->isMove()) {
            MoveElement *move = dynamic_cast<MoveElement*>(currElement);
            assert (move != NULL);
            moves.push_back(move);

        } else if (currElement->isImmediate()) {
            ImmediateElement *imm =
                dynamic_cast<ImmediateElement*>(currElement);

            assert (imm != NULL);

            if (imm->isInline()) {
                ImmediateKey
                    immKey(imm->destinationUnit(), imm->destinationIndex());

                immediates[immKey] = imm;

            } else {
                // this is long immediate
                DisassemblyElement* dest =
                    createDisassemblyElement(MoveElement::MF_IMM,
                                             imm->destinationUnit(),
                                             imm->destinationIndex(),
                                             immediates);
                
                SimValue immValue(64);
                immValue = imm->longWord();

                // do not know if unit is signed or not without adf
                DisassemblyImmediateAssignment* immAssign = 
                    new DisassemblyImmediateAssignment(immValue, false, dest);
                               
                // add annotationes for the move
                for (Word j = 0; j < imm->annotationCount(); j++) {
                    InstructionAnnotation& ann = *(imm->annotation(j));
                    immAssign->addAnnotation(
                        new DisassemblyAnnotation(ann.id(), ann.payload()));
                }
                
                newInstruction->addLongImmediate(immAssign);
            }

        } else {
            assert(false);
        }

        currIndex++;

        // if we reached end of section
        if (currIndex == theCodeSection->elementCount()) {
            break;
        }

        currElement = dynamic_cast<InstructionElement*>(
            theCodeSection->element(currIndex));

    } while (!currElement->begin());

    // find out how many busses tpef have.
    ResourceSection* resources = dynamic_cast<ResourceSection*>(
        tpef_->section(Section::ST_MR, 0));

    assert(resources != NULL);

    std::vector<DisassemblyInstructionSlot*> organizedInstr;

    for (Word i = 0; i < resources->elementCount(); i++) {
        ResourceElement* resource =
            dynamic_cast<ResourceElement*>(resources->element(i));

        if (resource->type() == ResourceElement::MRT_BUS &&
            resource->id() != ResourceElement::UNIVERSAL_BUS) {

            organizedInstr.push_back(NULL);
        }
    }

    for (unsigned int i = 0; i < moves.size(); i++) {
        MoveElement *currMove = moves[i];

        if (!currMove->isEmpty()) {
            DisassemblyElement *source =
                createDisassemblyElement(currMove->sourceType(),
                                         currMove->sourceUnit(),
                                         currMove->sourceIndex(),
                                         immediates);

            DisassemblyElement *destination =
                createDisassemblyElement(currMove->destinationType(),
                                         currMove->destinationUnit(),
                                         currMove->destinationIndex(),
                                         immediates);

            DisassemblyGuard *guard = NULL;

            if (currMove->isGuarded()) {
                DisassemblyElement *guardElement =
                    createDisassemblyElement(currMove->guardType(),
                                             currMove->guardUnit(),
                                             currMove->guardIndex(),
                                             immediates);

                guard = new DisassemblyGuard(
                    guardElement, currMove->isGuardInverted());
            }

            DisassemblyMove *newMove =
                new DisassemblyMove(source, destination, guard);

            // add annotationes for the move
            for (Word j = 0; j < currMove->annotationCount(); j++) {
                InstructionAnnotation& ann = *(currMove->annotation(j));
                newMove->addAnnotation(
                    new DisassemblyAnnotation(ann.id(), ann.payload()));
            }

            // add unassigned moves to end of instruction
            if (currMove->bus() > 0) {
                organizedInstr[currMove->bus() - 1] = newMove;
            } else {
                organizedInstr.push_back(newMove);
            }
        } else {
            // empty move element. NOP?
            // add annotationes for the move
            for (Word j = 0; j < currMove->annotationCount(); j++) {
                InstructionAnnotation& ann = *(currMove->annotation(j));
                newInstruction->addAnnotation(
                    new DisassemblyAnnotation(ann.id(), ann.payload()));
            }
        }
    }

    // create all moves and add them to disassembler instruction
    for (Word i = 0; i < organizedInstr.size(); i++) {

        if (organizedInstr[i] != NULL) {
            newInstruction->addMove(organizedInstr[i]);

        } else {
            DisassemblyNOP *newNOP = new DisassemblyNOP();
            newInstruction->addMove(newNOP);
        }
    }

    return newInstruction;
}

/**
 * Address of first instruction.
 *
 * @return Address of first instruction.
 */
Word
TPEFDisassembler::startAddress() const {
    CodeSection *theCodeSection =
	dynamic_cast<CodeSection*>(tpef_->section(Section::ST_CODE,0));

    assert(theCodeSection != NULL);
    return theCodeSection->startingAddress();
}

/**
 * Number of instructions available in first code section of TPEF.
 *
 * @return Number of instruction available.
 */
Word
TPEFDisassembler::instructionCount() const {
    if (instructionStartCache_.empty()) {
	initCache();
    }
    return instructionStartCache_.size();
}

/**
 * Clears internal cache of instruction start indexes in TPEF
 * code section.
 *
 * If you modify order or begin flags of instruction elements in TPEF,
 * chache should be cleared, because instruction start indexes might be
 * changed.
 */
void
TPEFDisassembler::clearCache() const {
    instructionStartCache_.clear();
}

/**
 * Initilises instruction start indexes in code section to vector.
 *
 * After cache init we have vector of all instruction start indexes of the
 * section.
 */
void
TPEFDisassembler::initCache() const {
    CodeSection *theCodeSection =
	dynamic_cast<CodeSection*>(tpef_->section(Section::ST_CODE,0));

    assert(theCodeSection != NULL);

    for (Word i = 0; i < theCodeSection->elementCount(); i++) {
        if (dynamic_cast<InstructionElement*>(
                theCodeSection->element(i))->begin()) {
            // if i is begin of instruction push it to instruction start vector
            instructionStartCache_.push_back(i);
        }
    }
}

/**
 * Creates DisassemblyElement out of given paramters.
 *
 * Finds needed strings and stuff from TPEF hierarchy.
 *
 * @param type Type of elemet to create.
 * @param unit Unit id of referred resource.
 * @param index Index part of referred resource.
 * @param immediateMap All immediate values found in instruction.
 * @return Dynamically allocated fully contructed DisassemblerElement.
 */
DisassemblyElement*
TPEFDisassembler::createDisassemblyElement(
    MoveElement::FieldType type, Word unit, Word index,
    ImmediateMap &immediateMap) const {

    ResourceSection *resources =
        dynamic_cast<ResourceSection*>(tpef_->section(Section::ST_MR,0));

    assert(resources != NULL);

    StringSection *strings =
        dynamic_cast<StringSection*>(resources->link());

    switch (type) {

    case MoveElement::MF_RF: {
        if (unit & ResourceElement::UNIVERSAL_RF_MASK) {
            switch (unit) {
            case ResourceElement::INT_RF:
                return  new DisassemblyIntRegister(index);
            case ResourceElement::BOOL_RF:
                // only one boolean register for now
                assert(index == 0);
                return  new DisassemblyBoolRegister();
            case ResourceElement::FP_RF:
                return  new DisassemblyFPRegister(index);
            default:
                // Error: not universal unit
                assert(false);
            }
        } else {
            // real resources from real machines
            ResourceElement &rfUnit =
                resources->findResource(ResourceElement::MRT_RF,unit);

            std::string rfName = strings->chunk2String(rfUnit.name());

            return new DisassemblyRegister(rfName, index);
        }

    } break;

    case MoveElement::MF_IMM: {
        ImmediateKey  immKey(unit, index);

        if (MapTools::containsKey(immediateMap, immKey)) {

            ImmediateElement *imm =
                MapTools::valueForKey<ImmediateElement*>(immediateMap, immKey);

            assert(imm->isInline());

            SimValue immValue(64);
            immValue = imm->sLongWord();
            return new DisassemblyImmediate(immValue, false);

        } else {
            // register reference to immediate register.

            // get that immediate unit
            ResourceElement &immUnit =
                resources->findResource(ResourceElement::MRT_IMM, unit);

            std::string immUnitName = strings->chunk2String(immUnit.name());

            return new DisassemblyImmediateRegister(immUnitName, index);
        }

    } break;

    case MoveElement::MF_UNIT: {

        if (unit == ResourceElement::UNIVERSAL_FU) {

            // TODO refactor

            if (resources->hasResource(
                    ResourceElement::MRT_OP, index)) {

                ResourceElement resource =
                    resources->findResource(
                        ResourceElement::MRT_OP, index);

                std::string opString  =
                    strings->chunk2String(resource.name());

                std::string::size_type dotIndex = opString.rfind('.');
                std::string opName = opString.substr(0,dotIndex);

                // move dotIndex to start of regnumber part of operation
                dotIndex++;

                std::string opIndexStr = opString.substr(
                    dotIndex, opString.length() - dotIndex);

                Word opIndex = Conversion::toInt(opIndexStr);

                return new DisassemblyOperand(opName, opIndex);

            } else if (resources->hasResource(
                           ResourceElement::MRT_SR, index)) {

                ResourceElement &resource =
                    resources->findResource(
                        ResourceElement::MRT_SR, index);

                std::string regName = strings->chunk2String(resource.name());

                if (regName == ResourceElement::RETURN_ADDRESS_NAME) {
                    return new DisassemblyReturnAddressRegister();

                } else {
                    abortWithError("Unknown special register");
                }

            } else {
                abortWithError(
		    "Can't find universal operand or special register "
		    "resource by index:" + Conversion::toString(index));
            }

        } else {
            // terminal types fuName.portName or fuName.opName.index
	    // not universal FU, which has no fuName field

            // get FU name
            ResourceElement &fuResource =
                resources->findResource(ResourceElement::MRT_UNIT, unit);

            std::string fuName = strings->chunk2String(fuResource.name());

            // TODO refactor
            if (resources->hasResource(
                    ResourceElement::MRT_PORT, index)) {

                ResourceElement &fuPort =
                    resources->findResource(ResourceElement::MRT_PORT, index);

                std::string fuPortName =
                    strings->chunk2String(fuPort.name());

                return new DisassemblyFUPort(fuName, fuPortName);

            } else if (resources->hasResource(
                           ResourceElement::MRT_OP, index)) {

                ResourceElement &fuOperationOperand =
                    resources->findResource(ResourceElement::MRT_OP, index);

                std::string operandString =
                    strings->chunk2String(fuOperationOperand.name());

                std::string::size_type dotPos = operandString.rfind(".");

                assert(dotPos != std::string::npos);

                std::string opName = operandString.substr(0, dotPos);

                std::string operandIndexString =
                    operandString.substr(
                        dotPos+1, operandString.length() - dotPos - 1);

                Word operandIndex =
		    Conversion::toUnsignedInt(operandIndexString);

                return new DisassemblyFUOperand(fuName, opName, operandIndex);

            } else if (resources->hasResource(
                           ResourceElement::MRT_SR, index)) {

                ResourceElement &fuSpecialRegister =
                    resources->findResource(ResourceElement::MRT_SR, index);

                std::string srName =
                    strings->chunk2String(fuSpecialRegister.name());

                return new DisassemblyFUPort(fuName, srName);

            } else {
                abortWithError(
		    "Can't find real port, operation or special register by "
		    "index: " + Conversion::toString(index));
            }
        }

    } break;

    default:
        abortWithError("Unknown type: " + Conversion::toString((int)type));
    }

    assert(false);
    return NULL;
}
