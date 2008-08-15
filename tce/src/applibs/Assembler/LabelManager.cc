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
 * @file LabelManager.cc
 *
 * Definition of LabelManager class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen@tut.fi)
 *
 * @note rating: yellow
 */

#include "LabelManager.hh"
#include "MachineResourceManager.hh"

#include "Binary.hh"
#include "SymbolSection.hh"
#include "RelocSection.hh"
#include "RelocElement.hh"
#include "ASpaceElement.hh"
#include "DataSymElement.hh"
#include "CodeSymElement.hh"
#include "ProcedSymElement.hh"
#include "NoTypeSymElement.hh"
#include "StringSection.hh"
#include "ContainerTools.hh"
#include "SymbolElement.hh"
#include "CodeSection.hh"
#include "DataSection.hh"
#include "UDataSection.hh"
#include "SequenceTools.hh"
#include "MapTools.hh"
#include "Exception.hh"
#include "TPEFTools.hh"

using TPEF::Binary;
using TPEF::SymbolSection;
using TPEF::RelocSection;
using TPEF::RelocElement;
using TPEF::SectionElement;
using TPEF::ASpaceElement;
using TPEF::Section;
using TPEF::DataSymElement;
using TPEF::CodeSymElement;
using TPEF::ProcedSymElement;
using TPEF::NoTypeSymElement;
using TPEF::StringSection;
using TPEF::SymbolElement;
using TPEF::CodeSection;
using TPEF::UDataSection;
using TPEF::DataSection;
using TPEF::TPEFTools;

/**
 * Constructor.
 *
 * @param bin Binary containing data and code sections of relocations.
 * @param resources Resource manager for getting TPEF resources.
 * @param parent Assembler root class.
 */
LabelManager::LabelManager(
    Binary& bin, MachineResourceManager& resources, Assembler* parent) :
    bin_(bin), resources_(resources), parent_(parent) {
}

/**
 * Destructor.
 */
LabelManager::~LabelManager() {
    MapTools::deleteAllValues(labels_);

    while (!relocs_.empty()) {
        delete *relocs_.begin();
        relocs_.erase(relocs_.begin());
    }
}

/**
 * Returns value of a label by name.
 *
 * @param name Name of the requested label.
 * @return Value of the label.
 * @exception SymbolNotFound If there is not symbol for requested name.
 */
UValue
LabelManager::value(std::string &name)
    throw (SymbolNotFound) {

    if(!MapTools::containsKey(labels_, name)) {

        throw SymbolNotFound(
            __FILE__, __LINE__, __func__,
            "Can't find value for label: " + name);
    }

    return labels_[name]->value;
}

/**
 * Returns a TPEF address space element for a label by name.
 *
 * @param labelName Name of the requested label.
 * @return Address space of the label.
 * @exception SymbolNotFound If there is not symbol for requested name.
 */
ASpaceElement&
LabelManager::aSpaceElement(std::string& labelName)
    throw (SymbolNotFound) {

    if(!MapTools::containsKey(labels_, labelName)) {
        throw SymbolNotFound(
            __FILE__, __LINE__, __func__,
            "Can't find address address space for label: " + labelName);
    }

    return *labels_[labelName]->aSpace;
}

/**
 * Returns the name of the address space for a label.
 *
 * @param labelName Name of the requested label.
 * @return Name of the address space the label belongs in.
 * @exception SymbolNotFound If there is not symbol for requested name.
 */
std::string
LabelManager::aSpaceName(std::string& labelName)
    throw (SymbolNotFound) {
    return TPEFTools::addressSpaceName(bin_, aSpaceElement(labelName));
}


/**
 * Adds a label to manager.
 *
 * @param aSpace Address space of the label to add.
 * @param name Name of the label.
 * @param value Value for the label.
 * @exception ObjectAlreadyExists Label by the same name already exists.
 */
