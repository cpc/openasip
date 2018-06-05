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
 * @file ProgramWriter.cc
 *
 * Implementation of ProgramWriter class.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <cmath>

#include "ProgramWriter.hh"
#include "Binary.hh"
#include "NullSection.hh"
#include "ResourceSection.hh"
#include "StringSection.hh"
#include "ASpaceSection.hh"
#include "StringSection.hh"
#include "RelocSection.hh"
#include "CodeSection.hh"
#include "DataSection.hh"
#include "UDataSection.hh"
#include "SymbolSection.hh"
#include "AddressSpace.hh"
#include "Machine.hh"
#include "UniversalMachine.hh"
#include "FunctionUnit.hh"
#include "UniversalFunctionUnit.hh"
#include "ASpaceElement.hh"
#include "RelocElement.hh"
#include "Program.hh"
#include "ControlUnit.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "Bus.hh"
#include "MoveElement.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "TerminalFUPort.hh"
#include "RegisterFile.hh"
#include "UnboundedRegisterFile.hh"
#include "Guard.hh"
#include "MoveGuard.hh"
#include "FUPort.hh"
#include "SpecialRegisterPort.hh"
#include "HWOperation.hh"
#include "Operation.hh"
#include "ImmediateElement.hh"
#include "UDataSection.hh"
#include "DataSection.hh"
#include "NullInstruction.hh"
#include "Chunk.hh"
#include "InstructionReference.hh"
#include "TPEFTools.hh"
#include "GlobalScope.hh"
#include "CodeSymElement.hh"
#include "ProcedSymElement.hh"
#include "DataSymElement.hh"
#include "NoTypeSymElement.hh"
#include "SymbolElement.hh"
#include "CodeLabel.hh"
#include "DataLabel.hh"
#include "Immediate.hh"
#include "TerminalImmediate.hh"
#include "DataMemory.hh"
#include "DataDefinition.hh"
#include "SectionElement.hh"
#include "ProgramAnnotation.hh"
#include "InstructionElement.hh"
#include "TCEString.hh"
#include "MathTools.hh"
#include "POMDisassembler.hh"

using TPEF::Binary;
using TPEF::Section;
using TPEF::NullSection;
using TPEF::StringSection;
using TPEF::ResourceSection;
using TPEF::ASpaceSection;
using TPEF::StringSection;
using TPEF::RelocSection;
using TPEF::CodeSection;
using TPEF::DataSection;
using TPEF::UDataSection;
using TPEF::SymbolSection;
using TPEF::NoTypeSymElement;
using TPEF::ASpaceElement;
using TPEF::MoveElement;
using TPEF::ResourceElement;
using TPEF::ImmediateElement;
using TPEF::RelocElement;
using TPEF::UDataSection;
using TPEF::DataSection;
using TPEF::Chunk;
using TPEF::TPEFTools;
using TPEF::SymbolElement;
using TPEF::CodeSymElement;
using TPEF::ProcedSymElement;
using TPEF::DataSymElement;
using TPEF::Binary;
using TPEF::SectionElement;
using TPEF::InstructionAnnotation;

using TTAMachine::AddressSpace;
using TTAMachine::Machine;
using TTAMachine::FunctionUnit;
using TTAMachine::ControlUnit;
using TTAMachine::Bus;
using TTAMachine::Port;
using TTAMachine::RegisterFile;
using TTAMachine::Guard;
using TTAMachine::PortGuard;
using TTAMachine::RegisterGuard;
using TTAMachine::FUPort;
using TTAMachine::HWOperation;
using TTAMachine::SpecialRegisterPort;
using TTAMachine::ImmediateUnit;

