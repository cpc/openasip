/**
 * @file CodeSectionCreator.cc
 *
 * Definitions of CodeSectionCreator class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen@tut.fi)
 *
 * @note rating: yellow
 */

#include <deque>

#include "CodeSectionCreator.hh"

#include "MachineResourceManager.hh"
#include "LabelManager.hh"
#include "Assembler.hh"

#include "Binary.hh"
#include "Section.hh"
#include "MoveElement.hh"
#include "ImmediateElement.hh"
#include "CodeSection.hh"
#include "ResourceElement.hh"
#include "ResourceSection.hh"

#include "Machine.hh"
#include "MathTools.hh"

using namespace TPEF;

// in future this can be defined by code address space size
const UValue CodeSectionCreator::CODE_RELOC_SIZE = 32;

/**
 * Constructor.
 *
 * @param resourceManager TPEF resources and strings.
 * @param parent Assembler root class.
 */
CodeSectionCreator::CodeSectionCreator(
    MachineResourceManager &resourceManager,
    Assembler* parent) :
    resources_(resourceManager), parent_(parent),
    isNextBegin_(true), slotNumber_(0), immediateIndex_(0) {
}

/**
 * Creator to start new section from given start address.
 *
 * @param startAddress Start address of next section.
 * @exception OutOfRange Start address is not in code address space.
 */
void
CodeSectionCreator::newSection(UValue startAddress)
    throw (OutOfRange) {

    // Checks are not needed yet, since we support currently only
    // one code section.

    // @todo if multiple times then create new section each time..
    //       for now only one code section is supported

    // @todo  sanity checks for addresses and moves (next is begin...)

    internalSection_.startAddress = startAddress;
}

/**
 * Adds new move to section.
 *
 * If exception is thrown, creator will remain as it was before function call.
 *
 * @param move Parsed move.
 * @exception CompileError If referenced resource is not found from machine.
 */