void
LabelManager::addLabel(
    ASpaceElement &aSpace, std::string &name, UValue value)
    throw (ObjectAlreadyExists) {

    if(MapTools::containsKey(labels_, name)) {
        throw ObjectAlreadyExists(
            __FILE__, __LINE__, __func__,
            "Multiply defined lablel: " + name);
    }

    InternalLabel *newElem = new InternalLabel();

    newElem->name = name;
    newElem->aSpace = &aSpace;
    newElem->value = value;

    labels_[name] = newElem;
}

/**
 * Removes a label from manager.
 *
 * @param name Name of the label.
 * @exception SymbolNotFound Label is not found.
 */
void
LabelManager::removeLabel(std::string &name)
    throw (SymbolNotFound) {

    if(!MapTools::containsKey(labels_, name)) {
        throw SymbolNotFound(
            __FILE__, __LINE__, __func__,
            "Can't find label: " + name);
    }

    labels_.erase(name);
}

/**
 * Adds a procedure to manager.
 *
 * @param name Name of the procedure
 * @param address Start address of the procedure.
 * @exception ObjectAlreadyExists Procedure with a same name is already added.
 */
void
LabelManager::addProcedure(
    std::string &name, UValue address)
    throw (ObjectAlreadyExists) {

    if(MapTools::containsKey(procedures_, name)) {
        throw ObjectAlreadyExists(
            __FILE__, __LINE__, __func__,
            "Multiply defined procedure: " + name);
    }

    procedures_[name] = address;
}

/**
 * Set's label to be global label.
 *
 * @param labelName Name of label to set global.
 */
void
LabelManager::setGlobal(std::string &labelName) {
    globals_.insert(labelName);
}

/**
 * Adds relocated immediate or data chunk.
 *
 * @param locationSect Section where relocated element is stored.
 * @param location Immediate or Chunk containing value to relocate.
 * @param dstASpace Destination address space of relocation.
 * @param destination Destination address of relocation.
 * @param bitWidth Size of the relocated field.
 */
void
LabelManager::addRelocation(
    Section& locationSect, 
    SectionElement& location,
    ASpaceElement& dstASpace, 
    UValue destination, 
    UValue bitWidth) {
    
    InternalRelocation *newElem = new InternalRelocation();
    
    newElem->locationSect = &locationSect;
    newElem->location = &location;
    newElem->dstASpace = &dstASpace;
    newElem->destination = destination;
    newElem->bitWidth = bitWidth;
 
    uncommittedRelocs_.insert(newElem);
}

/**
 * Removes relocations that are added but not committed to manager.
 *
 * Used for cleaning up in error cases.
 */
void
LabelManager::clearLastRelocations() {
    SequenceTools::deleteAllItems(uncommittedRelocs_);
}

/**
 * Frees all internally allocated data.
 */
void
LabelManager::cleanup() {
    SequenceTools::deleteAllItems(relocs_);
    SequenceTools::deleteAllItems(uncommittedRelocs_);
    MapTools::deleteAllValues(labels_);
    globals_.clear();
    procedures_.clear();
}

/**
 * Adds permanently relocations that are added but not yet
 * committed to manager.
 */
void
LabelManager::commitLastRelocations() {

    // add last relocations for writing down
    for (std::set<InternalRelocation*>::iterator iter =
             uncommittedRelocs_.begin();
         iter != uncommittedRelocs_.end(); iter++) {

        relocs_.insert(*iter);
    }

    uncommittedRelocs_.clear();
}

/**
 * Creates symbol and relocation tables corresponding to input information.
 *
 * All destinations must exist in TPEF before this method can be executed,
 * otherwise finalize will throw exception. If exception is thrown finalize
 * method will not do any changes to TPEF. When error that caused the
 * exception is fixed, method can be re-executed and it continues from the
 * same phase until the end.
 *
 * @exception CompileError There was problems during the resolving.
 */
