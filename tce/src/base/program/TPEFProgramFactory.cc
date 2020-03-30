/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file TPEFProgramFactory.cc
 *
 * Implementation of TPEFProgramFactory class.
 *
 * @author Mikael Lepist� 2005 (tmlepist-no.spam-cs.tut.fi)
 * @author Pekka J��skel�inen 2011
 * @note rating: yellow
 */

#include <list>
#include <map>
#include <vector>
#include <boost/format.hpp>

#include "TPEFProgramFactory.hh"
#include "ContainerTools.hh"
#include "ASpaceElement.hh"
#include "Section.hh"
#include "SymbolSection.hh"
#include "CodeSymElement.hh"
#include "CodeSection.hh"
#include "DataSymElement.hh"
#include "ImmediateElement.hh"
#include "Instruction.hh"
#include "Procedure.hh"
#include "MoveGuard.hh"
#include "Move.hh"
#include "UnboundedRegisterFile.hh"
#include "UniversalFunctionUnit.hh"
#include "TerminalRegister.hh"
#include "TerminalFUPort.hh"
#include "TerminalImmediate.hh"
#include "OperationPool.hh"
#include "FUPort.hh"
#include "Guard.hh"
#include "HWOperation.hh"
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"
#include "Application.hh"
#include "CodeLabel.hh"
#include "DataLabel.hh"
#include "RelocElement.hh"
#include "AddressSpace.hh"
#include "InstructionReferenceManager.hh"
#include "TerminalInstructionReference.hh"
#include "TerminalAddress.hh"
#include "InstructionReference.hh"
#include "RelocSection.hh"
#include "ProcedSymElement.hh"
#include "Immediate.hh"
#include "NullInstructionTemplate.hh"
#include "DataMemory.hh"
#include "DataDefinition.hh"
#include "DataAddressDef.hh"
#include "DataInstructionAddressDef.hh"
#include "ProgramAnnotation.hh"
#include "BinaryStream.hh"
#include "BinaryReader.hh"
#include "MathTools.hh"
#include "GlobalScope.hh"
#include "UniversalMachine.hh"
#include "Program.hh"

using namespace TTAMachine;
using namespace TPEF;
using std::string;

namespace TTAProgram {

/**
 * Little helper class for storing information of function start points.
 */
class FunctionStart {
public:
    FunctionStart(std::string aName) :
        name_(aName) { }