void
CodeSectionCreator::addMove(const ParserMove& move)
    throw (CompileError) {

    try {
        if (move.isBegin) {
            startNewInstruction();
        }

        InternalElement* newMove = NULL;
        InternalElement* newImmediate = NULL;

        UValue srcWidth = 0, dstWidth = 0;

        switch (move.type) {

        case ParserMove::LONG_IMMEDIATE: {

            // check that source is valid
            if (move.source.isRegister) {
                CompileError error(
                    __FILE__, __LINE__, __func__,
                    "Internal error: Immediate source must be literal or "
                    "expression.");

                error.setCodeFileLineNumber(move.asmLineNumber);
                throw error;
            }

            static InternalElement newElement;
            newImmediate = &newElement;

            newElement.type = IMMEDIATE;
            newElement.asmLineNumber = move.asmLineNumber;
            newElement.isBegin =  false;

            newElement.annotationes = move.annotationes;

            // check value bitwidth
            newElement.immValue = move.source.immTerm;

            UValue tempValue = newElement.immValue.value;
            while(tempValue > 0) {
                tempValue = tempValue >> 1;
                srcWidth++;
            }

            // destination of the immediate
            MachineResourceManager::ResourceID& resID =
                resources_.resourceID(
                    move.asmLineNumber, move.destination, newElement.slot,
                    MachineResourceManager::RQST_WRITE);

            newElement.dstType  = resID.type;

            if (newElement.dstType != MoveElement::MF_IMM) {
                CompileError error(
                    __FILE__, __LINE__, __func__,
                    "Long immediate destination must be immediate unit.");

                error.setCodeFileLineNumber(move.asmLineNumber);
                throw error;
            }

            newElement.dstUnit  = resID.unit;
            newElement.dstIndex = resID.index;
            dstWidth = resID.width;

        } break;

        case ParserMove::EMPTY: {
            static InternalElement newElement;
            newMove = &newElement;

            newElement.asmLineNumber = move.asmLineNumber;
            newElement.type = EMPTY;
            newElement.isBegin = move.isBegin;
            newElement.slot = slotNumber();
        } break;

        case ParserMove::TRANSPORT: {
            static InternalElement newElement;
            newElement.asmLineNumber = move.asmLineNumber;
            newMove = &newElement;

            newElement.slot = slotNumber();
            newElement.type = MOVE;
            
            newElement.annotationes = move.annotationes;

            // source
            if (move.source.isRegister) {

                MachineResourceManager::ResourceID &resID =
                    resources_.resourceID(
                        move.asmLineNumber,
                        move.source.regTerm, newElement.slot,
                        MachineResourceManager::RQST_READ);

                newElement.srcType  = resID.type;
                newElement.srcUnit  = resID.unit;
                newElement.srcIndex = resID.index;

                srcWidth = resID.width;

            } else {
                // inline immediate
                static InternalElement immediate;
                immediate.asmLineNumber = move.asmLineNumber;
                newImmediate = &immediate;

                immediate.slot = newElement.slot;
                immediate.type = IMMEDIATE;
                immediate.dstUnit = ResourceElement::INLINE_IMM;
                immediate.dstIndex = immediateIndex();
                immediate.isBegin =  false;
                immediate.immValue = move.source.immTerm;

                newElement.srcType = MoveElement::MF_IMM;
                newElement.srcUnit = immediate.dstUnit;
                newElement.srcIndex = immediate.dstIndex;

                // check value bitwidth
                UValue tempValue = immediate.immValue.value;
                while(tempValue > 0) {
                    tempValue = tempValue >> 1;
                    srcWidth++;
                }
            }

            // destination
            MachineResourceManager::ResourceID &resID =
                resources_.resourceID(move.asmLineNumber,
                                      move.destination, newElement.slot,
                                      MachineResourceManager::RQST_WRITE);

            newElement.dstType  = resID.type;
            newElement.dstUnit  = resID.unit;
            newElement.dstIndex = resID.index;
            newElement.isBegin = move.isBegin;
            dstWidth = resID.width;

            // guard
            newElement.isGuarded = move.guard.isGuarded;

            if (newElement.isGuarded) {
                newElement.isInverted = move.guard.isInverted;

                MachineResourceManager::ResourceID *resID = NULL;

                if (newElement.isInverted) {
                    resID = &resources_.resourceID(
                        move.asmLineNumber,
                        move.guard.regTerm, newElement.slot,
                        MachineResourceManager::RQST_INVGUARD);

                } else {
                    resID = &resources_.resourceID(
                        move.asmLineNumber,
                        move.guard.regTerm, newElement.slot,
                        MachineResourceManager::RQST_GUARD);
                }

                newElement.guardType  = resID->type;
                newElement.guardUnit  = resID->unit;
                newElement.guardIndex = resID->index;
            }


            if (isDestinationAlreadyWritten(newElement)) {
                parent_->addWarning(
                    move.asmLineNumber,
                    "Move destination: " +
                    move.destination.toString() +
                    " is already written "
                    "in current instruction.");
            }

        } break;

        default:
            assert(false);
        }

        if (newMove != NULL) {
            // check that there are enough busses with sufficient width for
            // all parsed moves
            try {
                // if source is wider than bus
                if (resources_.findBusWidth(newMove->slot) < srcWidth) {

                    parent_->addWarning(
                        move.asmLineNumber,
                        "Bus width is smaller than source.");
                }

                // if source or destination is wider than bus
                if (dstWidth < srcWidth) {
                    parent_->addWarning(
                        move.asmLineNumber,
                        "Source is wider than destination.");
                }

            } catch (OutOfRange& e) {
                CompileError error(
                    __FILE__, __LINE__, __func__,
                    "Too many bus slots used.");
                error.setCodeFileLineNumber(move.asmLineNumber);
                error.setCause(e);

                throw error;
            }
            
            internalSection_.elements.push_back(*newMove);
        }

        if (newImmediate != NULL) {
            internalSection_.elements.push_back(*newImmediate);
        }

    } catch (IllegalMachine& e) {
        CompileError error(
            __FILE__, __LINE__, __func__, e.errorMessage());

        error.setCodeFileLineNumber(move.asmLineNumber);
        error.setCause(e);

        throw error;
    }
}

/**
 * Writes created sections to given binary.
 *
 * All data stored inside creator is freed after this call, unless
 * exception is thrown.
 *
 * In case of exception creator restores its state to be same that
 * state was before running finalize() (finalize() can be runned again).
 *
 * @param tpef Binary where to created sections should be added.
 * @param labels LabelManager where to add data labels and relocations.
 * @exception CompileError If there is any errors during compiling.
 */