void
LabelManager::finalize() throw (CompileError) {

    SymbolSection* symbolSect = NULL;

    // write labels
    StringSection* strings = resources_.stringSection();
    NoTypeSymElement* undefSymbol = NULL;

    // create always symbol section with undefined symbol
    symbolSect = dynamic_cast<SymbolSection*>(
        Section::createSection(Section::ST_SYMTAB));

    try {
        symbolSect->setASpace(resources_.undefinedAddressSpace());

        symbolSect->setLink(strings);

        undefSymbol = new NoTypeSymElement();
        undefSymbol->setName(strings->string2Chunk(""));
        symbolSect->addElement(undefSymbol);

        // write code and data labels
        for (SymbolMap::iterator iter = labels_.begin();
             iter != labels_.end(); iter++) {

            InternalLabel *currLabel = (*iter).second;

            Section *ownerSection = findSectionByAddress(currLabel);
            assert(ownerSection != NULL);

            SymbolElement *symbol =
                createSymbolWithReference(currLabel, ownerSection);
            assert(symbol != NULL);

            symbol->setSection(ownerSection);
            symbol->setName(strings->string2Chunk(currLabel->name));
            symbol->setAbsolute(false);

            // check if global
            if (ContainerTools::containsValue(globals_, currLabel->name)) {
                symbol->setBinding(SymbolElement::STB_GLOBAL);
                globals_.erase(currLabel->name);
            } else {
                symbol->setBinding(SymbolElement::STB_LOCAL);
            }

            symbolSect->addElement(symbol);
        }

        // if there is global definition without corresponding label
        if (!globals_.empty()) {
            throw CompileError(
                __FILE__, __LINE__, __func__,
                "Can't set label to be global. " +
                *(globals_.begin()) + " is undefined.");
        }

        // write procedure symbols
        if (bin_.sectionCount(Section::ST_CODE) != 0) {

            CodeSection *codeSect =
                dynamic_cast<CodeSection*>(
                    bin_.section(Section::ST_CODE, 0));

            assert(codeSect != NULL);
            writeProcedureSymbols(codeSect, symbolSect, strings);

        } else if (procedures_.size() != 0) {
            throw CompileError(
                __FILE__, __LINE__, __func__,
                "Invalid procedure declaration: " +
                (*procedures_.begin()).first);
        }

        // write relocations
        for (std::set<InternalRelocation*>::iterator iter = relocs_.begin();
             iter != relocs_.end(); iter++) {

            InternalRelocation *currReloc = *iter;
            
            // find relocation section for corresponding location section
            RelocSection* relocSect =
                findOrCreateRelocationSection(currReloc, symbolSect);
            assert(relocSect != NULL);

            // find destination section
            Section* dstSect = findRelocationDstSection(currReloc);
            
            if (dstSect == NULL) {
                throw CompileError(
                    __FILE__, __LINE__, __func__,
                    "Can't find destination section for address: "
                    + Conversion::toString(currReloc->destination) +
                    " of adress space: " +
                    TPEFTools::addressSpaceName(bin_, *currReloc->dstASpace));
            }
            
            RelocElement *newReloc = new RelocElement();

            newReloc->setLocation(currReloc->location);
            
            Word dstIndex =
                currReloc->destination - dstSect->startingAddress();

            // set destination element
            if (dstSect->type() == Section::ST_CODE) {
                CodeSection* codeSect =
                    dynamic_cast<CodeSection*>(dstSect);
                
                newReloc->setDestination(&codeSect->instruction(dstIndex));
                
            } else if (dstSect->type() == Section::ST_DATA ||
                       dstSect->type() == Section::ST_UDATA) {
                
                UDataSection* uDataSect =
                    dynamic_cast<UDataSection*>(dstSect);
                
                Word byteOffset = uDataSect->MAUsToBytes(dstIndex);
               
                newReloc->setDestination(uDataSect->chunk(byteOffset));

            } else {
                abortWithError("Unknown dst section of relocation.");
            }

            newReloc->setASpace(currReloc->dstASpace);
            newReloc->setType(RelocElement::RT_SELF);
            newReloc->setChunked(false);
            newReloc->setSymbol(undefSymbol);
            newReloc->setSize(currReloc->bitWidth);

            relocSect->addElement(newReloc);
        }

    } catch(CompileError& e) {
        delete symbolSect;
        throw e;
    }

    bin_.addSection(symbolSect);

    cleanup();
}