    std::string name() const {return name_;}

private:
    std::string name_;
};

/**
 * Constructor.
 *
 * For mixed code which contains universal machine references and
 * target machine references.
 *
 * @param aBinary Binary that contains a program's instructions and data.
 * @param aMachine Actual machine to which parallel code refers.
 * @param aUniversalMachine Universal machine for sequential code.
 * @param relocs Managed collection of relocation points in the program.
 */
TPEFProgramFactory::TPEFProgramFactory(
    const Binary& aBinary,
    const Machine& aMachine,
    UniversalMachine*):
    binary_(&aBinary), machine_(&aMachine),
    universalMachine_(&UniversalMachine::instance()),
    tpefTools_(aBinary),
    adfInstrASpace_(NULL),
    tpefInstrASpace_(NULL) {
}

/**
 * Constructor for fully scheduled code. 
 *
 * @param aBinary Binary that contains a program's instructions and data.
 * @param aMachine Machine to which code refers.
 * @param relocs Managed collection of relocation points in the program.
 */
TPEFProgramFactory::TPEFProgramFactory(
    const Binary& aBinary, const Machine& aMachine):
    binary_(&aBinary), machine_(&aMachine),
    universalMachine_(&UniversalMachine::instance()),
    tpefTools_(aBinary),
    adfInstrASpace_(NULL),
    tpefInstrASpace_(NULL) {
}

/**
 * Constructor for fully unscheduled code. 
 *
 * @param aBinary Binary that contains a program's instructions and data.
 * @param uMachine Universal Machine to which code refers.
 * @param relocs Managed collection of relocation points in the program.
 */
TPEFProgramFactory::TPEFProgramFactory(
    const Binary &aBinary, UniversalMachine*):
    binary_(&aBinary), machine_(NULL),
    universalMachine_(&UniversalMachine::instance()),
    tpefTools_(aBinary),
    adfInstrASpace_(NULL),
    tpefInstrASpace_(NULL) {
}


/**
 * Destructor.
 */
TPEFProgramFactory::~TPEFProgramFactory() {
    MapTools::deleteAllValues(functionStartPositions_);
}

/**
 * Returns value of chunk as string.
 *
 * Helps getting names for various TPEF resources.
 *
 * @param chunk Chunk referring to section.
 * @param chunkOwner StringSection that contains requested string.
 * @return String referred by chunk.
 */
std::string
TPEFProgramFactory::stringOfChunk(
    const Chunk* chunk,
    const Section* chunkOwner) const {

    const StringSection* strSect =
        dynamic_cast<const StringSection*>(chunkOwner);
    assert(strSect != NULL);
    return strSect->chunk2String(chunk);
}

/**
 * Builds program model out of TPEF model.
 *
 * @return Created program.
 * @exception NotAvailable if there the binary contains no sections; if the
 *     instruction address space is missing or conflicting with architecture
 *     definition.
 * @exception Exception if the TPEF or program in it is somehow broken.
 */
Program*
TPEFProgramFactory::build() {
    assert(machine_ != NULL || universalMachine_ != NULL);

    if (binary_->sectionCount(Section::ST_CODE) == 0) {
        throw NotAvailable(
            __FILE__, __LINE__, __func__,
            "No code sections in TPEF.");
    }

    tpefInstrASpace_ =
        binary_->section(Section::ST_CODE, 0)->aSpace();

    // get address space for program from machine depending on
    // type of code (sequential, partially scheduled, fully scheduled)
    adfInstrASpace_ = NULL;

    if (machine_ == NULL) {
        adfInstrASpace_ = &universalMachine_->instructionAddressSpace();
        
    } else {
    
        if (binary_->type() == Binary::FT_OBJSEQ ||
            binary_->type() == Binary::FT_PURESEQ ||
            binary_->type() == Binary::FT_LIBSEQ) {
                throw NotAvailable(
                    __FILE__, __LINE__, __func__,
                    "Tried to load a sequential program with ADF already "
                    "loaded.");
            }
        
        // check if real machine has address space by defined name.
        // if not found then use universal address space.
        Machine::AddressSpaceNavigator aSpaceNav =
            machine_->addressSpaceNavigator();

        std::string aSpaceName = stringOfChunk(
            tpefInstrASpace_->name(),
            binary_->section(Section::ST_ADDRSP, 0)->link());

        if (aSpaceNav.hasItem(aSpaceName )) {
            adfInstrASpace_ = aSpaceNav.item(aSpaceName);

        } else {
            if (universalMachine_ == NULL) {
                throw NotAvailable(
                    __FILE__, __LINE__, __func__,
                    "No instruction(gcu) address space in ADF.");
            }

            adfInstrASpace_ = &universalMachine_->instructionAddressSpace();
        }
    }
    
    if (machine_ == NULL && 
        binary_->type() == Binary::FT_PARALLEL) {
        throw NotAvailable(
            __FILE__, __LINE__, __func__,
            "Tried to load a parallel TPEF without ADF.");
    }

    assert(adfInstrASpace_ != NULL);

    // ignored for backwards compatibility
    //     if (tpefInstrASpace_->MAU() != 0) {
    //         throw NotAvailable(
    //             __FILE__, __LINE__, __func__,
    //             (boost::format(
    //                 "TPEF instruction address space MAU size should be (%d).") %
    //                 static_cast<int>(tpefInstrASpace_->MAU())).str());
    //     }

    
    clearCache();

    seekFunctionStartPoints();

    Program* newProgram = new Program(*adfInstrASpace_);
    newProgram->setUniversalMachine(universalMachine_);

    // create all the code
    addProcedures(*newProgram, *adfInstrASpace_);

    TTAProgram::Program::InstructionVector allInstructions =
        newProgram->instructionVector();
    const InstructionAddress startAddress =
        newProgram->startAddress().location();
        
    // fix TerminalAddresses pointing to instructions to be
    // TerminalInstructionAddresses.
    while (!instructionImmediates_.empty()) {
        std::shared_ptr<Move> move = *instructionImmediates_.begin();
        instructionImmediates_.erase(instructionImmediates_.begin());

        Terminal &addressTerm = move->source();

        assert(&(addressTerm.address().space()) == adfInstrASpace_);

        Instruction& referencedInstruction =
            *allInstructions.at(addressTerm.address().location() - startAddress);

        InstructionReference instructionReference =
            newProgram->instructionReferenceManager().createReference(
                referencedInstruction);

        TerminalInstructionReference* instrTerm =
            new TerminalInstructionReference(instructionReference);

        move->setSource(instrTerm);
    }

    // and same for long immediates which refers to instruction addresses
    while (!longInstructionImmediates_.empty()) {
        auto immediate = *longInstructionImmediates_.begin();
        longInstructionImmediates_.erase(longInstructionImmediates_.begin());

        TerminalImmediate &addressTerm = immediate->value();

        assert(&(addressTerm.address().space()) == adfInstrASpace_);

        Instruction& referencedInstruction =
            *allInstructions.at(addressTerm.address().location() - startAddress);

        InstructionReference instructionReference =
            newProgram->instructionReferenceManager().createReference(
                referencedInstruction);

        TerminalInstructionReference* instrTerm =
            new TerminalInstructionReference(instructionReference);

        immediate->setValue(instrTerm);
    }

    createDataMemories(*newProgram);
    createLabels(*newProgram);

    return newProgram;
}

/**
 * Parses procedures from all TPEF CodeSections and adds them to Program.
 *
 * @param program Program where to add new procedures.
 * @param programASpace Address space of instruction memory.
 */
 void
 TPEFProgramFactory::addProcedures(
     Program& program,
     const AddressSpace& programASpace) const {

     // find code sections to chop and organize them by start address
     std::list<CodeSection*> sectionsToChop;

     for (Word i = 0; i < binary_->sectionCount(Section::ST_CODE); i++) {

         CodeSection* sectionToAdd =
             dynamic_cast<CodeSection*>(
                 binary_->section(Section::ST_CODE, i));

         if (sectionsToChop.empty()) {
             sectionsToChop.push_back(sectionToAdd);
             continue;
         }

         std::list<CodeSection*>::iterator iter = sectionsToChop.begin();

         while (iter != sectionsToChop.end()) {

             if ((*iter)->startingAddress() >
                 sectionToAdd->startingAddress()) {
                 sectionsToChop.insert(iter, sectionToAdd);
                 break;
             }

             iter++;
         }
     }

     // NOTE: maybe it should be checked if found code sections are legal.
     //       (adressSpaces and addresses does not collide)

     // add instruction elements of every found section
     std::list<CodeSection*>::iterator sectionIterator =
         sectionsToChop.begin();

     while (sectionIterator != sectionsToChop.end()) {
         CodeSection* section = *sectionIterator;

         ResourceSection* resources =
             dynamic_cast<ResourceSection*>(section->link());
         assert(resources != NULL);

         Word i = 0;
         int currentInstructionNumber = 0;
         while (i < section->elementCount()) {

             try {
                 // Create and add a new procedure to program with name if
                 // new procedure is started by current instruction or if
                 // there is no procedures in program.

                 SectionElement* element = section->element(i);
                 InstructionElement* instructionElement =
                     dynamic_cast<InstructionElement*>(element);
                 assert(instructionElement != NULL);

                 if (isFunctionStart(*instructionElement) ||
                     program.procedureCount() == 0) {

                     assert(instructionElement->begin());

                     // TODO: set the real start address.. ?

                     Procedure* newProcedure = new Procedure(
                         functionName(*instructionElement),
                         programASpace, 0);

                     program.addProcedure(newProcedure);
                 }

                 // scan instruction elements of next instruction

                 // moves of instruction
                 MoveVector moveElements;
                 // slots that encode immediate bits of instruction
                 ImmediateVector longImmediates;
                 // inline immediates of instruction
                 ImmediateMap immElements;

                 InstructionElement* beginElement = NULL;

                 do {
                     if (instructionElement->begin()) {
                         beginElement = instructionElement;
                     }

                     if (instructionElement->isMove()) {
                         moveElements.push_back(
                             dynamic_cast<MoveElement*>(instructionElement));

                     } else if (instructionElement->isImmediate()) {
                         ImmediateElement* imm =
                             dynamic_cast<ImmediateElement*>(
                                 instructionElement);

                         if (imm->isInline()) {
                             std::pair<Word,Word> 
                                 immKey(imm->destinationUnit(),
                                        imm->destinationIndex());
                             immElements[immKey] = imm;
                         } else {
                             longImmediates.push_back(imm);
                         }
                     } else {
                         abortWithError("Unknown instruction element type.");
                     }

                     i++;
                     if (i >= section->elementCount()) {
                         break;
                     }

                     SectionElement* sectionElement = section->element(i);
                     assert(sectionElement != NULL);

                     instructionElement =
                         dynamic_cast<InstructionElement*>(sectionElement);

                     assert(i < section->elementCount());

                 } while (instructionElement->begin() == false);

                 Instruction* currentInstruction =
                     createInstruction(*resources, moveElements,
                                       longImmediates, immElements);

                 // add created instruction to map for finding program
                 // instruction by tpef instruction element
                 assert(beginElement != NULL);

                 instructionMap_[beginElement] = currentInstruction;

                 assert(currentInstruction != NULL);

                 program.addInstruction(currentInstruction);
                 currentInstructionNumber++;

             } catch (const Exception& e) {
                 // add instruction number to start of exception message
                 NotAvailable error(
                     __FILE__, __LINE__, __func__,
                     (boost::format(
                         "Instruction %d: ") % currentInstructionNumber).
                     str() + e.errorMessage());
                 error.setCause(e);

                 throw error;
             }
         }

         sectionIterator++;
     }
 }

/**
 * Creates an instruction out of given moves and immediate elements.
 *
 * @param resources TPEF resource section.
 * @param moveElements Move elements of instruction.
 * @param longImemdiates Long immediates of instruction.
 * @param immElements Immediate elements of instruction.
 * @return A new instruction.
 */
Instruction*
TPEFProgramFactory::createInstruction(
    const TPEF::ResourceSection& resources,
    MoveVector& moveElements,
    ImmediateVector& longImmediates,
    ImmediateMap& immElements) const {

    std::vector<SocketAllocation> allocatedSockets;

    Instruction* newInstruction =
        new Instruction(NullInstructionTemplate::instance());

    for (unsigned int i = 0; i < moveElements.size(); i++) {
        MoveElement* move = moveElements[i];

        // NOTE: we just ignore empty moves
        if (!move->isEmpty()) {
            std::shared_ptr<Move> newMove = NULL;
            Terminal* source = NULL;
            Terminal* destination = NULL;
            Terminal* guardRegister = NULL;
            MoveGuard* guard = NULL;

            // get bus
            Bus& bus = findBus(resources, move->bus());

            try {
                // create source terminal
                source = createTerminal(
                    resources, &bus, Socket::OUTPUT, move->sourceType(),
                    move->sourceUnit(), move->sourceIndex(), &immElements);

                // and destination terminal
                destination = createTerminal(
                    resources, &bus, Socket::INPUT, move->destinationType(),
                    move->destinationUnit(), move->destinationIndex());

                // create guard if move is guarded
                if (move->isGuarded()) {

                    Guard &adfGuard = findGuard(
                        resources,
                        bus,
                        move->guardType(),
                        move->guardUnit(),
                        move->guardIndex(),
                        move->isGuardInverted());

                    guard = new MoveGuard(adfGuard);

                    assert(guard != NULL);
                }

            } catch (const NotAvailable& e) {

                if (guard != NULL) {
                    delete guard;
                    guard = NULL;

                } else {
                    if (guardRegister != NULL) {
                        delete guardRegister;
                        guardRegister = NULL;
                    }
                }

                if (source != NULL) {
                    delete source;
                    source = NULL;
                }

                if (destination != NULL) {
                    delete destination;
                    destination = NULL;
                }

                delete newInstruction;
                newInstruction = NULL;

                throw e;
            } catch (Exception& e) {
                throw e;
            }

            if (guard != NULL) {
                newMove = std::make_shared<Move>(source, destination, bus, guard);
            } else {
                newMove = std::make_shared<Move>(source, destination, bus);
            }
            assert(newMove != NULL);

            // Add possible sockets for register references.
            Machine::SocketNavigator socketNav =
                bus.machine()->socketNavigator();

            SocketAllocation newAlloc(newMove, allocatedSockets.size());

            // find possible sockets for move source and destination
            for (int i = 0; i < socketNav.count(); i++) {
                Socket* currSocket = socketNav.item(i);

                if (currSocket->isConnectedTo(bus)) {
                    for (int j = 0; j < currSocket->portCount(); j++) {

                        if ((source->isGPR() ||
                             source->isImmediateRegister()) &&
                            currSocket->direction() == Socket::OUTPUT &&
                            currSocket->port(j)->parentUnit() ==
                            source->port().parentUnit()) {

                            newAlloc.srcSocks.push_back(currSocket);
                        }

                        if ((destination->isGPR() ||
                             destination->isImmediateRegister()) &&
                            currSocket->direction() == Socket::INPUT &&
                            currSocket->port(j)->parentUnit() ==
                            destination->port().parentUnit()) {

                            newAlloc.dstSocks.push_back(currSocket);
                        }
                    }
                }
            }

            allocatedSockets.push_back(newAlloc);

            // add move annotations
            if (move->annotationCount() > 0) {
                for (Word annotationIndex = 0; 
                     annotationIndex < move->annotationCount(); 
                     ++annotationIndex) {
                    newMove->addAnnotation(
                        ProgramAnnotation(
                            static_cast<ProgramAnnotation::Id>(
                                move->annotation(annotationIndex)->id()),
                            move->annotation(annotationIndex)->payload()));
                }
            }

            newInstruction->addMove(newMove);

            if (newMove->source().isAddress() &&
                &(newMove->source().address().space()) == adfInstrASpace_) {
                instructionImmediates_.push_back(newMove);
            }
        }
    }

    // get template
    InstructionTemplate& instrTemplate =
        findInstrTemplate(resources, longImmediates, moveElements);

    newInstruction->setInstructionTemplate(instrTemplate);

    // and add long immediates
    for (unsigned int i = 0; i < longImmediates.size(); i++) {
        ImmediateElement* imm = longImmediates[i];
        Byte iUnitID = imm->destinationUnit();

        Terminal* destination = createTerminal(
            resources, NULL, Socket::INPUT,
            MoveElement::MF_IMM, iUnitID, imm->destinationIndex());

        ImmediateUnit& immUnit(findImmediateUnit(resources, iUnitID));
        SimValue simVal(instrTemplate.supportedWidth(immUnit));
        if (immUnit.signExtends()) {
            simVal = imm->signedWord();
        } else {
            simVal = imm->word();
        }

        TerminalImmediate* immTerm = NULL;

        // TODO: refactor with createTerminal method's line 57
        bool isInstructionReference = false;

        if (tpefTools_.hasRelocation(*imm)) {
            const RelocElement &reloc = tpefTools_.relocation(*imm);

            // check if instruction address space
            if (tpefInstrASpace_ == reloc.aSpace()) {
                // create temporary TerminalAddress and add to vector
                // for late replacement
                immTerm = new TerminalAddress(simVal, *adfInstrASpace_);

                isInstructionReference = true;

            } else {
                AddressSpace& adfDataSpace =
                    findAddressSpace(reloc.aSpace());
                immTerm =
                    new TerminalAddress(simVal, adfDataSpace);
            }

        } else {
            immTerm = new TerminalImmediate(simVal);
        }

        auto newImmediate = std::make_shared<Immediate>(immTerm, destination);

        newInstruction->addImmediate(newImmediate);

        if (isInstructionReference) {
            longInstructionImmediates_.push_back(newImmediate);
        }
    }

    resolveSocketAllocations(allocatedSockets);
    return newInstruction;
}

/**
 * Creates a move terminal of the appropriate type for given input data.
 *
 * This method *cannot* be used for creating terminal for immediate unit
 * or register file registers. 
 * 
 *
 * @param resources TPEF resource section.
 * @param aBus Bus to which terminal is connected.
 * @param direction Read or write terminal.
 * @param type TPEF type of terminal.
 * @param unitId TPEF identification code of the unit to which terminal
 *        belongs.
 * @param index Register or operation terminal index, or immediate
 *        identifier.
 * @param immediateMap All immediates of currently created instruction.
 * @return A new move terminal.
 */
Terminal*
TPEFProgramFactory::createTerminal(
    const ResourceSection& resources, const Bus* aBus,
    Socket::Direction direction, MoveElement::FieldType type, Byte unitId,
    HalfWord index, const ImmediateMap* immediateMap) const {

    // omit caching because RF and IMM unit ports are resolved later
    if (type == MoveElement::MF_RF) {
        // port(0) just a dummy temporary assignment, it may even be illegal
        RegisterFile& registerFile = findRegisterFile(resources, unitId);
        return new TerminalRegister(*registerFile.port(0), index);

    } else if (type == MoveElement::MF_IMM &&
               unitId != ResourceElement::INLINE_IMM) {

        // port(0) just a dummy temporary assignment, it may even be illegal
        ImmediateUnit& immUnit = findImmediateUnit(resources, unitId);
        return new TerminalRegister(*immUnit.port(0), index);
    }

    CacheKey cacheKey(*aBus, direction, type, unitId, index);

    Terminal* returnValue = getFromCache(cacheKey);

    if (returnValue == NULL) {

        switch (type) {

        case MoveElement::MF_IMM: {
            ImmediateKey immKey(unitId, index);
            ImmediateElement* imm = NULL;

            if (MapTools::containsKey(*immediateMap, immKey)) {
                imm = MapTools::valueForKey<ImmediateElement*>(
                    *immediateMap, immKey);
            } else {
                Application::logStream()
                    << "Cannot find immediate with unitId/index "
                    << static_cast<int>(unitId) << "/" << index << std::endl
                    << "immediateMap.size(): " << immediateMap->size()
                    << std::endl;
                Application::abortProgram();
            }

            if (imm->isInline()) {

                int immWidth = aBus->immediateWidth();
                SimValue simValue(immWidth);
                simValue = imm->word();
                                
                // TODO: refactor with line createInstruction 
                //       method's line 158
                if (tpefTools_.hasRelocation(*imm)) {
                    const RelocElement &reloc = tpefTools_.relocation(*imm);

                    // check if instruction address space
                    if (tpefInstrASpace_ == reloc.aSpace()) {
                        // create temporary TerminalAddress and add to vector
                        // for late replacement
                        returnValue =
                            new TerminalAddress(simValue, *adfInstrASpace_);

                    } else {
                        try {
                            AddressSpace& adfDataSpace =
                                findAddressSpace(reloc.aSpace());
                            returnValue =
                                new TerminalAddress(simValue, adfDataSpace);
                        } catch (const NotAvailable& e) {
                            NotAvailable newException(
                                __FILE__, __LINE__, __func__, 
                                (boost::format(
                                    "Unable to find address space for "
                                    "target of reloc element for immediate "
                                    "'%d'.") % imm->word()).
                                str());
                            newException.setCause(e);
                            throw newException;                        
                        }                        
                    }

                } else {
                    returnValue = new TerminalImmediate(simValue);
                }
                TTAProgram::TerminalImmediate* retVal =
                    dynamic_cast<TerminalImmediate*>(returnValue);
                assert(returnValue != NULL);
                // add immediate annotations
                if (imm->annotationCount() > 0) {
                    for (Word annotationIndex = 0; 
                         annotationIndex < imm->annotationCount(); 
                         ++annotationIndex) {
                        retVal->addAnnotation(
                            ProgramAnnotation(
                                static_cast<ProgramAnnotation::Id>(
                                    imm->annotation(annotationIndex)->id()),
                                imm->annotation(annotationIndex)->payload()));
                    }
                }
                
            } else {
                abortWithError("Error: immediate register references "
                               "should be already handled in same place "
                               "with normal register references.");
            }
        } break;

        case MoveElement::MF_UNIT: {
            ResourceElement* tpefResource = NULL;

            // TODO refactor
            if (resources.hasResource(
                    ResourceElement::MRT_OP, index)) {

                tpefResource =
                    &resources.findResource(ResourceElement::MRT_OP, index);

            } else if (resources.hasResource(
                           ResourceElement::MRT_PORT, index)) {

                tpefResource =
                    &resources.findResource(ResourceElement::MRT_PORT, index);

            } else if (resources.hasResource(
                           ResourceElement::MRT_SR, index)) {

                tpefResource =
                    &resources.findResource(ResourceElement::MRT_SR, index);

            } else {
                abortWithError("Can't find resource port, operation or "
                               "special register with index:" +
                               Conversion::toString(index));

            }

            assert(tpefResource != NULL);

            std::string tpefOpStr =
                stringOfChunk(tpefResource->name(), resources.link());

            // TODO:
            // I need only unit.port and unit.operation.index parsing since
            // unit.port.opcode references are converted to
            // unit.operation.index form in tpef

            // NOTE:
            // for now parser can chop just add.1 and sub.3 etc. kind of
            // strings.

            // start of hack parser
            bool opCodePort = false;
            std::string::size_type opNameLength = tpefOpStr.rfind('.');
            std::string tpefOpName = tpefOpStr;
            int tpefOpIndex = 0;

            if (opNameLength != std::string::npos) {
                tpefOpName = tpefOpStr.substr(0, opNameLength);
                std::string::size_type opIndexStart = opNameLength + 1;
                std::string tpefOpIndexStr =
                    tpefOpStr.substr(
                        opIndexStart, tpefOpStr.length() - opIndexStart);
                tpefOpIndex = Conversion::toInt(tpefOpIndexStr);
                opCodePort = true;
            }
            // end of hack parser

            // returns normal fu or universal fu or universal gcu..
            FunctionUnit& functionUnit =
                findFunctionUnit(resources, unitId, tpefOpName);

            if (opCodePort) {
                // find HWOperation....
                HWOperation& oper = *functionUnit.operation(tpefOpName);
                returnValue = new TerminalFUPort(oper, tpefOpIndex);

            } else {
                // special register or plain port reference
                BaseFUPort& port = dynamic_cast<BaseFUPort&>(
                    findPort(*aBus, functionUnit, tpefOpName));

                returnValue = new TerminalFUPort(port);
            }
        } break;

        default: {
            abortWithError("Unknown move field type!");
        }

        }

        // immediates are not cached...
        if (type != MoveElement::MF_IMM) {
            addToCache(cacheKey, returnValue);
        }

    } else {
        return returnValue->copy();
    }

    return returnValue;
}

/**
 * Returns the bus of real or universal machine with the given bus
 * identification number, as found in TPEF.
 *
 * @param resources The resource section where the bus should be searched.
 * @param busId Id of requested bus.
 * @return Requested bus.
 * @exception NotAvailable if requested bus does not belong to the
 *     target architecture.
 */
Bus&
TPEFProgramFactory::findBus(
    const ResourceSection &resources,
    Byte busId) const {

    if (busId == ResourceElement::UNIVERSAL_BUS) {
        if (universalMachine_ == NULL) {
            throw NotAvailable(
                __FILE__, __LINE__, __func__,
                "TPEF needs universal machine for universal bus reference.");
        }

        return universalMachine_->universalBus();

    } else {
        if (machine_ == NULL) {
            throw NotAvailable(
                __FILE__, __LINE__, __func__,
                "TPEF needs real machine for non-universal bus reference.");
        }

        // internal error with TPEF...
        assert(resources.hasResource(ResourceElement::MRT_BUS, busId));

        ResourceElement &tpefBus =
            resources.findResource(ResourceElement::MRT_BUS, busId);

        std::string busName =
            stringOfChunk(tpefBus.name(), resources.link());

        if (!machine_->busNavigator().hasItem(busName)) {
            throw NotAvailable(
                __FILE__, __LINE__, __func__,
                "ADF does not contain bus: " + busName);
        }

        return *(machine_->busNavigator().item(busName));
    }
}

/**
 * Returns RegisterFile by TPEF id number.
 *
 * @param resources The resource section where the bus should be searched.
 * @param rfId Id of requested register file.
 * @return Register file from real or universal machine.
 * @exception NotAvailable if requested register file does not belong to the
 *     target architecture.
 */
RegisterFile&
TPEFProgramFactory::findRegisterFile(
    const ResourceSection& resources,
    Byte rfId) const {

    switch (rfId) {

    case ResourceElement::ILLEGAL_RF:
        abortWithError("Illegal registerfile ID!");

    case ResourceElement::INT_RF:
        if (universalMachine_ == NULL) {
            throw NotAvailable(
                __FILE__, __LINE__, __func__,
                "TPEF needs universal machine for universal integer RF "
                "reference.");
        }

        return universalMachine_->integerRegisterFile();

    case ResourceElement::BOOL_RF:
        if (universalMachine_ == NULL) {
            throw NotAvailable(
                __FILE__, __LINE__, __func__,
                "TPEF needs universal machine for universal boolean RF "
                "reference.");
        }

        return universalMachine_->booleanRegisterFile();

    case ResourceElement::FP_RF:
        if (universalMachine_ == NULL) {
            throw NotAvailable(
                __FILE__, __LINE__, __func__,
                "TPEF needs universal machine for universal floating point "
                "RF reference.");
        }

        return universalMachine_->doubleRegisterFile();

    default: {

        if (machine_ == NULL) {
            throw NotAvailable(
                __FILE__, __LINE__, __func__,
                "TPEF needs real machine for non-universal RF reference.");
        }

        ResourceElement &tpefRF =
            resources.findResource(ResourceElement::MRT_RF, rfId);

        std::string rfName =
            stringOfChunk(tpefRF.name(), resources.link());

        if (!machine_->registerFileNavigator().hasItem(rfName)) {
            throw NotAvailable(
                __FILE__, __LINE__, __func__,
                "Can't  find RF \"" + rfName + "\" from ADF.");
        }

        return *(machine_->registerFileNavigator().item(rfName));
    }

    }

    abortWithError("This line should never be executed!");
    return universalMachine_->integerRegisterFile();
}

/**
 * Returns ImmediateUnit by TPEF id number.
 *
 * @param resources The resource section where the bus should be searched.
 * @param immUnitId Id of requested immediate unit.
 * @return Immediate unit.
 * @exception NotAvailable if requested immediate unit does not belong to the
 *     target architecture.
 */
ImmediateUnit&
TPEFProgramFactory::findImmediateUnit(
    const ResourceSection& resources,
    Byte immUnitId) const {

    if (machine_ == NULL) {
        throw NotAvailable(
            __FILE__, __LINE__, __func__,
            "TPEF needs real machine for immediate unit reference.");
    }

    ResourceElement &tpefImmUnit =
        resources.findResource(ResourceElement::MRT_IMM, immUnitId);

    std::string immUnitName =
        stringOfChunk(tpefImmUnit.name(), resources.link());

    if (!machine_->immediateUnitNavigator().hasItem(immUnitName)) {
        throw NotAvailable(
            __FILE__, __LINE__, __func__,
            "Can't  find immediate unit \"" + immUnitName + "\" from ADF.");
    }

    return *(machine_->immediateUnitNavigator().item(immUnitName));
}

/**
 * Returns FunctionUnit by TPEF identification number and operation name.
 *
 * If function unit ID corresponds to the universal function unit, then
 * operation name is checked to see if we should return universal gcu or
 * universal fu.
 *
 * If operation name is special register of function unit or if operation
 * name is operation of gcu then universal gcu is returned.
 *
 * @param resources The resource section where the bus should be searched.
 * @param unitId Id of requested unit.
 * @param tpefOpName Name of operation or special register for
 *                   universal machine resources.
 * @return Function unit from real or universal machine.
 * @exception NotAvailable if requested function unit does not belong to the
 *     target architecture.
 */
FunctionUnit&
TPEFProgramFactory::findFunctionUnit(
    const ResourceSection& resources,
    Byte unitId,
    std::string tpefOpName) const {

    switch (unitId) {

    case ResourceElement::UNIVERSAL_FU: {

        if (universalMachine_ == NULL) {
            throw NotAvailable(
                __FILE__, __LINE__, __func__,
                "TPEF needs universal machine for getting universal FU.");
        }

        FunctionUnit* gcu = universalMachine_->controlUnit();

        assert(gcu != NULL);

        if (tpefOpName == ResourceElement::RETURN_ADDRESS_NAME) {
            return *gcu;
        }

        FunctionUnit* fu  = &universalMachine_->universalFunctionUnit();
        assert(fu != NULL);

        if (gcu->hasOperation(tpefOpName)) {
            return *gcu;

        } else if (fu->hasOperation(tpefOpName)) {
            return *fu;

        } else {
            boost::format errMsg(
                "Unknown operation '%1%'. Operation definition not found.");
            throw NotAvailable(
                __FILE__, __LINE__, __func__, (errMsg % tpefOpName).str());
        }
    }

    default: {

        if (machine_ == NULL) {
            throw NotAvailable(__FILE__, __LINE__, __func__,
                "TPEF needs real target architecture for getting a "
                "non-universal FU.");
        }

        ResourceElement &tpefFU = resources.findResource(
	    ResourceElement::MRT_UNIT, unitId);

        std::string fuName =
            stringOfChunk(tpefFU.name(), resources.link());

        if (machine_->functionUnitNavigator().hasItem(fuName)) {
            return *(machine_->functionUnitNavigator().item(fuName));

        } else {

            // maybe it's GCU ..
            if (machine_->controlUnit()->name() == fuName) {
                return *machine_->controlUnit();

            } else {
                throw NotAvailable(
                    __FILE__, __LINE__, __func__,
                    "Can't  find RFU \"" + fuName + "\" from ADF.");
            }
        }
    }

    }

    abortWithError("This line should never be run!");
    return universalMachine_->universalFunctionUnit();
}

/**
 * Finds any function unit or special register port from target architecture
 * (possiby the universal machine).
 *
 * NOTE: this is currently used only for plain port references
 * (sr or port without opcode) if TPEFProgramFactory. However 
 * method should be able finding also function unit ports.
 *
 * @param bus Bus where to socket is connected.
 * @param portParent Parent to which port in connected to.
 * @param tpefOpName Name of operation if opcode port to find.
 * @param tpefOpIndex Terminal index of operation if opcode port to find.
 * @return Found port.
 * @exception NotAvailable if requested port does not belong to the target
 *     architecture.
 */
Port&
TPEFProgramFactory::findPort(
    const Bus& bus,
    const Unit& portParent,
    std::string tpefOpName,
    int tpefOpIndex) const {

    const Machine* machineOfBus = NULL;

    if (universalMachine_ != NULL &&
        &bus == &universalMachine_->universalBus()) {
        machineOfBus = universalMachine_;
    } else {
        machineOfBus = machine_;
    }

    assert(machineOfBus != NULL);

    // check if it is function unit port to find
    const FunctionUnit* fu =
        dynamic_cast<const FunctionUnit*>(&portParent);

    if (fu != NULL) {

        // check if known special register (for sequential code)
        if (tpefOpName == ResourceElement::RETURN_ADDRESS_NAME) {

            const ControlUnit &controlUnit =
                dynamic_cast<const ControlUnit&>(portParent);

            if (controlUnit.hasReturnAddressPort()) {
                return *(controlUnit.returnAddressPort());

            } else {
                throw NotAvailable(
                    __FILE__, __LINE__, __func__,
                    "GCU needs return address port.");
            }

        } else {
            // function unit is either gcu or real fu

            // if operation index is valid and opname is found from FU
            // then we know that it's port with opcode given
            if (fu->hasOperation(tpefOpName) && tpefOpIndex != 0) {
                // must be operation port
                HWOperation* oper = fu->operation(tpefOpName);
                return *(oper->port(tpefOpIndex));

            } else if (fu->hasPort(tpefOpName)) {
                // must be special plain port reference
                TTAMachine::SpecialRegisterPort* srPort =
                    dynamic_cast<TTAMachine::SpecialRegisterPort*>(
                        fu->port(tpefOpName));

                if (srPort != NULL) {
                    // NOTE: only known specialregister ports are allowed!
                    //       add asserts for special registers here :)
                    assert(srPort == 
                           machineOfBus->controlUnit()->returnAddressPort());
                    return *srPort;
                } else {
                    return *fu->port(tpefOpName);
                }

            } else {
                throw NotAvailable(
                    __FILE__, __LINE__, __func__, "Can't find port for: " +
                    fu->name() + "." + tpefOpName + "." +
                    Conversion::toString(tpefOpIndex));
            }
        }
    }

    // didn't seem to be fu port... this is not used for normal 
    // registers anymore...

    std::string throwError = "Can't find port for: " + portParent.name();

    if (tpefOpName != "") {
        throwError += "." + tpefOpName;
    }

    throwError += "." + Conversion::toString(tpefOpIndex);

    throw NotAvailable(__FILE__, __LINE__, __func__,throwError);
}

/**
 * Finds address space by name.
 *
 * @param Name of address space.
 * @return Address space.
 * @exception NotAvailable if requested address space does not belong to
 *     the target architecture.
 */
TTAMachine::AddressSpace&
TPEFProgramFactory::findAddressSpace(const ASpaceElement* aSpace) const {

    std::string aSpaceName = tpefTools_.addressSpaceName(*aSpace);

    if (machine_ != NULL) {
        Machine::AddressSpaceNavigator aSpaceNavi =
            machine_->addressSpaceNavigator();

        if (aSpaceNavi.hasItem(aSpaceName)) {
            return *aSpaceNavi.item(aSpaceName);
        }
    }

    if (universalMachine_ != NULL) {
        if (aSpaceName == universalMachine_->dataAddressSpace().name()) {
            return universalMachine_->dataAddressSpace();
        } else if (aSpaceName == 
                   universalMachine_->instructionAddressSpace().name()) {
            return universalMachine_->instructionAddressSpace();
            
        }
    }

    throw NotAvailable(
        __FILE__, __LINE__, __func__,
        "Can't find address space by name: " + aSpaceName +
        " MAU: " + Conversion::toString(static_cast<int>(aSpace->MAU())));
}

/**
 * Finds guard of bus.
 * 
 * @param resources TPEF resource section.
 * @param bus Bus of guard.
 * @param type Is fu or register guard.
 * @param unitId Id of the unit that contains guard register or port.
 * @param index Register or operand index of guard.
 * @param isInverted Is inverted guard.
 */
TTAMachine::Guard&
TPEFProgramFactory::findGuard(
    const TPEF::ResourceSection &resources,
    TTAMachine::Bus &bus, TPEF::MoveElement::FieldType type,
    Byte unitId, HalfWord index, bool isInverted) const {

    RegisterFile* guardRF = NULL;
    Port* guardPort = NULL;

    // find corresponding function unit or register file.
    switch (type) {
    case TPEF::MoveElement::MF_UNIT: {
        FunctionUnit &guardUnit = findFunctionUnit(resources, unitId);

        // find port
        ResourceElement* resource = NULL;

        // TODO refactor
        if (resources.hasResource(
                ResourceElement::MRT_OP, index)) {

            resource =
                &resources.findResource(ResourceElement::MRT_OP, index);

        } else if (resources.hasResource(
                       ResourceElement::MRT_PORT, index)) {

            resource =
                &resources.findResource(ResourceElement::MRT_PORT, index);

        } else if (resources.hasResource(
                       ResourceElement::MRT_SR, index)) {

            resource =
                &resources.findResource(ResourceElement::MRT_SR, index);

        } else {
            abortWithError("Can't find resource port, operation or "
                           "special register with index:" +
                           Conversion::toString(index));

        }


        std::string tpefOpStr =
            stringOfChunk(resource->name(), resources.link());

        // find operation port or special register port
        if (guardUnit.hasPort(tpefOpStr)) {
            guardPort = guardUnit.port(tpefOpStr);

        } else {
            std::string::size_type dotPos = tpefOpStr.find('.');
            assert (dotPos != std::string::npos);
            std::string operationName = tpefOpStr.substr(0, dotPos);
            HWOperation* oper = guardUnit.operation(operationName);
            Word operandIndex = Conversion::toInt(
                tpefOpStr.substr(dotPos+1, tpefOpStr.length() - dotPos - 1));

            guardPort = oper->port(operandIndex);
        }
    } break;

    case TPEF::MoveElement::MF_RF:
        guardRF = &findRegisterFile(resources, unitId);
        break;

    default:
        abortWithError(
            "Error: Unknown guard type. Guard must be either FU port "
            "or RF index.");
    }

    assert (reinterpret_cast<long int>(guardPort) != 
            reinterpret_cast<long int>(guardRF));

    for (int i = 0; i < bus.guardCount(); i++) {
        Guard* currGuard = bus.guard(i);

        if (currGuard->isInverted() == isInverted) {
            PortGuard* portGuard = dynamic_cast<PortGuard*>(currGuard);
            RegisterGuard* registerGuard =
                dynamic_cast<RegisterGuard*>(currGuard);

            if (portGuard != NULL && guardPort != NULL) {
                if (portGuard->port() == guardPort) {
                    return *currGuard;
                }

            } else if (registerGuard != NULL && guardRF != NULL) {
                if (registerGuard->registerFile() == guardRF &&
                    registerGuard->registerIndex() == index) {
                    return *currGuard;
                }
            }
        }
    }

    std::string guardType;
    if (isInverted) {
        guardType = "! ";
    } else {
        guardType = "? ";
    }

    if (guardRF != NULL) {
        guardType += guardRF->name()  + "." + Conversion::toString(index);
    }

    if (guardPort != NULL) {
        guardType += "Some FU operation or special register.";
    }

    throw NotAvailable(
        __FILE__, __LINE__, __func__,
        "Can't find suitable guard: " + guardType + "\tfrom bus: " +
        bus.name());

    return *bus.guard(0);
}

/**
 * Returns instruction template that can be used for current instruction.
 *
 * @param resources TPEF resource section.
 * @param longImmediates Long immediates of instruction.
 * @param moves Moves of instruction.
 * @return Instruction template that can be used for this instruction.
 */
InstructionTemplate&
TPEFProgramFactory::findInstrTemplate(
    const TPEF::ResourceSection &resources,
    ImmediateVector& longImmediates,
    MoveVector& moves) const {

    if (machine_ == NULL) {
        assert(
            universalMachine_->instructionTemplateNavigator().count() == 1);
        return *universalMachine_->instructionTemplateNavigator().item(0);
    }

    // check how many bits must be written to each immediate unit....
    std::map<ImmediateUnit*, int> bitsToWrite;

    for (unsigned int i = 0; i < longImmediates.size(); i++) {
        ImmediateElement* imm = longImmediates[i];

        // destination unit
        ImmediateUnit* dstUnit =
            &findImmediateUnit(resources, imm->destinationUnit());

        if (MapTools::containsKey(bitsToWrite, dstUnit)) {
            throw NotAvailable(
                __FILE__, __LINE__, __func__,
                "Can't write two immediates to the same immediate unit "
                " in the same instruction.");
        } else {
            bitsToWrite[dstUnit] = dstUnit->zeroExtends() ?
                MathTools::requiredBits(imm->word()) :
                MathTools::requiredBitsSigned(imm->signedWord());
        }
    }

    // find suitable template
    Machine::InstructionTemplateNavigator tempNav =
        machine_->instructionTemplateNavigator();

    for (int i = 0; i < tempNav.count(); i++) {
        InstructionTemplate* insTemp = tempNav.item(i);

        // check if numberOfDestinations is same that number of
        // immediates to write
        if (insTemp->numberOfDestinations() ==
            static_cast<int>(bitsToWrite.size())) {

            bool templateIsGood = true;

            // check that destinations and bitwidths match
            for (std::map<ImmediateUnit*, int>::iterator iter =
                     bitsToWrite.begin();
                 iter != bitsToWrite.end();
                 iter++) {

                if (!insTemp->isOneOfDestinations(*(*iter).first) ||
                    (*iter).second >
                    insTemp->supportedWidth(*(*iter).first)) {
                    templateIsGood = false;
                    break;
                }
            }

            // check if move slots allows to use this template
            if (templateIsGood) {
                for (unsigned int j = 0; j < moves.size(); j++) {
                    MoveElement* move = moves[j];

                    if (move->isEmpty()) {
                        continue;
                    }

                    Bus& usedBus = findBus(resources, move->bus());

                    if (insTemp->usesSlot(usedBus.name())) {
                        templateIsGood = false;
                        break;
                    }
                }
            }

            // if template passed all the checks
            if (templateIsGood) {
                return *insTemp;
            }
        }
    }

    throw NotAvailable(__FILE__, __LINE__, __func__,
                       "Valid instruction template is not found.");
}

/**
 * Checks if the source of SocketAllocation can be assigned towards
 * given map of already allocated sockets.
 *
 * If tried socket is already used, it still can be used for another
 * reading if 
 * 1) the source register index is same for both of the allocations, or 
 * 2) the moves have opposite guards.
 *
 * @param alloc Socket allocation structure which is checked.
 * @param fixedSockets Map of already made socket allocations.
 * @return True if tried alloc is possible.
 */
bool
TPEFProgramFactory::canSourceBeAssigned(
    SocketAllocation& alloc,
    std::map<Socket*, std::vector<SocketAllocation*> >& fixedSockets) const {

    Socket* currentSocket = alloc.srcSocks[alloc.src];

    if (MapTools::containsKey(fixedSockets, currentSocket)) {

        std::vector<SocketAllocation*>& socketAllocs = fixedSockets[currentSocket];

        // test against all allocations.
        for (unsigned int i = 0; i < socketAllocs.size(); i++) {
            // TODO: check against all users.
            std::shared_ptr<Move> oldMove = socketAllocs[i]->move;
            Terminal* oldTerminal = &(oldMove->source());

            // allowed for same register of opposite guard.
            if (alloc.move->source().index() != oldTerminal->index() &&
                (alloc.move->isUnconditional() || oldMove->isUnconditional() ||
                 !alloc.move->guard().guard().isOpposite(oldMove->guard().guard()))) {
                return false;
            }
        }
        //Application::errorStream() << insTemp->NOPSlotCount() << " " << templateIsGood << " ";
        //Application::errorStream() << std::endl;
    }

    return true;
}


/**
 * Checks if the destination of SocketAllocation can be assigned towards
 * given map of already allocated sockets.
 *
 * If tried socket is already used, it still can be used for another
 * writing if the moves have opposite guards.
 *
 * @param alloc Socket allocation structure which is checked.
 * @param fixedSockets Map of already made socket allocations.
 * @return True if tried alloc is possible.
 */
bool
TPEFProgramFactory::canDestinationBeAssigned(
    SocketAllocation& alloc,
    std::map<Socket*, std::vector<SocketAllocation*> >& fixedSockets) const {

    Socket* currentSocket = alloc.dstSocks[alloc.dst];

    if (MapTools::containsKey(fixedSockets, currentSocket)) {

        std::vector<SocketAllocation*>& socketAllocs = fixedSockets[currentSocket];

        // test against all allocations.
        for (unsigned int i = 0; i < socketAllocs.size(); i++) {
            // TODO: check against all users.
            std::shared_ptr<Move> oldMove = socketAllocs[i]->move;
            if (alloc.move->isUnconditional() || oldMove->isUnconditional() ||
                !alloc.move->guard().guard().isOpposite(
                    oldMove->guard().guard())) {
                return false;
            }
        }
    }
    return true;
}




/**
 * Resolves sockets that are used for GPR and Immediate Unit reading and
 * writing for instruction.
 *
 * NOTE: TPEF specification currently supports storing port information
 *       of GPR references, so if this solution does not work or
 *       if this is too slow, then register port information should
 *       be implemented to TPEF object modell.
 *
 * @todo Yes, we should store the port allocations also
 * to TPEF, if they have been assigned by the scheduler. This kind of
 * resolving again and again does not make sense!
 *
 * @param allocs All GPR and IU reads and writes of instruction.
 * One item in this vector represents one move in the instruction.
 */
void
TPEFProgramFactory::resolveSocketAllocations(
    std::vector<SocketAllocation>& allocs) const {

    // map of socket to all moves which use it.
    std::map<Socket*, std::vector<SocketAllocation*> > fixedSockets;

    // try to resolve working combination starting from the first allocation
    unsigned currIndex = 0;

    while (currIndex < allocs.size()) {
        bool allocationWasSuccess = true;

        SocketAllocation& alloc = allocs[currIndex];

        if (alloc.srcSocks.empty()) {
            currIndex++;
            continue;
        }

        // try to get allocation for current element
        if (alloc.src < alloc.srcSocks.size()) {
            while (!canSourceBeAssigned(alloc, fixedSockets)) {
                alloc.src++;

                if (alloc.src == alloc.srcSocks.size()) {
                    // working socket could not be found
                    allocationWasSuccess = false;
                    break;
                }
            }
        } else {
            allocationWasSuccess = false;
        }

        // if allocation was success add allocation to for the socket
        // otherwise remove the latest allocations and try to resolve them
        // again

        if (allocationWasSuccess) {
            // assign allocation for this socket
            Socket* currentSocket = alloc.srcSocks[alloc.src];
            fixedSockets[currentSocket].push_back(&alloc);

        } else {
            // reset allocation try sequence
            alloc.src = 0;

            // free previous allocation and try again
            unsigned int prevIndex = currIndex;

            do {
                if (prevIndex == 0) {
                    throw NotAvailable(
                        __FILE__, __LINE__, __func__,
                        "Can't resolve src sockets for instruction.");
                }

                prevIndex--;

            } while (allocs[prevIndex].srcSocks.empty());


            Socket* socketToFree =
                allocs[prevIndex].srcSocks[allocs[prevIndex].src];
            std::vector<SocketAllocation*>& freedAllocs =
                fixedSockets[socketToFree];

            bool prevFound = false;
            // find first freed allocation where we continue search
            for (int k = prevIndex; k >= 0 && !prevFound; k--) {
                for (std::vector<SocketAllocation*>::iterator j =
                         freedAllocs.begin(); j != freedAllocs.end(); j++) {
                    
                    if (static_cast<int>((*j)->index) == k) {
                        (*j)->src++;
                        currIndex = k;
                        prevFound = true;
                        freedAllocs.erase(j);
                        break;
                    }                        
                }
            }
            continue; // don't touch currIndex here
        }

        currIndex++;
    }


    // and same for the destinations
    fixedSockets.clear();
    currIndex = 0;

    while (currIndex < allocs.size()) {
        bool allocationWasSuccess = true;

        SocketAllocation& alloc = allocs[currIndex];

        if (alloc.dstSocks.empty()) {
            currIndex++;
            continue;
        }

        // try to get allocation for current element
        if (alloc.dst < alloc.dstSocks.size()) {
            while (!canDestinationBeAssigned(alloc, fixedSockets)) {
                alloc.dst++;

                if (alloc.dst == alloc.dstSocks.size()) {
                    // working socket could not be found
                    allocationWasSuccess = false;
                    break;
                }
            }
        } else {
            allocationWasSuccess = false;
        }

        // if allocation was success add allocation to for the socket
        // otherwise remove the latest allocations and try to resolve them
        // again

        if (allocationWasSuccess) {
            // assign allocation for this socket
            Socket* currentSocket = alloc.dstSocks[alloc.dst];
            fixedSockets[currentSocket].push_back(&alloc);

        } else {
            // reset allocation try sequence
            alloc.dst = 0;

            // free previous allocation and try again
            unsigned int prevIndex = currIndex;

            do {
                if (prevIndex == 0) {
                    throw NotAvailable(
                        __FILE__, __LINE__, __func__,
                        "Can't resolve dst sockets for instruction.");
                }

                prevIndex--;

            } while (allocs[prevIndex].dstSocks.empty());


            Socket* socketToFree =
                allocs[prevIndex].dstSocks[allocs[prevIndex].dst];
            std::vector<SocketAllocation*>& freedAllocs =
                fixedSockets[socketToFree];

            // find first freed allocation where we continue search
            for (unsigned int j = 0; j < freedAllocs.size(); j++) {
                if (freedAllocs[j]->index < currIndex) {
                    currIndex = freedAllocs[j]->index;
                }
            }

            // clean allocation indexes and increment index that is used
            // as a next start point for the search loop
            for (unsigned int j = 0; j < freedAllocs.size(); j++) {
                if (freedAllocs[j]->index != currIndex) {
                    freedAllocs[j]->dst = 0;
                } else {
                    freedAllocs[j]->dst++;
                }
            }

            fixedSockets.erase(socketToFree);
            continue; // don't touch currIndex here
        }

        currIndex++;
    }

    // fix terminals
    for (unsigned i = 0; i < allocs.size(); i++) {
        SocketAllocation &alloc = allocs[i];

#if 0
        std::cerr << "next allocation source: " 
                  << alloc.move->source().isGPR()
                  << " destination: " 
                  << alloc.move->destination().isGPR() << std::endl;
#endif

        if (!alloc.srcSocks.empty()) {
            Socket* srcSocket = alloc.srcSocks[alloc.src];
            Unit* parent = alloc.move->source().port().parentUnit();

            for (int j = 0; j < srcSocket->portCount(); j++) {
                if (srcSocket->port(j)->parentUnit() == parent) {
                    alloc.move->setSource(
                        new TerminalRegister(
                            *srcSocket->port(j),
                            alloc.move->source().index()));
#if 0
                    std::cerr << "source was replaced" << std::endl;
#endif
                    break;
                }
            }
        }

        if (!alloc.dstSocks.empty()) {
            Socket* dstSocket = alloc.dstSocks[alloc.dst];
            Unit* parent = alloc.move->destination().port().parentUnit();

            for (int j = 0; j < dstSocket->portCount(); j++) {
                if (dstSocket->port(j)->parentUnit() == parent) {
                    alloc.move->setDestination(
                        new TerminalRegister(
                            *dstSocket->port(j),
                            alloc.move->destination().index()));
#if 0
                    std::cerr << "dst was replaced" << std::endl;
#endif
                    break;
                }
            }
        }
#if 0
        if (alloc.move->source().isGPR() &&
            alloc.move->destination().isGPR()) {
            std::cerr << "next allocation source: "
                      << alloc.move->source().port().name()
                      << " destination: "
                      << alloc.move->destination().port().name() << std::endl;
        }
#endif
    }
}

/**
 * Returns cached terminal.
 *
 * @param key Cache key of terminal.
 * @return Cached terminal if there is one, NULL otherwise.
 */
Terminal*
TPEFProgramFactory::getFromCache(
    const TPEFProgramFactory::CacheKey &key) const {

    if (MapTools::containsKey(cache_,key)) {
        return MapTools::valueForKey<Terminal*>(cache_,key);
    }
    return NULL;
}

/**
 * Adds Terminal to cache.
 *
 * @param key Key of search variables for getting terminal from machine(s).
 * @param cachedTerm Terminal to be add to cache.
 * @return cached terminal if there is one.
 */
void
TPEFProgramFactory::addToCache(
    const CacheKey &key,
    Terminal* cachedTerm) const {

    cache_[key] = cachedTerm;
}

/**
 * Clears cache.
 */
void
TPEFProgramFactory::clearCache() const {
    cache_.clear();
}

/**
 * Analyses all CodeSection from TPEF and tries to find function start points.
 *
 * @todo This function should do code analysis, but for it actually scans
 *       symbol sections for code symbol, which are intepret as function
 *       start points.
 */
void
TPEFProgramFactory::seekFunctionStartPoints() {

    // clear table if already exists (from previous POM builds)
    MapTools::deleteAllValues(functionStartPositions_);

    // check every symbol section
    for (Word i = 0; i < binary_->sectionCount(Section::ST_SYMTAB); i++) {

        SymbolSection *currSect =
            dynamic_cast<SymbolSection*>(
                binary_->section(Section::ST_SYMTAB, i));

        for (Word j = 0; j < currSect->elementCount(); j++) {
            ProcedSymElement* procedSymbol =
                dynamic_cast<ProcedSymElement*>(currSect->element(j));

            // symbol was not procedure symbol
            if (procedSymbol == NULL) {
                continue;
            }

            InstructionElement* referencedInstruction =
                procedSymbol->reference();

            // maybe we just could ignore null elements... but
            // there really should not be NULL code symbols, unless
            // referenced instruction has been moved or something...
            assert(referencedInstruction != NULL);

            std::string functionName =
                stringOfChunk(procedSymbol->name(), currSect->link());

            // add instruction element of symbol to entrypoint table
            functionStartPositions_[referencedInstruction] =
                new FunctionStart(functionName);
        }
    }
}

/**
 * Adds global labels of TPEF to Program.
 *
 * @param prog Program where to add labels.
 */
void
TPEFProgramFactory::createLabels(Program &prog) {
    /// prevent addition of local symbol with same name multiple times    
    std::map<string, std::pair<DataLabel*, bool> > dataLabels;

    for (Word i = 0; i < binary_->sectionCount(Section::ST_SYMTAB); i++) {

        SymbolSection* currSect = dynamic_cast<SymbolSection*>(
            binary_->section(Section::ST_SYMTAB, i));

        assert(currSect != NULL);

        StringSection* strings =
            dynamic_cast<StringSection*>(currSect->link());

        assert(strings != NULL);

        for (Word j = 0; j < currSect->elementCount(); j++) {
            SymbolElement* sym =
                dynamic_cast<SymbolElement*>(currSect->element(j));

            // read all local and global data labels 
            if (sym->type() == SymbolElement::STT_DATA) {

                DataSymElement* dataSym =
                    dynamic_cast<DataSymElement*>(sym);
                
                const std::string labelString = 
                    strings->chunk2String(dataSym->name());

                UDataSection* dataSection = dynamic_cast<UDataSection*>(
                    dataSym->section());
                
                assert(dataSection != NULL);

                TTAMachine::AddressSpace* targetASpace = NULL;
                try {
                    targetASpace = 
                        &findAddressSpace(dataSection->aSpace());
                } catch (const NotAvailable& e) {
                    NotAvailable newException(
                        __FILE__, __LINE__, __func__, 
                        (boost::format(
                            "Unable to find address space for target "
                            "of data label '%s'") % labelString).str());
                    newException.setCause(e);
                    throw newException;                        
                }
                                
                Address refAddress(
                    dataSection->bytesToMAUs(
                        dataSym->reference()->offset()) +
                    dataSection->startingAddress(),
                    *targetASpace);

                DataLabel* dataLabel = new DataLabel(
                    labelString, refAddress, prog.globalScope());

// nice debug info
//		    std::cerr << "Added data label\t"
//			      <<"\tname: " << dataLabel->name()
//			      << "\taddress: " 
//                << Conversion::toString(dataLabel->address().location())
//			      << std::endl;
                                
                // check if label name is already used
                if (MapTools::containsKey(dataLabels, labelString)) {
                    
                    // if latest is global remove old one
                    if (sym->binding() == SymbolElement::STB_GLOBAL) {
                        
                        // if there is two global symbols with same name 
                        // throw exception
                        if (dataLabels[labelString].second) {

                            // free reserved labels
                            for (std::map<string, 
                                     std::pair<DataLabel*, 
                                     bool> >::iterator iter = 
                                     dataLabels.begin();
                                 iter != dataLabels.end(); iter++) {
                                delete (*iter).second.first;
                            }
                            delete dataLabel;

                            throw NotAvailable(
                                __FILE__, __LINE__, __func__,
                                "Found two global symbols with same name: " + 
                                labelString);
                        }
                        
                        delete dataLabels[labelString].first;
                        dataLabels[labelString].first = dataLabel;
                        dataLabels[labelString].second = true;
                    }
                } else {
                    // add symbol first time
                    dataLabels[labelString].first = dataLabel;
                    dataLabels[labelString].second = 
                        (sym->binding() == SymbolElement::STB_GLOBAL);
                }
                
                // Global code labels
            } else if (sym->binding() == SymbolElement::STB_GLOBAL && 
                       sym->type() == SymbolElement::STT_CODE) {

                CodeSymElement* codeSym =
                        dynamic_cast<CodeSymElement*>(sym);

                    assert(MapTools::containsKey(
                        instructionMap_, codeSym->reference()));

                    InstructionReference refIns =
                        prog.instructionReferenceManager().createReference(
                            *instructionMap_[codeSym->reference()]);

                    CodeLabel* newLabel =
                        new CodeLabel(refIns,
                                      strings->chunk2String(sym->name()));

                    prog.globalScope().addCodeLabel(newLabel);
            }
        }
    }

    // Add symbols to program
    /// prevent addition of local symbol with same name multiple times    
    for (std::map<string, std::pair<DataLabel*, bool> >::iterator iter =  
             dataLabels.begin();
         iter != dataLabels.end(); iter++) {

        prog.globalScope().addDataLabel((*iter).second.first);
    }
}

/**
 * Creates data memories to program.
 *
 * @param prog Program containing converted TPEF instructions.
 */
void
TPEFProgramFactory::createDataMemories(Program &prog) {

    // ------- search through data sections and group them by address spaces
    std::map<AddressSpace*, std::vector<UDataSection*> > memories;    

    for (int i = 0; i < static_cast<int>(binary_->sectionCount()); i++) {
        Section* currSect = binary_->section(i);
        
        if (currSect->type() == Section::ST_DATA || 
            currSect->type() == Section::ST_UDATA ||
            currSect->type() == Section::ST_LEDATA) {
            
            UDataSection* uDataSect = dynamic_cast<UDataSection*>(currSect);

            AddressSpace& aSpace = findAddressSpace(uDataSect->aSpace());
            std::vector<UDataSection*>& secVec = memories[&aSpace];
            
            // sort sections of current vector by addess...
            UDataSection* temp = NULL;
            for (int j = 0; j < static_cast<int>(secVec.size()); j++) {
                if (secVec[j]->startingAddress() > 
                    uDataSect->startingAddress()) {
                    temp = secVec[j];
                    secVec[j] = uDataSect;                    
                    uDataSect = temp;
                }
            }
            
            secVec.push_back(uDataSect);
        }
    }

    // ------------- create DataMemory for each address space
    for (std::map<AddressSpace*, std::vector<UDataSection*> >::iterator iter =
             memories.begin(); 
         iter != memories.end(); iter++) {
               
        AddressSpace& aSpace = *(*iter).first;
        std::vector<UDataSection*>& secVec = (*iter).second;
        
        DataMemory* newDataMem = new DataMemory(aSpace);
        
        for (int i = 0; i < static_cast<int>(secVec.size()); i++) {
            UDataSection* currSect = secVec[i];

            if (currSect->type() == Section::ST_UDATA) {
                // ------- create uninitializes data definition
                DataDefinition* newDef = 
                    new DataDefinition(
                        Address(currSect->startingAddress(), aSpace),
                        static_cast<int>(currSect->lengthInMAUs()),
                        prog.targetProcessor().isLittleEndian());

                newDataMem->addDataDefinition(newDef);
                               
            } else {
                // -------- create initialized data definition
                assert(currSect->type() == Section::ST_DATA ||
                       currSect->type() == Section::ST_LEDATA);
                               
                // find relocation section for this section
                RelocSection* relocs = NULL;
                
                for (int j = 0; 
                     j < static_cast<int>(
                         binary_->sectionCount(Section::ST_RELOC)); j++) {
                    
                    RelocSection* temp = 
                        dynamic_cast<RelocSection*>(
                            binary_->section(Section::ST_RELOC, j));
                                        
                    if (temp->referencedSection() == currSect) {
                        // found it!
                        relocs = temp;
                        break;
                    }
                }

                // ------- create relocated definitions.
                if (relocs != NULL) {
 
                    DataSection* refSect = dynamic_cast<DataSection*>(
                        relocs->referencedSection());

                    for (int j = 0; 
                         j < static_cast<int>(relocs->elementCount()); j++) {
                                                
                        RelocElement* currElem = 
                            dynamic_cast<RelocElement*>(relocs->element(j));
                        
                        AddressSpace& dstSpace = 
                            findAddressSpace(currElem->aSpace());
                        
                        // resolve location address of relocation
                        Chunk* srcChunk = 
                            dynamic_cast<Chunk*>(currElem->location());
                        
                        int sourceAddress = 
                            refSect->startingAddress() + 
                            refSect->chunkToMAUIndex(srcChunk);
                        
                        Address startAddr(sourceAddress, aSpace);
                        
                        // resolve mau size of address field 
                        int mauSize = currElem->size() / aSpace.width(); 
                        
                        // field size must be multiple of mau of address space
                        assert(currElem->size() % aSpace.width() == 0);
                                       
                        if (&dstSpace == adfInstrASpace_) {
                            // ------- destination is instruction 
                            
                            InstructionElement* tpefInstr = 
                                dynamic_cast<InstructionElement*>(
                                    currElem->destination());
                            
                            assert(tpefInstr != NULL);
                            
                            // get the instruction reference of destination
                            InstructionReference instrRef = 
                                prog.instructionReferenceManager().
                                createReference(*instructionMap_[tpefInstr]);
                            
                            DataInstructionAddressDef* newDataDef = 
                                new DataInstructionAddressDef(
                                    startAddr, mauSize, instrRef,
                                    prog.targetProcessor().isLittleEndian());
                            
                            newDataMem->addDataDefinition(newDataDef);
                            
                        } else {
                            // ------- destination is chunk
                            
                            // find dst section of destination() chunk
                            Chunk* dstChunk = 
                                dynamic_cast<Chunk*>(currElem->destination());
                            
                            assert(dstChunk != NULL);

                            UDataSection* dstSect = NULL;
                            
                            std::vector<UDataSection*>& dstSecs = 
                                memories[&dstSpace];

                            for (int k = 0; 
                                 k < static_cast<int>(dstSecs.size()); k++) {

                                UDataSection* temp = dstSecs[k];

                                if (temp->belongsToSection(dstChunk)) {
                                    // dst section found!
                                    dstSect = temp;
                                    break;
                                }
                            }
                            
                            assert(dstSect != NULL);
                            
                            Address dstAddr(
                                dstSect->startingAddress() +
                                dstSect->chunkToMAUIndex(dstChunk), 
                                dstSpace);
                                                      
                            DataAddressDef* newDataDef = 
                                new DataAddressDef(
                                    startAddr, mauSize, dstAddr,
                                    prog.targetProcessor().isLittleEndian());

                            newDataMem->addDataDefinition(newDataDef);
                            
                        }                                                
                    }
                }
                
                // ----- create all the rest of the init data

                // find start and end addresses of current section
                std::pair<int, int> wholeSection(currSect->startingAddress(),
                                                 currSect->lengthInMAUs());

                std::vector<std::pair <int, int> > dataAreas;                
                dataAreas.push_back(wholeSection);

                // split area if there is some data area definitions in the
                // same addresses with current section
                for (int k = 0; k < newDataMem->dataDefinitionCount(); k++) {
                    DataDefinition& currDef = newDataMem->dataDefinition(k);

                    int prevIndex = dataAreas.size()-1;
                    std::pair<int, int>& lastArea = dataAreas[prevIndex];

                    // if data definition is inside this section, it splits 
                    // the last data area definition
                    if (static_cast<int>(currDef.startAddress().location()) >= 
                        lastArea.first && 
                        static_cast<int>(currDef.startAddress().location()) < 
                        lastArea.first + lastArea.second) {
                        
                        int lastAreaStart = lastArea.first;
                        int lastAreaEnd = currDef.startAddress().location();

                        int newAreaStart = 
                            currDef.startAddress().location() + 
                            currDef.size();

                        int newAreaEnd = lastArea.first + lastArea.second;

                        // new area starts after this data definition
                        std::pair<int, int> newArea(
                            newAreaStart, newAreaEnd - newAreaStart);
                        
                        // if the last area is no area anymore
                        lastArea.second = lastAreaEnd - lastAreaStart;
                        
                        if (lastArea.second == 0) {
                            dataAreas.pop_back();
                        }
                        
                        if (newArea.second != 0) {
                            dataAreas.push_back(newArea);
                        }
                    }
                }
                
                DataSection* dataSect = dynamic_cast<DataSection*>(currSect);
                assert(dataSect != NULL);

                // write out collected initialized data areas
                for (unsigned int k = 0; k < dataAreas.size(); k++) {

                    std::pair<int, int>& currArea = dataAreas[k];
                    std::vector<MinimumAddressableUnit> initData;
                    
                    int mauIndex = 
                        currArea.first - currSect->startingAddress();

                    assert(mauIndex >= 0);

                    assert(mauIndex + currArea.second <= 
                           static_cast<int>(currSect->lengthInMAUs()));

                    bool allZeros = true;
                    for (int l = 0; l < currArea.second; l++) {
                        if(dataSect->MAU(mauIndex + l) != 0) {
                            allZeros = false;
                        }
                    }

                    DataDefinition* newDataDef = NULL;

                    if (allZeros) {
                        newDataDef = new DataDefinition(
                            Address(currArea.first, aSpace),
                            currArea.second,
                            prog.targetProcessor().isLittleEndian(),
                            NULL, true );
                    } else {
                        for (int l = 0; l < currArea.second; l++) {
                            initData.push_back(dataSect->MAU(mauIndex++));
                        }
                    
                        newDataDef = new DataDefinition(
                            Address(currArea.first, aSpace),
                            initData, prog.targetProcessor().isLittleEndian());
                    }
                    
                    newDataMem->addDataDefinition(newDataDef);
                }
            }
        }
        
        prog.addDataMemory(newDataMem);                
    }
}

/**
 * Checks if move is start point of function.
 *
 * @param instructionElement Intruction element which should be checked.
 * @return True if instruction in parameter is function start point.
 */
bool
TPEFProgramFactory::isFunctionStart(
    const InstructionElement &instructionElement) const {
    return MapTools::containsKey(
        functionStartPositions_, &instructionElement);
}

/**
 * Returns name of function, that starts, from given instruction element.
 *
 * @param instructionElement Starting element, of procedure.
 * @return Function name string.
 */
std::string
TPEFProgramFactory::functionName(
    const InstructionElement &instructionElement) const {
    if (MapTools::containsKey(functionStartPositions_, &instructionElement)) {
        return MapTools::valueForKey<FunctionStart*>(
            functionStartPositions_, &instructionElement)->name();
    } else {
        return "unknownFunctionName";
    }
}
}