void
CodeSectionCreator::finalize(Binary &tpef, LabelManager &labels)
    throw (CompileError) {

    // we don't have to have emty CodeSection
    if (internalSection_.elements.size() > 0) {

        CodeSection* codeSection = dynamic_cast<CodeSection*>(
            Section::createSection(Section::ST_CODE));

        assert(codeSection != NULL);

        try {
            codeSection->setLink(resources_.resourceSection());
            codeSection->setName(resources_.stringToChunk(""));

            try {
                codeSection->setASpace(resources_.codeAddressSpace());

            } catch (IllegalMachine& e) {
                CompileError error(
                    __FILE__, __LINE__, __func__,
                    "Can't find code address space.");

                error.setCause(e);

                throw error;
            }

            std::vector<ImmediateElement*> recentImmediates;

            // create the moves
            for (unsigned int i = 0;
                 i < internalSection_.elements.size(); i++) {

                InternalElement &elem = internalSection_.elements[i];

                assert(elem.slot <= 0xff);

                if (elem.isBegin) {
                    // TODO: Check that the recent long immediates are well
                    // formed, as per following conditions.

                    // A valid instruction template exists that encodes
                    // them.

                    // The same unit is never written twice.

                    // There exists a combination of all destination units
                    // needed.

                    // The fields that encode each immediate are wide enough
                    // to encode the given constant.

                    recentImmediates.clear();
                }

                switch(elem.type) {
                case EMPTY: {
                    MoveElement *emptyMove = new MoveElement();
                    emptyMove->setBegin(elem.isBegin);
                    emptyMove->setEmpty(true);
                    emptyMove->setBus(elem.slot + 1);                    
                    addAnnotationes(*emptyMove, elem, labels);
                    codeSection->addElement(emptyMove);
                } break;

                case IMMEDIATE: {
                    ImmediateElement *immElem = new ImmediateElement();
                    immElem->setBegin(elem.isBegin);

                    UValue immValue = 0;
                    if (elem.immValue.isExpression) {

                        immValue = labels.resolveExpressionValue(
                            elem.asmLineNumber, elem.immValue);

                        // MARK
                        ASpaceElement& aSpaceElement = 
                            labels.aSpaceElement(
                                elem.immValue.expression.label);

                        const std::string aSpaceName = 
                            labels.aSpaceName(elem.immValue.expression.label);

                        std::size_t relocSize = CODE_RELOC_SIZE;

                        try {
                            // figure out what is the maximum address of
                            // the referred address space and set it as
                            // the relocation width for the immediate
                            const TTAMachine::AddressSpace* addressSpace =
                                parent_->targetMachine().
                                addressSpaceNavigator().item(aSpaceName);
                            assert(addressSpace != NULL);
                            relocSize = MathTools::requiredBits(
                                static_cast<unsigned int>(
                                    addressSpace->end()));
                        } catch (const Exception& e) {
                            Application::logStream() 
                                << "Could not get access to MOM address space."
                                << std::endl;
                        }


                        labels.addRelocation(
                            *codeSection, *immElem, aSpaceElement, immValue, 
                            relocSize);

                    } else {
                        immValue = elem.immValue.value;
                    }

                    immElem->setDestinationUnit(elem.dstUnit);
                    immElem->setDestinationIndex(elem.dstIndex);
                    immElem->setWord(immValue);

                    if (immElem->destinationUnit() !=
                        ResourceElement::INLINE_IMM) {

                        recentImmediates.push_back(immElem);
                    }

                    addAnnotationes(*immElem, elem, labels);
                    codeSection->addElement(immElem);
                } break;

                case MOVE: {
                    MoveElement *newMove = new MoveElement();
                    newMove->setEmpty(false);
                    newMove->setBegin(elem.isBegin);

                    newMove->setBus(elem.slot + 1);

                    newMove->setSourceType(elem.srcType);
                    newMove->setSourceUnit(elem.srcUnit);
                    newMove->setSourceIndex(elem.srcIndex);

                    newMove->setDestinationType(elem.dstType);
                    newMove->setDestinationUnit(elem.dstUnit);
                    newMove->setDestinationIndex(elem.dstIndex);

                    newMove->setGuardType(elem.guardType);
                    newMove->setGuardUnit(elem.guardUnit);
                    newMove->setGuardIndex(elem.guardIndex);

                    newMove->setGuarded(elem.isGuarded);
                    newMove->setGuardInverted(elem.isInverted);

                    addAnnotationes(*newMove, elem, labels);
                    codeSection->addElement(newMove);
                } break;

                default:
                    assert(false);
                }
            }

        } catch (CompileError& e) {
            labels.clearLastRelocations();
            delete codeSection;
            throw e;
        }

        labels.commitLastRelocations();
        tpef.addSection(codeSection);
    }

    cleanup();
}

/**
 * Frees all internally allocated data.
 */
void
CodeSectionCreator::cleanup() {
    immediateIndex_ = 0;
    isNextBegin_ = true;
    slotNumber_ = 0;
}

/**
 * Inits privat attributes for start of new instruction.
 */
void
CodeSectionCreator::startNewInstruction() {
    slotNumber_ = 0;
    immediateIndex_ = 0;
}

/**
 * Returns slot number for currently added move.
 *
 * @return Slot number for currently added move.
 */
UValue
CodeSectionCreator::slotNumber() {
    slotNumber_++;
    return slotNumber_ - 1;
}