/**
 * Returns section that contains the address stored in label.
 *
 * @param currLabel Label whose destination section is returned.
 * @return Section that contains the address stored in label.
 */
Section*
LabelManager::findSectionByAddress(InternalLabel *currLabel) {

    // find section by address space
    for (Word i = 0; i < bin_.sectionCount(); i++) {
        Section *currSect = bin_.section(i);

        if (currSect->aSpace() == currLabel->aSpace) {

            // address must be found from section
            if (currSect->type() == Section::ST_DATA ||
                currSect->type() == Section::ST_UDATA) {

                UDataSection* sect =
                    dynamic_cast<UDataSection*>(currSect);

                assert(sect != NULL);

                if (currLabel->value >= sect->startingAddress() &&
                    currLabel->value - sect->startingAddress() <
                    sect->lengthInMAUs()) {

                    return currSect;
                }

            } else if (currSect->type() == Section::ST_CODE) {

                CodeSection* sect = dynamic_cast<CodeSection*>(currSect);

                assert(sect != NULL);

                if (currLabel->value >= sect->startingAddress() &&
                    currLabel->value - sect->startingAddress() <
                    sect->instructionCount()) {

                    return currSect;
                }

            } else {
                abortWithError("Label must point to data or code section.");
            }
        }

    }
    return NULL;
}

/**
 * Creates new TPEF SymbolElement and sets reference field of symbol.
 *
 * @param currLabel Label which for symbol element is created.
 * @param ownerSection Section who contains element referred by label.
 * @return Dynamically allocated TPEF symbol with reference field set.
 */
SymbolElement*
LabelManager::createSymbolWithReference(
    InternalLabel *currLabel,
    Section* ownerSection) {

    SymbolElement *symbol = NULL;

    if (ownerSection->type() == Section::ST_CODE) {
        CodeSymElement *codeSym = new CodeSymElement();
        symbol = codeSym;

        CodeSection *codeSection =
            dynamic_cast<CodeSection*>(ownerSection);

        codeSym->setReference(
            &codeSection->instruction(currLabel->value));

    } else {
        DataSymElement *dataSym = new DataSymElement();
        symbol = dataSym;

        UDataSection *uDataSection =
            dynamic_cast<UDataSection*>(ownerSection);

        dataSym->setReference(
            uDataSection->chunk(
                uDataSection->MAUsToBytes(
                    currLabel->value - ownerSection->startingAddress())));
    }

    return symbol;
}

/**
 * Writes all procedure symbols of code section to symbol section.
 *
 * @param codeSect Section whose symbols are written.
 * @param symbolSect Section where to symbols are written.
 * @param string Sectiong where symbol strings are stored.
 * @exception CompileError Procedure declaration is is not in code section.
 */
void
LabelManager::writeProcedureSymbols(
    CodeSection* codeSect, SymbolSection* symbolSect, StringSection* strings)
    throw (CompileError) {

    for (std::map<std::string, UValue>::iterator
             iter = procedures_.begin();
         iter != procedures_.end(); iter++) {

        std::string name = (*iter).first;
        UValue address = (*iter).second;

        if (address >=
            codeSect->instructionCount() +
            codeSect->startingAddress()) {

            throw CompileError(
                __FILE__, __LINE__, __func__,
                "Invalid procedure declaration: " +
                name);
        }

        ProcedSymElement *procedSym = new ProcedSymElement();

        procedSym->setReference(
            &codeSect->instruction(
                address - codeSect->startingAddress()));

        procedSym->setSection(codeSect);
        procedSym->setName(strings->string2Chunk(name));
        procedSym->setAbsolute(false);
        procedSym->setBinding(SymbolElement::STB_LOCAL);

        symbolSect->addElement(procedSym);
    }
}

