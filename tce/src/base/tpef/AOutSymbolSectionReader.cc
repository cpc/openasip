/**
 * @file AOutSymbolSectionReader.cc
 *
 * Definition of class AOutSymbolSectionReader.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 18.12.2003 (tmlepist@cs.tut.fi)
 * @note reviewed 23 October 2003 by pj, am, ll, jn
 *
 * @note rating: yellow
 */

#include "AOutSymbolSectionReader.hh"
#include "AOutReader.hh"
#include "ReferenceKey.hh"
#include "SafePointer.hh"
#include "SectionReader.hh"
#include "NoTypeSymElement.hh"
#include "DataSymElement.hh"
#include "CodeSymElement.hh"
#include "ProcedSymElement.hh"
#include "SectionSymElement.hh"
#include "FileSymElement.hh"
#include "ResourceSection.hh"
#include "ResourceElement.hh"
#include "StringSection.hh"
#include "DebugStabElem.hh"
#include "CodeSection.hh"
#include "InstructionElement.hh"
#include "MoveElement.hh"


namespace TPEF {

using ReferenceManager::SectionKey;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::SectionIndexKey;
using ReferenceManager::SafePointer;

AOutSymbolSectionReader AOutSymbolSectionReader::proto_;
const Word AOutSymbolSectionReader::DATA_SYMBOL_SIZE = 4;

const std::string 
AOutSymbolSectionReader::GCC_MODULE_START_SYMBOL1 = "gcc2_compiled.";

const std::string 
AOutSymbolSectionReader::GCC_MODULE_START_SYMBOL2 = "___gnu_compiled_c";

/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
AOutSymbolSectionReader::AOutSymbolSectionReader() : AOutSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
AOutSymbolSectionReader::~AOutSymbolSectionReader() {
}

/**
 * Returns the type of section that this reader reads.
 *
 * @return The type of section this reader can read.
 */
Section::SectionType
AOutSymbolSectionReader::type() const {
    return Section::ST_SYMTAB;
}

/**
 * Reads a.out symbol section out of stream and stores it in section.
 *
 * @param stream The stream to be read from.
 * @param section The section in which data is stored.
 * @exception UnreachableStream If reading of section fails.
 * @exception KeyAlreadyExists Key was in use when trying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some of read values were out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue If there was unexpected value when reading.
 */
void
AOutSymbolSectionReader::readData(
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {
    
    StringSection* stringSection =
        dynamic_cast<AOutReader*>(parent())->stringSection();

    FileOffset offset = stream.readPosition();
    AOutReader* reader = dynamic_cast<AOutReader*>(parent());
    assert(reader != NULL);

    // create undef symbol to start of section (needed by TPEF)
    SymbolElement *undefSymbol =  new NoTypeSymElement();

    SectionOffsetKey nullStringKey =
        SectionOffsetKey(AOutReader::ST_STRING, 0);

    undefSymbol->setName(CREATE_SAFEPOINTER(nullStringKey));
    undefSymbol->setSection(reader->nullSection());

    SectionIndexKey undefSymbolKey =
        SectionIndexKey(AOutReader::ST_SYMBOL, 0);
    SafePointer::addObjectReference(undefSymbolKey, undefSymbol);

    section->addElement(undefSymbol);

    // in this point there is no recources added yet
    resolvedResources_.clear();
    addedResources_.clear();

    // symbols start in a.out with index 0
    int symIndex = 0;
    while (stream.readPosition() <
           offset + reader->header().sectionSizeSymbol()) {

        SectionOffset sectionOffset = stream.readPosition() - offset;

        SymbolElement* elem =
            initializeSymbol(stream, sectionOffset, reader);

        if (elem != NULL) {
            SectionIndexKey symIndexKey =
                SectionIndexKey(AOutReader::ST_SYMBOL, symIndex + 1);
            SafePointer::addObjectReference(symIndexKey, elem);

            section->addElement(elem);

        }

        // always remember to increment index counter, even if symbol is not
        // created
        symIndex++;
    }


    // ----------------------------------------------------------------------
    // write resource table out of N_PRTAB stabs collected from symbol table
    // ----------------------------------------------------------------------

    ResourceSection* resourceTable =
        dynamic_cast<AOutReader*>(parent())->resourceTable();

    ResourceElement *resource = NULL;

    while (!resolvedResources_.empty()) {
        Word value = (*(resolvedResources_.begin())).second;
        Chunk* name = 
            stringSection->string2Chunk(
                (*(resolvedResources_.begin())).first);
        
        // if operation register, make entry to resource table
        if (value > AOutReader::FIRST_FU_REGISTER) {

            resource = new ResourceElement();
            resource->setId(value);
            resource->setType(ResourceElement::MRT_OP);

            resource->setName(
                CREATE_SAFEPOINTER(
                    SectionOffsetKey(AOutReader::ST_STRING, name->offset())));

            resourceTable->addElement(resource);

            // the very first of FU registers in a.out is 
            // "return-address" special register
        } else if (value == AOutReader::FIRST_FU_REGISTER) {

            resource = new ResourceElement();
            resource->setId(value);
            resource->setType(ResourceElement::MRT_SR);

            resource->setName(
                CREATE_SAFEPOINTER(
                    SectionOffsetKey(AOutReader::ST_STRING, name->offset())));

            resourceTable->addElement(resource);
        }

        resolvedResources_.erase(resolvedResources_.begin());
    }

    // add int register file
    resource = new ResourceElement();
    resource->setId(ResourceElement::INT_RF);
    resource->setType(ResourceElement::MRT_RF);
    resource->setName(CREATE_SAFEPOINTER(nullStringKey));
    resourceTable->addElement(resource);

    // add fp registerfile
    resource = new ResourceElement();
    resource->setId(ResourceElement::FP_RF);
    resource->setType(ResourceElement::MRT_RF);
    resource->setName(CREATE_SAFEPOINTER(nullStringKey));
    resourceTable->addElement(resource);

    // add bool register file
    resource = new ResourceElement();
    resource->setType(ResourceElement::MRT_RF);
    resource->setId(ResourceElement::BOOL_RF);
    resource->setName(CREATE_SAFEPOINTER(nullStringKey));
    resourceTable->addElement(resource);

    // add unit
    resource = new ResourceElement();
    resource->setType(ResourceElement::MRT_UNIT);
    resource->setId(ResourceElement::UNIVERSAL_FU);
    resource->setName(CREATE_SAFEPOINTER(nullStringKey));
    resourceTable->addElement(resource);

    // add bus
    resource = new ResourceElement();
    resource->setType(ResourceElement::MRT_BUS);
    resource->setId(ResourceElement::UNIVERSAL_BUS);
    resource->setName(CREATE_SAFEPOINTER(nullStringKey));
    resourceTable->addElement(resource);
}

/**
 * Creates and initilizes  one symbol element.
 *
 * Now only non-stabb symbols are read.
 *
 * @param stream The stream to be read from.
 * @param sectionOffset The offset of the element in section.
 * @param reader The base reader for a.out.
 * @return Newly created symbol.
 * @exception KeyAlreadyExists If key used to register object is in use.
 * @exception UnreachableStream If there occurs problems with stream.
 * @exception OutOfRange If offset read is out of the section.
 */
SymbolElement*
AOutSymbolSectionReader::initializeSymbol(
    BinaryStream& stream,
    SectionOffset sectionOffset,
    AOutReader* reader) const
    throw (KeyAlreadyExists, UnreachableStream,
           OutOfRange) {

    StringSection* stringSection =
        dynamic_cast<AOutReader*>(parent())->stringSection();

    SymbolElement *element = NULL;

    // first there is an offset to string in string table
    Word strtabOff = stream.readWord();

    // type of symbol
    Byte type = stream.readByte();

    // not used
    Byte other = stream.readByte();

    // description of the symbol, reserved for debugger use
    // not needed in tpef (there is debug section for that)
    HalfWord desc = stream.readHalfWord();

    // offset of the value of the symbol
    Word value = stream.readWord();

    // key to string of symbol
    SectionOffsetKey strOffKey =
            SectionOffsetKey(AOutReader::ST_STRING, strtabOff);


    // create element of right type
    switch (type&(~AOutReader::N_EXT)) {

    case AOutReader::N_UNDF: {
        element = new NoTypeSymElement();
        element->setSection(reader->nullSection());
        break;
    }

    case AOutReader::N_TEXT: {
        // create code symbols only if there really is code section
        if (reader->header().sectionSizeText() != 0) {
            
            // if new compilation module start symbol, start new module for
            // resource id resolving...
            std::string symbolName = 
                stringSection->chunk2String(stringSection->chunk(strtabOff));

            if (GCC_MODULE_START_SYMBOL1 == symbolName) {
                Word tpefAddress = value/8;
                addedResources_.push_back(CompilationModule(tpefAddress));
            }

            element = new CodeSymElement();
            
            // convert a.out address to suitable for section offset key..
            value = reader->sectionOffsetOfAddress(value);

            SectionOffsetKey valueKey(AOutReader::ST_TEXT, value);
            CodeSymElement *elem = dynamic_cast<CodeSymElement*>(element);
            elem->setReference(CREATE_SAFEPOINTER(valueKey));

            SectionKey sKey(AOutReader::ST_TEXT);
            element->setSection(CREATE_SAFEPOINTER(sKey));
            
        } else { // otherwise create no type symbol
            element = new NoTypeSymElement();
            element->setSection(reader->nullSection());
        }

        break;
    }

    case AOutReader::N_DATA:
    case AOutReader::N_BSS: {
        element = new DataSymElement();

        value = reader->sectionOffsetOfAddress(value);

        SectionOffsetKey valueKey(type&(~AOutReader::N_EXT), value);
        DataSymElement *elem = dynamic_cast<DataSymElement*>(element);
        elem->setReference(CREATE_SAFEPOINTER(valueKey));


        elem->setSize(DATA_SYMBOL_SIZE);

        SectionKey sKey(type&(~AOutReader::N_EXT));
        element->setSection(CREATE_SAFEPOINTER(sKey));
        break;
    }

    case AOutReader::N_FN: {
        element = new FileSymElement();
        element->setSection(reader->nullSection());
        break;
    }

    case AOutReader::N_PRTAB: {
        std::string symbolName = 
            stringSection->chunk2String(stringSection->chunk(strtabOff));

        if (!MapTools::containsKey(resolvedResources_, symbolName)) {
            resolvedResources_[symbolName] = value;
        } else {
            // if symbol with same name has different value 
            // add replacement entry
            Word resolvedValue = resolvedResources_[symbolName];
            if (value != resolvedValue) {
                addedResources_[addedResources_.size()-1].resources_[value] = 
                    resolvedValue;
            }
        }
        
        return NULL;
    }

    case AOutReader::N_ANN: {
        
        std::string symbolName = 
            stringSection->chunk2String(stringSection->chunk(strtabOff));
        
        // value - address of instruction, symbolName - all the data
        Word numberOfInstruction = value / 8; 
        annotationes_.push_back(std::pair<Word, std::string>(numberOfInstruction, symbolName));
        return NULL;
    }
        
    // rest of symbols are stored as stabs
    default: {
        DebugStabElem* stab =
            new DebugStabElem(type, other, desc, value);

        stab->setDebugString(CREATE_SAFEPOINTER(strOffKey));

        reader->debugSection()->addElement(stab);

        return NULL;
    } break;

    }

    // inform ReferenceManager about new symbol
    SectionOffsetKey offKey =
        SectionOffsetKey(AOutReader::ST_SYMBOL, sectionOffset);
    SafePointer::addObjectReference(offKey, element);
    
    // figure out symbol linkage scope (local/global)
    if (static_cast<Byte>(type) & static_cast<Byte>(AOutReader::N_EXT)) {
        element->setBinding(SymbolElement::STB_GLOBAL);
    } else {
        element->setBinding(SymbolElement::STB_LOCAL);
    }

    // symbol name string
    element->setName(CREATE_SAFEPOINTER(strOffKey));

    return element;
}

/**
 * Finalizer method for AOut symbol sections.
 *
 * Removes reserved gcc code symbols and converts compilation unit code label
 * to file symbol. 
 *
 * Fixes resource references of special operations to use same id in all
 * compilation units.
 *
 * Adds annotationes to needed moves.
 *
 * @param section Section to finalize.
 */
void
AOutSymbolSectionReader::finalize(Section* section) const {
    
    StringSection *strTab = dynamic_cast<StringSection*>(section->link());

    NullSection* nullSection =
        dynamic_cast<AOutReader*>(parent())->nullSection();

    assert(nullSection);

    // make file symbols to tpef
    for (Word i = 0; i < section->elementCount(); i++) {

        SymbolElement *sym =
            dynamic_cast<SymbolElement*>(section->element(i));

        if (sym->type() == SymbolElement::STT_CODE) {

            std::string symName = strTab->chunk2String(sym->name());

            // if read a.out is linked and compilation unit's start 
            // symbol is found (i == 1 and GCC_MODULE_START_SYMBOL1 is 
            // found it means that first read symbol from a.out was 
            // GCC_MODULE_START_SYMBOL1. In linked a.out there is always 
            // symbol for each compilation module before 
            // GCC_MODULE_START_SYMBOL1)


            if (i > 1 && symName == GCC_MODULE_START_SYMBOL1) {

                SymbolElement *prevSymbol =
                    dynamic_cast<SymbolElement*>(section->element(i - 1));

                // previous symbol can be either code symbol or no type, 
                // if there is no instructions in compilation modulex
                assert(prevSymbol->type() == SymbolElement::STT_CODE ||
                       prevSymbol->type() == SymbolElement::STT_NOTYPE);

                FileSymElement *newFileSym = new FileSymElement();
                newFileSym->setName(prevSymbol->name());

                newFileSym->setSection(nullSection);

                CodeSymElement *codeSym = 
                    dynamic_cast<CodeSymElement*>(prevSymbol);

                if (codeSym != NULL) {
                    // TODO: set value and section to be same that in code 
                    //    section newFileSym->setSection(codeSym->section());
                    //	  newFileSym->setValue();
                }

                section->setElement(i - 1, newFileSym);

                // replace all references to prevSymbol with 
                // newFileSym reference.
                SafePointer::replaceAllReferences(newFileSym, prevSymbol);
                delete prevSymbol;
                prevSymbol = NULL;
            }

            if (symName == GCC_MODULE_START_SYMBOL1 ||
                symName == GCC_MODULE_START_SYMBOL2) {

                NoTypeSymElement *replacingSymbol = new NoTypeSymElement();
                replacingSymbol->setName(sym->name());
                replacingSymbol->setSection(nullSection);

                section->setElement(i, replacingSymbol);
                
                // replace all references to sym with replacing symbol 
                // reference
                SafePointer::replaceAllReferences(replacingSymbol, sym);
                delete sym;
                sym = NULL;

            } else {
                // create corresponding STT_PROCEDURE symbol for each code 
                // symbol
                ProcedSymElement *newSym = new ProcedSymElement();
                CodeSymElement *codeSym = dynamic_cast<CodeSymElement*>(sym);
                assert(codeSym != NULL);

                newSym->setAbsolute(codeSym->absolute());
                newSym->setBinding(SymbolElement::STB_LOCAL);
                newSym->setName(codeSym->name());
                newSym->setSection(codeSym->section());
                newSym->setReference(codeSym->reference());
                newSym->setSize(codeSym->size());

                section->addElement(newSym);
            }
        }        
    }
 
    // fix operand references from code section with information gathered from
    // symbol table.
    
    CodeSection* textSection = 
        dynamic_cast<AOutReader*>(parent())->textSection();
       
    for (unsigned int i = 0; i < addedResources_.size();  i++) {
        CompilationModule& module = addedResources_[i];        
        
        // resolve address slice to convert...
        Word lastAddress = textSection->instructionCount();                

        if (i < addedResources_.size() - 1) {
            lastAddress = addedResources_[i+1].startAddress_;
        }

        // fix the stuff... 
        for (unsigned int j = module.startAddress_; j < lastAddress; j++) {
            
            // get move to check
            InstructionElement* instr = &textSection->instruction(j);
            
            MoveElement* move = NULL;
            
            // check only move's
            if (instr->isImmediate()) {
                Word sectionIndex = textSection->instructionToSectionIndex(j);
                instr = dynamic_cast<InstructionElement*>(
                    textSection->element(sectionIndex + 1));
            }
            
            move = dynamic_cast<MoveElement*>(instr);
            
            // if source is universal FU port, replace if updating is needed
            if (move->sourceType() == MoveElement::MF_UNIT &&
                move->sourceUnit() == ResourceElement::UNIVERSAL_FU &&
                MapTools::containsKey(
                    module.resources_, move->sourceIndex())) {
                
                move->setSourceIndex(module.resources_[move->sourceIndex()]);
            }

            // if destination is universal FU port, 
            // replace if updating is needed
            if (move->destinationType() == MoveElement::MF_UNIT &&
                move->destinationUnit() == ResourceElement::UNIVERSAL_FU &&
                MapTools::containsKey(
                    module.resources_, move->destinationIndex())) {
                
                move->setDestinationIndex(
                    module.resources_[move->destinationIndex()]);
            }
        }

    }

    // Add annotations to instruction elements
    for (Word i = 0; i < annotationes_.size(); i++) {

        InstructionElement* instr = dynamic_cast<InstructionElement*>(
            textSection->element(annotationes_[i].first));
        
        std::string annString = annotationes_[i].second;
        
        // first 10 letters are annotation ID
        if (annString.at(10) != ':') {
            throw IllegalProgram(
                __FILE__, __LINE__, __func__, 
                "11 first characters of annotation stab must contain "
                "annotation ID ending with ':' e.g. '0x00011000:'");
        }            
        
        Word annId = Conversion::toInt(annString.substr(0,10));
        
        std::string payString = annString.substr(11,annString.length() - 10);
        std::vector<Byte> payload;
        
        for (unsigned int j = 0; j < payString.length(); j++) {
            payload.push_back(payString.at(j));
        }        

        instr->addAnnotation(new InstructionAnnotation(annId, payload));
    }
}

} // namespace TPEF