/**
 * Returns next possible index for inline immediate.
 *
 * @return Next possible index for inline immediate.
 */
UValue
CodeSectionCreator::immediateIndex() {
    immediateIndex_++;
    return immediateIndex_;
}

/**
 * Returns true if RF index or FU port is written twice in a same instruction.
 *
 * @param elem Move to check.
 * @return True if RF index or FU port is written twice in a same instruction.
 */
bool
CodeSectionCreator::isDestinationAlreadyWritten(
    const InternalElement& elem) const {

    if (elem.type == MOVE && !elem.isBegin) {

        std::vector<const InternalElement*> guardedMoves;

        for (int i = internalSection_.elements.size() - 1;
             i >= 0 && !internalSection_.elements[i].isBegin; i--) {

            const InternalElement &compare = internalSection_.elements[i];

            if (compare.type == elem.type &&
                compare.dstType == elem.dstType &&
                compare.dstUnit == elem.dstUnit &&
                compare.dstIndex == elem.dstIndex) {

                if (!compare.isGuarded || !elem.isGuarded) {
                    // destination is same and only one of the moves 
                    // is  guarded
                    return true;

                } else {

                    if (compare.guardType == elem.guardType &&
                        compare.guardUnit == elem.guardUnit &&
                        compare.guardIndex == elem.guardIndex &&
                        compare.isInverted == elem.isInverted) {

                        // same destination and same guards
                        return true;
                    }

                    // gather possibly colliding moves
                    guardedMoves.push_back(&compare);
                }
            }

        }

        // check if possibly colliding moves has two opposites of same guard
        for (unsigned int i = 0; i < guardedMoves.size(); i++) {
            for (unsigned int j = i + 1; j < guardedMoves.size(); j++) {

                const InternalElement* iMove = guardedMoves[i];
                const InternalElement* jMove = guardedMoves[j];

                if (iMove->guardType == jMove->guardType &&
                    iMove->guardUnit == jMove->guardUnit &&
                    iMove->guardIndex == jMove->guardIndex &&
                    iMove->isInverted != jMove->isInverted) {

                    // destination that is currently written is
                    // definately already written in this instruction
                    return true;
                }
            }
        }
    }

    return false;
}


/**
 * Adds annotations of element of internal presentation to TPEF element.
 *
 * @param instrElem TPEF InstructionElement where to add annotation data.
 * @param elem Element of internal presentation that caontains annotationes 
 *             from assembly code. 
 * @exception CompileError If defined value needs more room that is defined
 *                         in init data.
 */
void 
CodeSectionCreator::addAnnotationes(InstructionElement& instrElem,
                                   InternalElement& elem, LabelManager& labels) const {

    for (unsigned int i = 0; i < elem.annotationes.size();i++) {
        Word id = elem.annotationes[i].id;
        std::vector<Byte> payload;
        
        for (unsigned int j = 0; j < elem.annotationes[i].payload.size(); j++) {
            InitDataField& initData = elem.annotationes[i].payload[j];
            std::deque<Byte> temp;
            
            UValue value = 0;
            if (initData.litOrExpr.isExpression) {
                value = labels.resolveExpressionValue(elem.asmLineNumber, 
                                                      initData.litOrExpr);
            } else {
                value = initData.litOrExpr.value;
            }
            
            // write value to field
            for (int k = sizeof(value) - 1; k >= 0; k--) {
                Byte nextVal = (value >> (k*BYTE_BITWIDTH));
                temp.push_back(nextVal);               
            }
            
            // remove extra leading ones if signed
            if (initData.litOrExpr.isSigned) {
                while (temp.size() > 1 && temp[0] == 0xff && 
                       temp.size() > initData.width &&
                       (temp[1] & 0x80) != 0) {

                    temp.pop_front();
                }                
            }

            // remove extra leading zeroes
            while (temp[0] == 0 && temp.size() > initData.width) {
                temp.pop_front();
            }
            
            // add needed leading zeroes or ones if signed            
            while (temp.size() < initData.width) {

                if (initData.litOrExpr.isSigned && (temp[0] & 0x80) != 0) {
                    temp.push_front(0xff);
                } else {
                    temp.push_front(0);
                }

            }
                                    
            if (initData.width != 0 && temp.size() > initData.width) {
                std::string errorMessage = 
                    "Annotation payload " + initData.toString() + 
                    " is too big for defined field size.";

                throw CompileError(
                    __FILE__, __LINE__, __func__,
                    errorMessage);
            }
            
            // add to payload
            for (unsigned int j = 0; j < temp.size(); j++) {
                payload.push_back(temp[j]);
            }
        }
                      
        instrElem.addAnnotation(new InstructionAnnotation(id, payload));
    }
}