/**
 * Finds or creates relocation section for relocation.
 *
 * @param currReloc Relocation whose section is looked for.
 * @param symbolSect Strings for created section.
 * @return Suitable relocation section for requested relocation.
 */
RelocSection*
LabelManager::findOrCreateRelocationSection(
    InternalRelocation* currReloc, SymbolSection* symbolSect) {

    RelocSection* relocSect = NULL;

    for (Word j = 0; j < bin_.sectionCount(Section::ST_RELOC); j++) {
        RelocSection* currSect =
            dynamic_cast<RelocSection*>(
                bin_.section(Section::ST_RELOC, j));

        if (currSect->referencedSection() ==
            currReloc->locationSect) {
            return currSect;
        }
    }

    // none found, create new reloc section
    if (relocSect == NULL) {
        relocSect = dynamic_cast<RelocSection*>(
            Section::createSection(Section::ST_RELOC));

        relocSect->setReferencedSection(currReloc->locationSect);
        relocSect->setASpace(resources_.undefinedAddressSpace());
        relocSect->setLink(symbolSect);

        bin_.addSection(relocSect);
    }

    return relocSect;
}

/**
 * Returns destination section of the relocation.
 *
 * @param currReloc Relocation whose destination is wanted.
 * @return Destination section of the relocation.
 */
Section*
LabelManager::findRelocationDstSection(InternalRelocation* currReloc) {

    for (Word j = 0; j < bin_.sectionCount(); j++) {
        Section* currSect = bin_.section(j);

        // check address space and starting address
        if (currSect->aSpace() == currReloc->dstASpace &&
            currSect->startingAddress() <= currReloc->destination) {

            if (currSect->type() == Section::ST_CODE) {
                CodeSection* codeSect = dynamic_cast<CodeSection*>(currSect);

                // check that section is enough long
                if (codeSect->instructionCount() +
                    codeSect->startingAddress() > currReloc->destination) {
                    
                    return currSect;
                }
                
            } else if (currSect->type() == Section::ST_DATA ||
                       currSect->type() == Section::ST_UDATA) {
                
                UDataSection* uDataSect =
                    dynamic_cast<UDataSection*>(currSect);

                // check that section is enough long
                if (uDataSect->lengthInMAUs() +
                    uDataSect->startingAddress() > currReloc->destination) {

                    return currSect;
                }
            }
        }
    }

    return NULL;
}


/**
 * Resolves value of label expression.
 *
 * @param asmLineNumber Current assembly code line number error information.
 * @param litOrExpr Expression whose value to resolve.
 * @param labels Label manager for resolving label values.
 * @return Value of expression.
 * @exception CompileError There was error with resolving value.
 */
UValue
LabelManager::resolveExpressionValue(
    UValue asmLineNumber,
    LiteralOrExpression& litOrExpr)
    throw (CompileError) {

    UValue retVal = 0;

    try {
        retVal = value(litOrExpr.expression.label);

    } catch (SymbolNotFound& e) {
        CompileError error(
            __FILE__, __LINE__, __func__,
            e.errorMessage());

        error.setCause(e);
        error.setCodeFileLineNumber(asmLineNumber);

        throw error;
    }

    if (litOrExpr.expression.hasOffset) {
        if (litOrExpr.expression.isMinus) {
            retVal -= litOrExpr.expression.offset;
        } else {
            retVal += litOrExpr.expression.offset;
        }
    }

    // check if value matches with label + offset
    if(litOrExpr.expression.hasValue) {
        if (litOrExpr.expression.value != retVal) {

            CompileError error(
                __FILE__, __LINE__, __func__,
                "Defined expression value (" +
                Conversion::toString(litOrExpr.expression.value) +
                ") does not match with resolved (" +
                Conversion::toString(retVal) +
                ") one.");

            error.setCodeFileLineNumber(asmLineNumber);

            throw error;
        }
    }

    return retVal;
}