namespace TTAProgram {

////////////////////////////////////////////////////////////////////////////
// TPEFResourceUpdater
////////////////////////////////////////////////////////////////////////////

TPEFResourceUpdater::TPEFResourceUpdater(
    Machine &mach, ResourceSection &resources) :
    mach_(mach), resources_(resources),
    lastBusId_(1), lastUnitPortId_(1),
    lastFunctionUnitId_(1), lastRegisterFileId_(1),
    lastImmediateUnitId_(1) {

    initCache();
}

/**
 * Initialises resource updater cache, for finding TPEF resources by machine
 * resources. 
 *
 * This also checks already created TPEF resources to prevent multiple 
 * creation of the same ResourceElement.
 */
void
TPEFResourceUpdater::initCache() {

    for (Word i = 0; i < resources_.elementCount(); i++) {
        ResourceElement* currRes =
            dynamic_cast<ResourceElement*>(resources_.element(i));

        StringSection* strings =
            dynamic_cast<StringSection*>(resources_.link());

        std::string resourceName = strings->chunk2String(currRes->name());

        switch(currRes->type()) {

        case ResourceElement::MRT_NULL: {
            abortWithError("There should not be NULL resources.");
        } break;

        case ResourceElement::MRT_BUS: {
            Machine::BusNavigator navi = mach_.busNavigator();
            assert(navi.hasItem(resourceName));
            Bus *bus = navi.item(resourceName);

            cache_[CacheKey(bus, 0)] = currRes;
        } break;

        case ResourceElement::MRT_UNIT: {
            Machine::FunctionUnitNavigator navi =
                mach_.functionUnitNavigator();
            assert(navi.hasItem(resourceName));

            if (lastFunctionUnitId_ < currRes->id()) {
                lastFunctionUnitId_ = currRes->id()+1;
            }

            FunctionUnit *fu = navi.item(resourceName);

            cache_[CacheKey(fu, 0)] = currRes;
        } break;

        case ResourceElement::MRT_RF: {
            Machine::RegisterFileNavigator navi =
                mach_.registerFileNavigator();
            assert(navi.hasItem(resourceName));

            if (lastRegisterFileId_ < currRes->id()) {
                lastRegisterFileId_ = currRes->id()+1;
            }

            RegisterFile *rf = navi.item(resourceName);

            cache_[CacheKey(rf, 0)] = currRes;
        } break;

        case ResourceElement::MRT_OP: {
            std::string::size_type dotPos = resourceName.find('.');
            assert(dotPos != std::string::npos);

            std::string operName = resourceName.substr(0, dotPos);

            int operandIndex = Conversion::toInt(
                resourceName.substr(
                    dotPos+1, resourceName.length() - dotPos - 1));

            Machine::FunctionUnitNavigator navi =
                mach_.functionUnitNavigator();

            if (lastUnitPortId_ < currRes->id()) {
                lastUnitPortId_ = currRes->id()+1;
            }

            // if function unit has operation by that name, get HWOperation...
            for (int i = 0; i < navi.count(); i++) {
                FunctionUnit *fu = navi.item(i);
                if (fu->hasOperation(operName)) {
                    cache_[CacheKey(fu->operation(operName), operandIndex)] =
                        currRes;
                }
            }

        } break;

        case ResourceElement::MRT_IMM: {
            Machine::ImmediateUnitNavigator navi =
                mach_.immediateUnitNavigator();
            assert(navi.hasItem(resourceName));

            if (lastImmediateUnitId_ < currRes->id()) {
                lastImmediateUnitId_ = currRes->id()+1;
            }

            ImmediateUnit* immUnit = navi.item(resourceName);

            cache_[CacheKey(immUnit, 0)] = currRes;

        } break;

        case ResourceElement::MRT_PORT:
        case ResourceElement::MRT_SR: {
            Machine::FunctionUnitNavigator navi =
                mach_.functionUnitNavigator();

            if (lastUnitPortId_ < currRes->id()) {
                lastUnitPortId_ = currRes->id()+1;
            }

            // if function unit has port by that name, get port
            for (int i = 0; i < navi.count(); i++) {
                FunctionUnit *fu = navi.item(i);

                if (fu->hasPort(resourceName)) {
                    cache_[CacheKey(fu->port(resourceName), 0)] = currRes;
                }
            }
        } break;

        default:
            abortWithError("Unknown resource type.");
        }

    }
}


/**
 * Clears the temporary variables and reinits the cache.
 */
void
TPEFResourceUpdater::clearCache() {
    cache_.clear();
    lastBusId_ = 1;
    lastUnitPortId_ = 1;
    lastFunctionUnitId_ = 1;
    lastRegisterFileId_ = 1;
    lastImmediateUnitId_ = 1;
    initCache();
}


/**
 * Finds and creates ResourceElement for the bus given in parameter.
 *
 * If resource id not already created to TPEF method creates it
 * and adds CacheKey for it.
 *
 * @param bus Bus whose tpef resource is looked for.
 * @return ResourceElement for the bus given in parameter.
 */
ResourceElement&
TPEFResourceUpdater::bus(const Bus &bus) {

    CacheKey key = CacheKey(&bus, 0);
    if (!MapTools::containsKey(cache_, key)) {
        ResourceElement *newRes = new ResourceElement();

        newRes->setType(ResourceElement::MRT_BUS);

        // check if bus belongs to universal machine, set universal bus ID
        // if it does
        UniversalMachine *uMach =
            dynamic_cast<UniversalMachine*>(bus.machine());

        if (uMach == NULL) {
            newRes->setId(lastBusId_++);

        } else {
            // find universal bus
            if (&bus == &uMach->universalBus()) {
                newRes->setId(ResourceElement::UNIVERSAL_BUS);

            } else {
                boost::format unexpectedBusMsg(
                    "Unexpected bus '%1%' in universal machine in addition "
                    "to the unique universal bus '%2%'.");

                abortWithError(unexpectedBusMsg.str());
            }
        }

        StringSection *strings =
            dynamic_cast<StringSection*>(resources_.link());

        newRes->setName(strings->string2Chunk(bus.name()));
#if 0
        std::cerr << "added MRT_BUS resource type: " << newRes->type()
                  << "\tid:" << newRes->id() << "\tname: "
                  << strings->chunk2String(newRes->name()) << std::endl;
#endif
        resources_.addElement(newRes);
        cache_[key] = newRes;
    }

    return *cache_[key];
}

/**
 * Finds and creates ResourceElement for a fu port given in parameter.
 *
 * If resource id not already created to TPEF method creates it
 * and adds CacheKey for it.
 *
 * @param port Port whose tpef resource is looked for.
 * @return ResourceElement for the machine part given in parameter.
 */
ResourceElement&
TPEFResourceUpdater::functionUnitPort(const Port &port) {

    CacheKey key = CacheKey(&port, 0);
    if (!MapTools::containsKey(cache_, key)) {
        ResourceElement *newRes = new ResourceElement();

        if (dynamic_cast<const TTAMachine::FUPort*>(&port) != NULL) {
            newRes->setType(ResourceElement::MRT_PORT);

        } else if (dynamic_cast<const TTAMachine::SpecialRegisterPort*>(
                       &port) != NULL) {

            newRes->setType(ResourceElement::MRT_SR);

        } else {
            abortWithError(
                "Port must be either FUPort or SpecialRegisterPort!");
        }

        newRes->setId((lastUnitPortId_++));

        StringSection *strings =
            dynamic_cast<StringSection*>(resources_.link());

        // check if universal and set return-address register name if needed
        UniversalMachine *uMach =
            dynamic_cast<UniversalMachine*>(port.parentUnit()->machine());

        if (uMach != NULL) {
            assert(uMach->controlUnit()->hasReturnAddressPort());
            if (uMach->controlUnit()->returnAddressPort() == &port) {

                newRes->setName(
                    strings->string2Chunk(
                        ResourceElement::RETURN_ADDRESS_NAME));
            } else {

                const SpecialRegisterPort* srp =
                    dynamic_cast<const SpecialRegisterPort*>(&port);

                if (srp == NULL) {
                    boost::format unexpectedPortMsg(
                        "Port '%1%' in universal global control unit. "
                        "Normal RF and FU ports are not allowed there.");
                    unexpectedPortMsg % port.name();
                    abortWithError(unexpectedPortMsg.str());

                } else {
                    // Might be allowed in the future if the system can
                    // be extended to support open-ended sets of special
                    // register ports in GCU.
                    boost::format unexpectedSRPortMsg(
                        "The special register port '%1%' is not one of "
                        "the ports currently supported by the global "
                        "control unit.");
                    unexpectedSRPortMsg % port.name();
                    abortWithError(unexpectedSRPortMsg.str());
                }
            }
        } else {
            newRes->setName(strings->string2Chunk(port.name()));
        }

#if 0
        std::cerr << "added MRT_SR resource type: " << newRes->type()
                  << "\tid:" << newRes->id() << "\tname: "
                  << strings->chunk2String(newRes->name()) << std::endl;
#endif
        resources_.addElement(newRes);

        // TODO: check that port is really found from universal or normal
        // machine
        cache_[CacheKey(&port, 0)] = newRes;

    }

    return *cache_[key];
}


/**
 * Finds and creates ResourceElement for a fu operand given in parameter.
 *
 * If resource id not already created to TPEF method creates it
 * and adds CacheKey for it.
 *
 * @param oper Operation whose TPEF resource is looked for.
 * @param operandIndex Index of the operand of operation.
 * @return ResourceElement for the operand given in parameter.
 */
ResourceElement&
TPEFResourceUpdater::operand(const HWOperation &oper, int operandIndex) {

    CacheKey key = CacheKey(&oper, operandIndex);
    if (!MapTools::containsKey(cache_, key)) {

        ResourceElement *newRes = new ResourceElement();

        newRes->setType(ResourceElement::MRT_OP);
        newRes->setId(lastUnitPortId_++);

        StringSection *strings =
            dynamic_cast<StringSection*>(resources_.link());

        newRes->setName(
            strings->string2Chunk(
                oper.name() + "." + Conversion::toString(operandIndex)));
#if 0
        std::cerr << "Added MRT_OP resource type: " << newRes->type()
                  << "\tid:" << newRes->id() << "\tname: "
                  << strings->chunk2String(newRes->name()) << std::endl;
#endif
        resources_.addElement(newRes);

        FunctionUnit *parentUnit = oper.parentUnit();

        if (parentUnit->hasOperation(oper.name())) {
            cache_[CacheKey(parentUnit->operation(oper.name()),
                            operandIndex)] = newRes;
        } else {
            abortWithError(
                "Parent unit: " + parentUnit->name() +
                " didn't contain requested operation:" + oper.name());
        }
    }
    return *cache_[key];
}


/**
 * Finds and creates ResourceElement for a fu given in parameter.
 *
 * If resource id not already created to TPEF method creates it
 * and adds CacheKey for it.
 *
 * @param fu Function unit whose TPEF resource is looked for.
 * @return ResourceElement for the function unit given in parameter.
 */
ResourceElement&
TPEFResourceUpdater::functionUnit(const FunctionUnit &fu) {

    CacheKey key = CacheKey(&fu, 0);

    if (!MapTools::containsKey(cache_, key)) {
        ResourceElement *newRes = new ResourceElement();

        newRes->setType(ResourceElement::MRT_UNIT);

        // check if universal and set universal id
        // if reference to universal machine
        UniversalMachine *uMach =
            dynamic_cast<UniversalMachine*>(fu.machine());

        if (uMach == NULL) {
            newRes->setId(lastFunctionUnitId_++);
            resources_.addElement(newRes);

        } else {
            // find universal function unit or gcu...
            if (&fu == &uMach->universalFunctionUnit() ||
                &fu == uMach->controlUnit()) {

                CacheKey uFUKey = CacheKey(
                    &uMach->universalFunctionUnit(), 0);
                CacheKey uGCUKey = CacheKey(uMach->controlUnit(), 0);

                // check if resource element is already created
                if (MapTools::containsKey(cache_, uFUKey)) {
                    delete newRes;
                    newRes = cache_[uFUKey];

                } else if (MapTools::containsKey(cache_, uGCUKey)) {
                    delete newRes;
                    newRes = cache_[uGCUKey];

                } else {
                    newRes->setId(ResourceElement::UNIVERSAL_FU);
                    resources_.addElement(newRes);
                }

            } else {
                abortWithError(
                    "Unknown universal machine reference to function unit: " +
                    fu.name());
            }
        }

        StringSection *strings =
            dynamic_cast<StringSection*>(resources_.link());

        newRes->setName(strings->string2Chunk(fu.name()));
#if 0
        std::cerr << "added MRT_UNIT resource type: " << newRes->type()
                  << "\tid:" << newRes->id() << "\tname: "
                  << strings->chunk2String(newRes->name()) << std::endl;
#endif

        cache_[key] = newRes;
    }
    return *cache_[key];
}


/**
 * Finds and creates ResourceElement for a rf given in parameter.
 *
 * If resource id not already created to TPEF method creates it
 * and adds CacheKey for it.
 *
 * @param rf Register file whose TPEF resource is looked for.
 * @return ResourceElement for the register file given in parameter.
 */
ResourceElement&
TPEFResourceUpdater::registerFile(const RegisterFile &rf) {

    CacheKey key = CacheKey(&rf, 0);
    if (!MapTools::containsKey(cache_, key)) {
        ResourceElement *newRes = new ResourceElement();

        newRes->setType(ResourceElement::MRT_RF);

        // check if universal RF
        UniversalMachine *uMach =
            dynamic_cast<UniversalMachine*>(rf.machine());

        if (uMach == NULL) {
            newRes->setId(lastRegisterFileId_++);

        } else {

            // find universal register file
            if (&rf == &uMach->booleanRegisterFile()) {
                newRes->setId(ResourceElement::BOOL_RF);
            } else if (&rf == &uMach->integerRegisterFile()) {
                newRes->setId(ResourceElement::INT_RF);
            } else if (&rf == &uMach->doubleRegisterFile()) {
                newRes->setId(ResourceElement::FP_RF);
            } else {
                abortWithError(
                    "Reference to unknown universal register file.");
            }
        }

        StringSection *strings =
            dynamic_cast<StringSection*>(resources_.link());

        newRes->setName(strings->string2Chunk(rf.name()));

#if 0
        std::cerr << "added MRT_RF resource type: " << newRes->type()
            << "\tid:" << newRes->id() << "\tname: "
            << strings->chunk2String(newRes->name()) << std::endl;
#endif

        resources_.addElement(newRes);
        cache_[key] = newRes;
    }
    return *cache_[key];
}


/**
 * Find and return the TPEF machine resource entry that corresponds to a
 * given immediate unit of the target processor.
 *
 * If no machine resource entry exists for the given unit, it is created,
 * cached into the resource updater and added to the TPEF machine resource
 * section.
 *
 * @param immUnit An immediate unit of the target processor.
 * @return The machine resource entry corresponding to given unit.
 */
ResourceElement&
TPEFResourceUpdater::immediateUnit(const ImmediateUnit& immUnit) {

    CacheKey key = CacheKey(&immUnit, 0);
    if (!MapTools::containsKey(cache_, key)) {
        ResourceElement* newRes = new ResourceElement();

        newRes->setType(ResourceElement::MRT_IMM);
        newRes->setId(lastImmediateUnitId_++);

        StringSection* strings =
            dynamic_cast<StringSection*>(resources_.link());

        newRes->setName(strings->string2Chunk(immUnit.name()));

#if 0
        std::cerr << "added MRT_IMM resource type: " << newRes->type()
            << "\tid:" << newRes->id() << "\tname: "
            << strings->chunk2String(newRes->name()) << std::endl;
#endif

        resources_.addElement(newRes);
        cache_[key] = newRes;
    }
    return *cache_[key];
}


////////////////////////////////////////////////////////////////////////////
// ProgramWriter part
////////////////////////////////////////////////////////////////////////////

const HalfWord
ProgramWriter::IMMEDIATE_ADDRESS_WIDTH = WORD_BITWIDTH;

const int
ProgramWriter::MAX_SIMM_WIDTH = 32;

/**
 * Constructor.
 *
 * @param prog Program that from TPEF is created.
 */
ProgramWriter::ProgramWriter(
    const Program& prog):
    prog_(prog) {
}


/**
 * Creates TPEF binary.
 *
 * @return TPEF binary of a program.
 */
Binary*
ProgramWriter::createBinary() const {
    Binary* newBin = new Binary();
        
    // create and initialize sections
    NullSection* nullSection = dynamic_cast<NullSection*>(
        Section::createSection(Section::ST_NULL));

    StringSection* strings = dynamic_cast<StringSection*>(
        Section::createSection(Section::ST_STRTAB));
    strings->addByte(0);

    ASpaceSection* aSpaces = dynamic_cast<ASpaceSection*>(
        Section::createSection(Section::ST_ADDRSP));

    ASpaceElement* undefASpace = new ASpaceElement();
    undefASpace->setName(strings->string2Chunk(""));
    aSpaces->setUndefinedASpace(undefASpace);

    // check instruction address space from adf and init instruction space
    Machine& adf = prog_.targetProcessor();
    ControlUnit* unit = adf.controlUnit();
    AddressSpace* adfInstrASpace = unit->addressSpace();

    if (prog_.instructionCount() > 0 && 
        (prog_.startAddress().location() < adfInstrASpace->start() ||
        prog_.startAddress().location() + prog_.instructionCount() > 
        adfInstrASpace->end())) {
        TCEString err = 
            "The program is out of bounds of the imem." 
            " Please increase the instruction address space size in adf or "
            " make the program smaller. "
            "Using a smaller unroll and/or inlining threshold may help.";
        err << " Imem address space size: "
            << (adfInstrASpace->end() - adfInstrASpace->start() +1)
            << ", required program size: "
            << prog_.instructionCount();

        throw CompileError(__FILE__, __LINE__, __func__, err);
    }

    aSpaces->addElement(undefASpace);

    ResourceSection* resources = dynamic_cast<ResourceSection*>(
        Section::createSection(Section::ST_MR));

    CodeSection* code =
        dynamic_cast<CodeSection*>(Section::createSection(Section::ST_CODE));

    SymbolSection* symbols =
        dynamic_cast<SymbolSection*>(
            Section::createSection(Section::ST_SYMTAB));

    NoTypeSymElement *undefSymbol = new NoTypeSymElement();
    undefSymbol->setName(strings->string2Chunk(""));
    symbols->addElement(undefSymbol);

    // add sections to binary
    newBin->addSection(nullSection);
    newBin->addSection(aSpaces);
    newBin->addSection(strings);
    newBin->addSection(resources);
    newBin->addSection(code);
    newBin->addSection(symbols);
    newBin->setStrings(strings);

    // set link fields
    nullSection->setLink(nullSection);
    strings->setLink(nullSection);
    aSpaces->setLink(strings);
    resources->setLink(strings);
    code->setLink(resources);
    symbols->setLink(strings);

    // set name fields
    nullSection->setName(strings->string2Chunk(""));
    strings->setName(strings->string2Chunk(""));
    aSpaces->setName(strings->string2Chunk(""));
    resources->setName(strings->string2Chunk(""));
    code->setName(strings->string2Chunk(""));
    symbols->setName(strings->string2Chunk(""));

    // set adress space fields
    nullSection->setASpace(undefASpace);
    strings->setASpace(undefASpace);
    aSpaces->setASpace(undefASpace);
    resources->setASpace(undefASpace);
    code->setASpace(&createASpaceElement(*adfInstrASpace, *newBin));
    symbols->setASpace(undefASpace);

    TPEFResourceUpdater resourceUpdater(prog_.targetProcessor(), *resources);

    createCodeSection(code, resourceUpdater);
    createDataSections(newBin, adf.isLittleEndian());

    // TODO: add labels here (all labels are inserted at once from skope
    // information after writing porgram sections)

    // TODO: refactor this...

    // procedure symbols
    for (int i = 0; i < prog_.procedureCount(); i++) {
        Procedure &currProced = prog_.procedure(i);

        // create CodeSymElement and add it to symbols
        ProcedSymElement *procedSym = new ProcedSymElement();

        procedSym->setAbsolute(false);
        procedSym->setBinding(SymbolElement::STB_LOCAL);

        procedSym->setName(
            strings->string2Chunk(currProced.name()));

        procedSym->setSection(code);

        procedSym->setReference(
            &code->instruction(currProced.startAddress().location()));

        symbols->addElement(procedSym);
    }

    // we add only global scope labels for now
    const GlobalScope &globalScope = prog_.globalScopeConst();

    for (int i = 0; i < globalScope.globalCodeLabelCount(); i++) {
        const CodeLabel &currLabel = globalScope.globalCodeLabel(i);

        // create CodeSymElement and add it to symbols
        CodeSymElement *codeSym = new CodeSymElement();

        codeSym->setAbsolute(false);
        codeSym->setBinding(SymbolElement::STB_GLOBAL);

        codeSym->setName(
            strings->string2Chunk(currLabel.name()));

        codeSym->setSection(code);

        codeSym->setReference(
            &code->instruction(currLabel.address().location()));

        symbols->addElement(codeSym);
    }

    for (int i = 0; i < globalScope.globalDataLabelCount(); i++) {
        const DataLabel &currLabel = globalScope.globalDataLabel(i);

        // create DataSymElement and add it to symbols
        DataSymElement *dataSym = new DataSymElement();

        dataSym->setAbsolute(false);
        dataSym->setBinding(SymbolElement::STB_GLOBAL);

        dataSym->setName(strings->string2Chunk(currLabel.name()));

        // TODO: find section by address. 
        //       refactor to own method (Method is already written)

        // it seems that we need to have some kind of data 
        // section finding by address
        ASpaceSection* aSpaces = dynamic_cast<ASpaceSection*>(
            newBin->section(Section::ST_ADDRSP,0));

        ASpaceElement* dstASpace = NULL;

        // find out dst address space by comparing names
        for (Word k = 0; k < aSpaces->elementCount(); k++) {
            ASpaceElement *currElem = dynamic_cast<ASpaceElement*>(
                aSpaces->element(k));

            if (TPEFTools::addressSpaceName(*newBin, *currElem) ==
                currLabel.address().space().name()) {

                dstASpace = currElem;
                break;
            }
        }

        if (dstASpace == NULL) {
            throw NotAvailable(
                __FILE__, __LINE__, __func__, 
                std::string("Cannot find address space ") + 
                currLabel.address().space().name() + 
                " for data label " + currLabel.name());
                               
        }
        UDataSection* dstSection = NULL;
        Word dstAddress = currLabel.address().location();

        for (Word k = 0; k < newBin->sectionCount(); k++) {
            UDataSection *currSect = dynamic_cast<UDataSection*>(
                newBin->section(k));

            if (currSect != NULL &&
                (currSect->type() == Section::ST_DATA ||
                 currSect->type() == Section::ST_UDATA||
                 currSect->type() == Section::ST_LEDATA) &&
                currSect->aSpace() == dstASpace &&
                currSect->startingAddress() <= dstAddress &&
                dstAddress <
                currSect->startingAddress() + currSect->lengthInMAUs()) {

                dstSection = currSect;
                break;
            }
        }

        if (dstSection == NULL) {
            abortWithError("Can't find section for data address: " +
                           Conversion::toString(dstAddress));
        }

        dataSym->setSection(dstSection);

        dstAddress -= dstSection->startingAddress();

        dataSym->setReference(
            dstSection->chunk(
                dstSection->MAUsToBytes(dstAddress)));

        symbols->addElement(dataSym);
    }
    
    // all the data is written... do the relocations
    createRelocSections(newBin);

    // clean up internal tables
    aSpaceMap_.clear();
    relocInfos_.clear();

    newBin->setArch(Binary::FA_TTA_TUT);
    newBin->setType(resolveFileType(*resources));

    return newBin;
}

/**
 * Creates code section.
 *
 * @param code Code section where to instructions are added.
 * @param updater Resource updater for finding TPEF resources for machine
 *                parts.
 */
void
ProgramWriter::createCodeSection(
    CodeSection* code,
    TPEFResourceUpdater& updater) const {

    // add all busses from machine to resource section
    Machine& adf = prog_.targetProcessor();
    Machine::BusNavigator navi = adf.busNavigator();
    for (int i = 0; i < navi.count(); i++) {
        // just call once for each bus, to make sure that all busses
        // are written to machine resource table.
        updater.bus(*navi.item(i));
    }

    code->setStartingAddress(prog_.startAddress().location());

    for (int i = 0; i < prog_.procedureCount(); i++) {
        Procedure &currProcedure = prog_.procedure(i);

        for (int j = 0; j < currProcedure.instructionCount(); j++) {
            Instruction &currInstr = currProcedure.instructionAtIndex(j);
            HalfWord immediateIndex = 0;
            bool beginFlag = true;

            for (int k = 0; k < currInstr.moveCount(); k++) {
                Move& progMove = currInstr.move(k);
                MoveElement* tpefMove = new MoveElement();

                tpefMove->setBegin(beginFlag);
                beginFlag = false;
                tpefMove->setBus(updater.bus(progMove.bus()).id());
                tpefMove->setEmpty(false);

                // add the possible move annotations
                if (progMove.hasAnnotations()) {
                    for (int annotationIndex = 0; 
                         annotationIndex < progMove.annotationCount(); 
                         ++annotationIndex) {
                        const ProgramAnnotation& annot =
                            progMove.annotation(annotationIndex);
                        tpefMove->addAnnotation(
                            new InstructionAnnotation(
                                annot.id(), annot.payload()));
                    }
                }

                code->addElement(tpefMove);

                if (!tpefMove->isEmpty()) {

                    // set source
                    if (!progMove.source().isImmediate()) {

                        ResourceID sourceId =
                            terminalResource(progMove.source(), updater);

#if 0
                        std::cerr << "src type: " << sourceId.type
                                  << "\tunit: " << sourceId.unit
                                  << "\tindex: " << sourceId.index
                                  << std::endl;
#endif

                        tpefMove->setSourceType(sourceId.type);
                        tpefMove->setSourceUnit(sourceId.unit);
                        tpefMove->setSourceIndex(sourceId.index);

                    } else {

                        ImmediateElement *newImmediate = 
                            new ImmediateElement();
                        newImmediate->setBegin(false);

                        const TTAProgram::TerminalImmediate& termImm =
                            dynamic_cast<TerminalImmediate&>(
                                progMove.source());

                        // add the possible immediate annotations
                        if (termImm.hasAnnotations()) {
                            for (int annotationIndex = 0; 
                                 annotationIndex < termImm.annotationCount(); 
                                 ++annotationIndex) {
                                const ProgramAnnotation& annot =
                                    termImm.annotation(annotationIndex);
                                newImmediate->addAnnotation(
                                    new InstructionAnnotation(
                                        annot.id(), annot.payload()));
                            }
                        }

                        unsigned int uvalue = 
                            progMove.source().value().unsignedValue();

                        // check that inline immediate fits to bus's 
                        // inline immediate field
                        unsigned int wordToStore =
                            progMove.source().value().unsignedValue();

                        int requiredBits = 0;
                        int fieldWidth = progMove.bus().immediateWidth();

                        if (progMove.bus().signExtends()) {
                            // Interpret as signed and sign extend if needed
                            int svalue = static_cast<int>(uvalue);
                            if (fieldWidth < MAX_SIMM_WIDTH) {
                                svalue = MathTools::signExtendTo(
                                    svalue, fieldWidth);
                            }
                            requiredBits = 
                                MathTools::requiredBitsSigned(svalue);
                        } else {
                            requiredBits = 
                                MathTools::requiredBits(uvalue);
                        }
                      
                        if (requiredBits <= fieldWidth) {
                            wordToStore = 
                                MathTools::zeroExtendTo(
                                    wordToStore, fieldWidth);
                        } else {
                            TCEString disasm = POMDisassembler::disassemble(progMove);
                            int location =  
                                progMove.parent().address().location();
                            TCEString message = "In procedure:";
                            message << currProcedure.name() <<  " Move: " <<
                                disasm << " Inline immediate value " <<
                                progMove.source().value().unsignedValue() <<
                                " of required width " <<
                                Conversion::toString(requiredBits) <<
                                " at location " <<
                                Conversion::toString(location) <<
                                " doesn't fit to bus: " <<
                                progMove.bus().name();

                            throw NotAvailable(
                                __FILE__, __LINE__, __func__, message);
                        }   
                        
                        newImmediate->setWord(wordToStore);
                        newImmediate->setDestinationUnit(
                            ResourceElement::INLINE_IMM);
                        newImmediate->setDestinationIndex(immediateIndex);
                        immediateIndex++;

                        code->addElement(newImmediate);

                        tpefMove->setSourceType(MoveElement::MF_IMM);
                        tpefMove->setSourceUnit(
                            newImmediate->destinationUnit());
                        tpefMove->setSourceIndex(
                            newImmediate->destinationIndex());

                        // add relocation antries if needed
                        if (progMove.source().isInstructionAddress()) {
                            
                            // TODO: check size of inline immediate field
                            RelocInfo newReloc(
                                code, newImmediate, 
                                progMove.source().address(),
                                IMMEDIATE_ADDRESS_WIDTH);
                            
                            relocInfos_.push_back(newReloc);
                            
                        } else if (progMove.source().isAddress()) {

                            // TODO: check size of inline immediate field
                            RelocInfo newReloc(
                                code, newImmediate, 
                                progMove.source().address(),
                                IMMEDIATE_ADDRESS_WIDTH);

                            relocInfos_.push_back(newReloc);

                        }
                    }

                    // set destination
                    ResourceID destinationId =
                        terminalResource(progMove.destination(), updater);
#if 0
                    std::cerr << "dst type: " << destinationId.type
                              << "\tunit: " << destinationId.unit
                              << "\tindex: " << destinationId.index
                              << std::endl;
#endif
                    tpefMove->setDestinationType(destinationId.type);
                    tpefMove->setDestinationUnit(destinationId.unit);
                    tpefMove->setDestinationIndex(destinationId.index);

                    // set guard stuff
                    if (progMove.isUnconditional()) {
                        tpefMove->setGuarded(false);
                    } else {

                        const Guard *guard = &progMove.guard().guard();

                        tpefMove->setGuarded(true);

                        tpefMove->setGuardInverted(guard->isInverted());

                        const PortGuard *portGuard =
                            dynamic_cast<const PortGuard*>(guard);
                        const RegisterGuard *registerGuard =
                            dynamic_cast<const RegisterGuard*>(guard);

                        if (portGuard != NULL) {
                            tpefMove->setGuardType(MoveElement::MF_UNIT);

                            FunctionUnit &funcUnit = 
                                *portGuard->port()->parentUnit();

                            ResourceElement &fu = 
                                updater.functionUnit(funcUnit);
                            tpefMove->setGuardUnit(fu.id());

                            // is operation or special register port
                            FunctionUnit *parentFu =
                                portGuard->port()->parentUnit();

                            const FUPort *fuPort = portGuard->port();

                            if (parentFu->hasOperationPort(fuPort->name())) {
                                HWOperation *oper = NULL;

                                // find just any operation that is 
                                // bound to guarded port
                                for (int i = 0; 
                                     i < parentFu->operationCount(); i++) {
                                    oper = parentFu->operation(i);

                                    if (oper->isBound(*fuPort)) {
                                        break;
                                    }

                                    oper = NULL;
                                }

                                assert(oper != NULL);

                                ResourceElement &opPort =
                                    updater.operand(
                                        *oper, oper->io(*fuPort));

                                tpefMove->setGuardIndex(opPort.id());

                            } else {
                                ResourceElement &fuPortResource =
                                    updater.functionUnitPort(*fuPort);

                                tpefMove->setGuardIndex(fuPortResource.id());
                            }

                        } else if (registerGuard != NULL) {
                            tpefMove->setGuardType(MoveElement::MF_RF);

                            RegisterFile &regFile = 
                                *registerGuard->registerFile();

                            ResourceElement &rf = 
                                updater.registerFile(regFile);

                            tpefMove->setGuardUnit(rf.id());

                            tpefMove->setGuardIndex(
                                registerGuard->registerIndex());

                        } else {
                            abortWithError("Unknown machine guard type.");
                        }
                    }
                }
            }

            // write long immediates
            for (int k = 0; k < currInstr.immediateCount(); k++) {
                Immediate& imm = currInstr.immediate(k);

                ImmediateElement* tpefImm = new ImmediateElement();

                tpefImm->setBegin(beginFlag);
                beginFlag = false;

                if (imm.destination().immediateUnit().signExtends()) {
                    tpefImm->setSignedWord(imm.value().value().sIntWordValue());
                } else {
                    tpefImm->setWord(imm.value().value().uIntWordValue());
                }

                ResourceID dstRes =
                    terminalResource(imm.destination(), updater);

                tpefImm->setDestinationUnit(dstRes.unit);
                tpefImm->setDestinationIndex(dstRes.index);

                code->addElement(tpefImm);

                // add relocation antries if needed
                if (imm.value().isInstructionAddress()) {
                    
                    // TODO: check size of long immediate field
                    RelocInfo newReloc(
                        code, tpefImm, 
                        imm.value().address(),
                        IMMEDIATE_ADDRESS_WIDTH);
                    
                    relocInfos_.push_back(newReloc);

                } else if (imm.value().isAddress()) {
                            
                    // TODO: check size of long immediate field
                    RelocInfo newReloc(
                        code, tpefImm, 
                        imm.value().address(),
                        IMMEDIATE_ADDRESS_WIDTH);
                    
                    relocInfos_.push_back(newReloc);

                }
            }

            // add empty instruction (instruction containing one empty move)
            if (currInstr.moveCount() == 0 &&
                currInstr.immediateCount() == 0) {

                MoveElement *tpefNOP = new MoveElement();
                tpefNOP->setBegin(true);
                tpefNOP->setEmpty(true);
                code->addElement(tpefNOP);
            }
        }
    }
}


/**
 * Finds out resource element and index by terminal.
 *
 * @param term Terminal whose TPEF resource information is needed.
 * @param updater Resource updater for finding TPEF resources.
 */
ProgramWriter::ResourceID
ProgramWriter::terminalResource(
    const Terminal& term,
    TPEFResourceUpdater& updater) const {

    ResourceID retVal;
    retVal.type = MoveElement::MF_RF;
    retVal.unit = 0;
    retVal.index = 0;
    
    try {

        if (term.isImmediate()) {
            abortWithError(
                "This function should never be called with ImmediateTerminal "
                " instance type.");

        } else if (term.isGPR()) {
            retVal.type = MoveElement::MF_RF;
            retVal.unit = updater.registerFile(term.registerFile()).id();
            retVal.index = term.index();

        } else if (term.isImmediateRegister()) {
            retVal.type = MoveElement::MF_IMM;
            retVal.unit = updater.immediateUnit(term.immediateUnit()).id();
            retVal.index = term.index();

        }  else if (term.isFUPort()) {

            retVal.type = MoveElement::MF_UNIT;

            FunctionUnit &fu =
                *dynamic_cast<FunctionUnit*>(term.port().parentUnit());

            retVal.unit = updater.functionUnit(fu).id();

            const TerminalFUPort& fuTerm =
                dynamic_cast<const TerminalFUPort&>(term);

            // find if there is information of operation in terminal
            if (&fuTerm.hintOperation() != &NullOperation::instance() ||
                fuTerm.isOpcodeSetting()) {

                std::string operationName =
                    (fuTerm.isOpcodeSetting()) ?
                    (fuTerm.operation().name()) :
                    (fuTerm.hintOperation().name());

                if (fu.hasOperation(operationName)) {
                    HWOperation *oper = fu.operation(operationName);

                    int index = oper->io(
                        dynamic_cast<const FUPort&>(term.port()));

                    retVal.index = updater.operand(*oper, index).id();

                } else {
                    abortWithError(
                        "Can't find operation " + operationName +
                        " from FU: " + fu.name());
                }

            } else {
                // not opcode setting normal fu port without operation hint
                assert(!fuTerm.isOpcodeSetting());
                ResourceElement &fuPort = 
                    updater.functionUnitPort(term.port());
                retVal.index = fuPort.id();
            }

        } else {
            abortWithError("Unknown terminal type.");
        }

    } catch (const InvalidData& e) {
        NotAvailable error(__FILE__, __LINE__, __func__,
                           "Problems with finding terminal: " + 
                           e.errorMessage());

        error.setCause(e);

        throw error;
    } 

    return retVal;
}


/**
 * Creates data section to TPEF.
 *
 * @param bin Binary to write the sections to.
 */
void
ProgramWriter::createDataSections(Binary* bin, bool littleEndian) const {
    
    std::map<AddressSpace*, ASpaceElement*> aSpaceMap;

    ASpaceSection* aSpaceSection = dynamic_cast<ASpaceSection*>(
        bin->section(Section::ST_ADDRSP, 0));

    StringSection* strings =
        dynamic_cast<StringSection*>(aSpaceSection->link());
    
    // go through all the DataMemories of POM
    for (int i = 0; i < prog_.dataMemoryCount(); i++) {
        DataMemory& currMem = prog_.dataMemory(i);        

        UDataSection* currSect = NULL;    

        for (int j = 0; j < currMem.dataDefinitionCount(); j++) {
            DataDefinition& currDef = currMem.dataDefinition(j);

            assert(&currDef.startAddress().space() == 
                   &currMem.addressSpace());
            
            // create new data dection if needed
            if (currSect == NULL ||
                
                (currDef.isInitialized() && 
                 !currSect->isDataSection()) ||
                
                (!currDef.isInitialized() && 
                 currSect->isDataSection()) ||
                
                (currDef.startAddress().location() !=
                 currSect->startingAddress() +
                 currSect->lengthInMAUs())) {
                
                if (currDef.isInitialized()) {
                    if (!littleEndian) {
                        currSect = dynamic_cast<DataSection*>(
                            Section::createSection(Section::ST_DATA));
                    } else {
                        currSect = dynamic_cast<DataSection*>(
                            Section::createSection(Section::ST_LEDATA));
                    }
                } else {
                    currSect = dynamic_cast<UDataSection*>(
                        Section::createSection(Section::ST_UDATA));
                }
               
                // add section to binary
                assert (currSect != NULL);
                bin->addSection(currSect);
                
                currSect->setStartingAddress(
                    currDef.startAddress().location());                

                currSect->setName(strings->string2Chunk(""));                
                
                currSect->setLink(bin->section(Section::ST_NULL,0));
                
                const AddressSpace& adfASpace =  currMem.addressSpace();
                currSect->setASpace(&createASpaceElement(adfASpace, *bin));
            }
            
            // add data to section
            if (currDef.isInitialized()) {
                DataSection* dataSect = dynamic_cast<DataSection*>(currSect);
                
                int byteOffset = dataSect->length();
                
                for (int k = 0; k < currDef.size(); k++) {
                    dataSect->addMAU(currDef.MAU(k));
                }
                
                if (currDef.isAddress()) {
                    RelocInfo newReloc(
                        dataSect, dataSect->chunk(byteOffset),
                        currDef.destinationAddress(),
                        currDef.size() * dataSect->aSpace()->MAU());

                    relocInfos_.push_back(newReloc);
                }
                
            } else {
                currSect->setLengthInMAUs(
                    currSect->lengthInMAUs() + currDef.size());
            }


#if 0
            // Prints debug data of all encountered data definitions.
            if (currSect != NULL) {
                Application::logStream()
                    << "datadef: addr: " 
                    << currDef.startAddress().space().name() << ":"
                    << currDef.startAddress().location()
                    << "\t" << "size: " << currDef.size()
                    << "\t" << "init: " << currDef.isInitialized()
                    << "\t" << "isAddr: " << currDef.isAddress();
                if (currDef.isAddress()) {
                    Application::logStream()
                        << "\t" << "dest: "
                        << currDef.destinationAddress().space().name() << ":"
                        << currDef.destinationAddress().location();
                }
                Application::logStream()
                    << std::endl;
            }
#endif

#if 0
            // prints out debug data of all created data sections
            if (currSect != NULL) {
                DataSection* dSectTmp = dynamic_cast<DataSection*>(currSect);
                Application::logStream()
                    << "data section " << currSect << " length: " 
                    << currSect->lengthInMAUs()
                    << "\tstart address: " 
                    << TPEFTools::addressSpaceName(
                        *bin, *currSect->aSpace())
                    << ":" << currSect->startingAddress()
                    << "\tinitialized: " 
                    // << static_cast<int>(currSect->isDataSection())
                    << (dSectTmp?true:false)
                    << std::endl;
            }
#endif

        }
    }
}

/**
 * Creates new address space element to binary or returns already created one.
 *
 * This function should be used always with same TPEF bin. 
 *
 * @param addressSpace ADF address space whose TPEF version is needed.
 * @param bin Binary to which created address space is added.
 * @return Address space element of corresponding ADF address space.
 */
TPEF::ASpaceElement&
ProgramWriter::createASpaceElement(
    const TTAMachine::AddressSpace& addressSpace, TPEF::Binary& bin)  const {

    StringSection* strings = dynamic_cast<StringSection*>(
        bin.section(Section::ST_STRTAB,0));
    Section* aSpaceSection = bin.section(Section::ST_ADDRSP,0);
    
    // create new address space if necessary
    if (!MapTools::containsKey(aSpaceMap_, &addressSpace)) {
        ASpaceElement *newASpace = new ASpaceElement();

        // set mau of instruction address space to be 0
        if (&addressSpace ==  
            prog_.targetProcessor().controlUnit()->addressSpace()) {
            newASpace->setMAU(0);
        } else {
            newASpace->setMAU(addressSpace.width());
        }
        newASpace->setName(
            strings->string2Chunk(addressSpace.name()));
        aSpaceSection->addElement(newASpace);
        aSpaceMap_[&addressSpace] = newASpace;
    }
    
    return  *aSpaceMap_[&addressSpace];
}

/**
 * Create relocation tables to binary and add relocation elements.
 *
 * @param bin Binary where to add relocation sections.
 * @exception NotAvailable Can't find needed resource.
 */
void
ProgramWriter::createRelocSections(TPEF::Binary* bin) const {
    StringSection* strings = dynamic_cast<StringSection*>(
        bin->section(Section::ST_STRTAB,0));

    ASpaceSection* aSpaces = dynamic_cast<ASpaceSection*>(
        bin->section(Section::ST_ADDRSP,0));

    SymbolSection* symbols = dynamic_cast<SymbolSection*>(
        bin->section(Section::ST_SYMTAB, 0));
    
    RelocSection* currRelocs = NULL;
    
    for (int i = 0; i < static_cast<int>(relocInfos_.size()); i++) {
        RelocInfo& currReloc = relocInfos_[i];
        
        Section& dstSect = findSection(*bin, currReloc.destination);
        
        // create new reloc section if needed
        if (currRelocs == NULL || 
            currRelocs->referencedSection() != currReloc.srcSect) {

            currRelocs = dynamic_cast<RelocSection*>(
                Section::createSection(Section::ST_RELOC));            
            bin->addSection(currRelocs);

            currRelocs->setName(strings->string2Chunk(""));         
            currRelocs->setLink(symbols);
            currRelocs->setASpace(aSpaces->undefinedASpace());
            currRelocs->setReferencedSection(currReloc.srcSect);
        }

        SectionElement* dstElem = NULL;

        // get element by address (destination element)
        if (dstSect.type() == Section::ST_CODE) {
            CodeSection& codeSect = 
                dynamic_cast<CodeSection&>(dstSect);
            
            dstElem = &codeSect.instruction(
                currReloc.destination.location() - 
                codeSect.startingAddress());
            
        } else {
            UDataSection& dataSect = 
                dynamic_cast<UDataSection&>(dstSect);
            
            int byteOffset = dataSect.MAUsToBytes(
                currReloc.destination.location() - 
                dataSect.startingAddress());
            
            dstElem = dataSect.chunk(byteOffset);
        }

        RelocElement* newReloc = new RelocElement();
        currRelocs->addElement(newReloc);

        newReloc->setType(RelocElement::RT_SELF);
        
        newReloc->setLocation(currReloc.srcElem);

        newReloc->setDestination(dstElem);

        newReloc->setSymbol(
            dynamic_cast<SymbolElement*>(symbols->element(0)));

        newReloc->setASpace(
            &createASpaceElement(
                currReloc.destination.space(), *bin));
        
        newReloc->setSize(currReloc.bits);
        
    }
}

/**
 * Finds section which contain requested address.
 *
 * @param bin TPEF where from sections are searched.
 * @param address The address that are looked for.
 * @return The section which contains requested address.
 */
TPEF::Section& 
ProgramWriter::findSection(Binary& bin, Address address) const {

    for (int i = 0; i < static_cast<int>(bin.sectionCount()); i++) {
        Section& currSect = *bin.section(i);

        if (currSect.isProgramSection()) {
            if (currSect.startingAddress() <= address.location() && 
                &createASpaceElement(address.space(), bin) == 
                currSect.aSpace()) {
                
                if (currSect.type() == Section::ST_CODE) {
                    CodeSection& codeSect = 
                        dynamic_cast<CodeSection&>(currSect);

                    if (codeSect.instructionCount() +
                        codeSect.startingAddress() > address.location()) {
                        
                        return codeSect;
                    }
                } else {
                    UDataSection& dataSect = 
                        dynamic_cast<UDataSection&>(currSect);
                    
                    if (dataSect.startingAddress() +
                        dataSect.lengthInMAUs() > address.location()) {
                        
                        return dataSect;
                    }
                }
            }
        }
    }
    
    throw NotAvailable(
        __FILE__, __LINE__, __func__,
        "Can't find section containing address: " + address.space().name() +
        ":" + Conversion::toString(address.location()));
}


/**
 * Find out the kind of TPEF file type.
 *
 * This method scans the resource section and checks the type of processor
 * resource elements it contains. Based on their types, it figures out what
 * should be the file type.
 *
 * @param resources Resource section.
 * @return File type of TPEF binary.
 */
TPEF::Binary::FileType
ProgramWriter::resolveFileType(TPEF::ResourceSection& resources) const {

    bool univRefs = false;
    bool realRefs = false;

    for (unsigned int i = 0; i < resources.elementCount(); i++) {
        ResourceElement* res =
            dynamic_cast<ResourceElement*>(resources.element(i));

        switch(res->type()) {

        case ResourceElement::MRT_BUS:
            if (res->id() == ResourceElement::UNIVERSAL_BUS) {
                univRefs = true;
            } else {
                realRefs = true;
            }
            break;

        case ResourceElement::MRT_UNIT:
            if (res->id() == ResourceElement::UNIVERSAL_FU) {
                univRefs = true;
            } else {
                realRefs = true;
            }
            break;

        case ResourceElement::MRT_RF:
            if (res->id() == ResourceElement::INT_RF ||
                res->id() == ResourceElement::BOOL_RF ||
                res->id() == ResourceElement::FP_RF) {
                univRefs = true;
            } else {
                realRefs = true;
            }
            break;

        case ResourceElement::MRT_IMM:
            realRefs = true;
            break;

        default:
            ;// just omit MRT_PORT, MRT_SR and MRT_OP
        }
    }

    if (univRefs && realRefs) {
        return Binary::FT_MIXED;
    } else if (univRefs) {
        return Binary::FT_PURESEQ;
    } else if (!univRefs && realRefs) {
        return Binary::FT_PARALLEL;
    }

    return Binary::FT_NULL;
}

}
