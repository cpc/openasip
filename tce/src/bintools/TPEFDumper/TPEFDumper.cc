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
 * @file TPEFDumper.hh
 *
 * Definition of TPEFDumper class.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-tut.fi)
 * @note rating: red
 */

#include "TPEFDumper.hh"

#include "BinaryReader.hh"
#include "Binary.hh"
#include "Section.hh"
#include "UDataSection.hh"
#include "DataSection.hh"
#include "ASpaceSection.hh"
#include "ASpaceElement.hh"
#include "StringSection.hh"
#include "SectionElement.hh"
#include "SymbolElement.hh"
#include "RelocElement.hh"
#include "RelocSection.hh"
#include "ResourceSection.hh"
#include "ResourceElement.hh"
#include "Chunk.hh"
#include "CodeSection.hh"
#include "InstructionElement.hh"
#include "TPEFTools.hh"
#include "FileSymElement.hh"
#include "SectionSymElement.hh"
#include "CodeSymElement.hh"
#include "ProcedSymElement.hh"
#include "DataSymElement.hh"
#include "TPEFDisassembler.hh"
#include "DisassemblyInstruction.hh"
#include "sstream"
#include "DebugElement.hh"
#include "DebugStabElem.hh"

using TPEF::BinaryReader;
using TPEF::Binary;
using TPEF::Section;
using TPEF::RawSection;
using TPEF::UDataSection;
using TPEF::DataSection;
using TPEF::ASpaceElement;
using TPEF::ASpaceSection;
using TPEF::StringSection;
using TPEF::SectionElement;
using TPEF::SymbolElement;
using TPEF::RelocElement;
using TPEF::RelocSection;
using TPEF::ResourceSection;
using TPEF::ResourceElement;
using TPEF::Chunk;
using TPEF::CodeSection;
using TPEF::InstructionElement;
using TPEF::TPEFTools;
using TPEF::FileSymElement;
using TPEF::SectionSymElement;
using TPEF::CodeSymElement;
using TPEF::DataSymElement;
using TPEF::ProcedSymElement;
using TPEF::DebugElement;
using TPEF::DebugStabElem;


/**
 * Constructor.
 */
TPEFDumper::TPEFDumper(Binary& tpef, std::ostream& out) :
    tpef_(tpef), out_(out), onlyLogicalInfo_(false) {
}

/**
 * Destructor.
 */
TPEFDumper::~TPEFDumper() {
}

/**
 * Organizes sections alphabetically and creates index table.
 *
 * NOTE: method does not work if there is two sections
 * with same header string (very rare).
 */
void
TPEFDumper::createLogicalIndexes() {

    std::map<std::string, Word> organizedSections;

    for (Word i = 0; i < tpef_.sectionCount(); i++) {
        Section *sect = tpef_.section(i);
        organizedSections[sectionHeader(*sect, true)] = i;
    }

    for (std::map<std::string, Word>::iterator
             iter = organizedSections.begin();
         iter != organizedSections.end();
         iter++) {

        actualIndexes_.push_back((*iter).second);
    }
}

/**
 * Converts index to print sections in logical order, if onlyLogical() is set.
 *
 * @param Requested index of section.
 * @return Converted or same index that in parameter.
 */
Word
TPEFDumper::actualIndex(Word origIndex) const {
    if (onlyLogical()) {
        return actualIndexes_[origIndex];
    } else {
        return origIndex;
    }
}

/**
 * Sets if dumper is wanted to print out only logical information of TPEF.
 *
 * If flag is set then no element or section indexes are dumped, only
 * information that is independent to that how TPEF hierarchy is physically
 * constructed is dumped. With this option user can check for example
 * if two different TPEF files actually contains same program, relocations,
 * symbols etc.
 *
 * @param flag If true, then logical printing is set on.
 */
void
TPEFDumper::setOnlyLogical(bool flag) {
    if (flag) {
        createLogicalIndexes();
    }

    onlyLogicalInfo_ = flag;
}

/**
 * Returns true if dumper is set to print only logical information.
 *
 * See. setOnlyLogical for more information.
 *
 * @return True if dumper is set to print only logical information.
 */
bool
TPEFDumper::onlyLogical() const {
    return onlyLogicalInfo_;
}

/**
 * Prints file headers.
 */
void
TPEFDumper::fileHeaders() {

    // generate string for strings() field of binary
    int strSectionIndex = findSectionIndex(*tpef_.strings());
    std::string strIndexString;

    if (strSectionIndex != -1) {
        strIndexString = Conversion::toString(strSectionIndex);
    } else if (tpef_.strings() == NULL) {
        strIndexString = "No strings section defined";
    } else {
        strIndexString = "INVALID TPEF: String section was not found.";
    }

    out_ << std::left
         << std::setw(20) << "Section count: " << tpef_.sectionCount()
         << std::endl
         << std::setw(20) << "String section: " << sectionString(*tpef_.strings(), false)
         << std::endl
         << std::setw(20) << "File architecture: "
         << fileArchString(tpef_.arch()) << std::endl
         << std::setw(20) << "File type: " << fileTypeString(tpef_.type())
         << std::endl << std::endl;
}

/**
 * Prints one lined section header for every section.
 */
void
TPEFDumper::sectionHeaders() {

    out_ << sectionHeader() << std::endl;

    for (Word i = 0; i < tpef_.sectionCount(); i++) {
        Section *sect = tpef_.section(actualIndex(i));

        out_ << sectionHeader(*sect) << std::endl;
    }

    out_ << std::endl
         << "B = Bytes. U = MAUs. E = SectionElements."
         << std::endl;

    out_ << std::endl;
}

/**
 * Prints information about needed data address space sizes for 
 * initialized and uninitialized data.
 */
void
TPEFDumper::memoryInfo() {
    
    out_ << "Minimum sizes for address spaces:" << std::endl << std::endl;

    // map of used address spaces
    std::map<ASpaceElement*, std::pair<int, int> > neededMAUsOfASpace;
    // map telling if CODE and/or DATA memory is used in this aspace
    std::map<ASpaceElement*, std::pair<bool, bool> > typeOfMemNeeded;
    
    for (Word i = 0; i < tpef_.sectionCount(); i++) {
        Section& currSect = *tpef_.section(actualIndex(i));

        if (currSect.isProgramSection()) {
            
            // init new address space limits to 0,0
            if (neededMAUsOfASpace.find(currSect.aSpace()) == 
                neededMAUsOfASpace.end()) {
                
                neededMAUsOfASpace[currSect.aSpace()] = std::pair<int,int>(0,0);
                typeOfMemNeeded[currSect.aSpace()] = std::pair<bool,bool>(false,false);
            }
            
            std::pair<int,int> currentLimits = neededMAUsOfASpace[currSect.aSpace()];
            int currMin = currSect.startingAddress();
            int currMax = currMin;
            std::pair<bool,bool> currentMemtypes = typeOfMemNeeded[currSect.aSpace()];
            
            if (currSect.type() == Section::ST_CODE) {
                
                currMax += dynamic_cast<const CodeSection*>
                    (&currSect)->instructionCount();
                currentMemtypes.first = true;
                                                              
            } else if (currSect.type() == Section::ST_DATA ||
                       currSect.type() == Section::ST_UDATA) {

                currMax += dynamic_cast<const UDataSection*>(
                    &currSect)->lengthInMAUs();
                currentMemtypes.second = true;
                
            } else {
                assert(false && "Unknown program section type");
            }
            
            // update limits to map
            if (currMin < currentLimits.first) {
                currentLimits.first = currMin;
            }

            if (currMax > currentLimits.second) {
                currentLimits.second = currMax;
            }

            neededMAUsOfASpace[currSect.aSpace()] = currentLimits;
            typeOfMemNeeded[currSect.aSpace()] = currentMemtypes;
        }
    }
    
    // print out in ASpace order...
    Section* aSpaces = tpef_.section(Section::ST_ADDRSP, 0);
    
    for (Word i = 0; i < aSpaces->elementCount(); i++) {
        ASpaceElement* aSpace = 
            dynamic_cast<ASpaceElement*>(aSpaces->element(i));
        
        if (neededMAUsOfASpace.find(aSpace) != neededMAUsOfASpace.end()) {
            int aSpaceSize =  
                neededMAUsOfASpace[aSpace].second - 
                neededMAUsOfASpace[aSpace].first;

            out_ <<  i << ":";
            if (typeOfMemNeeded[aSpace].first) out_ << " CODE";
            if (typeOfMemNeeded[aSpace].second) out_ << " DATA";
            out_ << ": " <<  aSpaceSize << " MAU(s)" << std::endl;

        } else {
            out_ << i << ": not used for data nor instructions." 
                 << std::endl;
        }
    }

    out_ << std::endl;
}

/**
 * Prints full information of every relocation table in tpef.
 */
void
TPEFDumper::relocationTables() {

    for (Word i = 0; i < tpef_.sectionCount(); i++) {
        Section& currSect = *tpef_.section(actualIndex(i));

        if (currSect.type() == Section::ST_RELOC) {
            section(currSect);
        }
    }
}

/**
 * Prints full information of every symbol table in tpef.
 */
void
TPEFDumper::symbolTables() {

    for (Word i = 0; i < tpef_.sectionCount(); i++) {
        Section& currSect = *tpef_.section(actualIndex(i));

        if (currSect.type() == Section::ST_SYMTAB) {
            section(currSect);
        }
    }
}

/**
 * Prints full information of one section in tpef.
 *
 * @param sectionIndex Index of requested section.
 */
void
TPEFDumper::section(Word sectionIndex) {
    if (sectionIndex < tpef_.sectionCount()) {
        section(*tpef_.section(actualIndex(sectionIndex)));

    } else {
        out_ << "There is no section with index: " << sectionIndex
             << std::endl << std::endl;
    }
}

/**
 * Returns field names (first line of table) of section header table.
 *
 * @return field names (first line of table) of section header table.
 */
std::string
TPEFDumper::sectionHeader() {
    std::stringstream retVal;

    retVal << std::setw(6) << std::left << "index"
           << std::setw(15)<< std::right << "type"
           << std::setw(12) << "address"
           << std::setw(17) << "address space"
           << std::setw(9)  << "flags"
           << std::setw(12)  << "link"
           << std::setw(15)  << "size(B/U/E)"
           << std::left << "\tname";

    return retVal.str();
}

/**
 * Returns section header string of one section.
 *
 * @param sect Section whose header is printed.
 * @return section header information of one section.
 */
std::string
TPEFDumper::sectionHeader(Section &sect, bool noIndex) {

    std::string nameString;
    if (tpef_.strings() == NULL) {
        nameString = "No section name defined.";
    } else {
        nameString = tpef_.strings()->chunk2String(sect.name());
    }

    std::string elementsOrMAUs;
    if (sect.isChunkable()) {

        if (sect.type() == Section::ST_STRTAB) {
            elementsOrMAUs = Conversion::toString(
                dynamic_cast<UDataSection&>(sect).length()) + " B";
        } else {
            elementsOrMAUs = Conversion::toString(
                dynamic_cast<UDataSection&>(sect).lengthInMAUs()) + " U";
        }

    } else {
        elementsOrMAUs = Conversion::toString(sect.elementCount()) + " E";
    }

    int sectIndex = findSectionIndex(sect);

    std::stringstream retVal;

    retVal << std::left;
    if (!noIndex) {
        retVal << std::setw(6)
               << Conversion::toString(sectIndex) + ":"
               << std::setw(15) << std::right;
    }

    retVal << sectionTypeString(sect.type())
           << std::setw(12) << sect.startingAddress()
           << std::setw(17) << addressSpaceString(*sect.aSpace())
           << std::setw(9)  << Conversion::toHexString((int)sect.flags(),2)
           << std::setw(12) << sectionString(*sect.link(), false)
           << std::setw(15) << elementsOrMAUs
           << std::left << "\t" + nameString;

    return retVal.str();
}

/**
 * Returns section index and/or type information.
 *
 * @param sect Section whose string is requested.
 * @param shortForm If true then only index is returned.
 * @return Type and/or index of section.
 */
std::string
TPEFDumper::sectionString(Section &sect, bool shortForm) {
    std::string str;

    int index = findSectionIndex(sect);

    if (index == -1) {
        str = "FAIL";
    } else {
        if (!shortForm) {
            str = sectionTypeString(sect.type(), true) + ":";
        }
        str += Conversion::toString(index);
    }

    return str;
}

/**
 * Prints all information of requested section.
 *
 * @param sect Section to print.
 */
void
TPEFDumper::section(Section &sect) {

    std::set<std::string> organizedElements;

    switch (sect.type()) {
    case Section::ST_SYMTAB: {
        out_ << "Symbol table:" << std::endl << std::endl;
        out_ << sectionHeader() << std::endl;;
        out_ << sectionHeader(sect) << std::endl;

        out_ << std::endl << std::left;

        if (!onlyLogical()) {
            out_ << std::setw(6)  << "index" << std::right;
        }

        out_ << std::setw(17) << "type" << std::right
             << std::setw(15) << "binding"
             << std::setw(10) << "absolute"
             << std::setw(12) << "owner"
             << std::setw(15) << "value"
             << std::left << "\tname"
             << std::endl << std::endl;

        StringSection* strings = dynamic_cast<StringSection*>(sect.link());

        for (Word i = 0; i < sect.elementCount(); i++) {
            SymbolElement* sym =
                dynamic_cast<SymbolElement*>(sect.element(i));

            std::string isAbso = (sym->absolute()) ? ("yes") : ("no");

            // resolve value
            std::string symValue;
            switch (sym->type()) {
            case SymbolElement::STT_NOTYPE: {
                symValue = "NONE";
            } break;

            case SymbolElement::STT_SECTION: {
                symValue = Conversion::toString(
                    dynamic_cast<SectionSymElement*>(sym)->value());
            } break;

            case SymbolElement::STT_FILE: {
                symValue = Conversion::toString(
                    dynamic_cast<FileSymElement*>(sym)->value());
            } break;

            case SymbolElement::STT_DATA: {
                UDataSection* uDataSect =
                    dynamic_cast<UDataSection*>(sym->section());
                assert(uDataSect != NULL);

                DataSymElement* dataSym =
                    dynamic_cast<DataSymElement*>(sym);
                assert(dataSym != NULL);

                symValue = Conversion::toString(
                    uDataSect->chunkToMAUIndex(dataSym->reference())) +
                    " U";

            } break;

            case SymbolElement::STT_CODE:
            case SymbolElement::STT_PROCEDURE: {
                CodeSection* codeSect =
                    dynamic_cast<CodeSection*>(sym->section());

                assert(codeSect != NULL);

                CodeSymElement* codeSym =
                    dynamic_cast<CodeSymElement*>(sym);
                assert(codeSym != NULL);

                symValue = Conversion::toString(
                    codeSect->indexOfElement(*codeSym->reference())) +
                    " E";
            } break;


            default:
                symValue = "Unknown symbol type!";
            };

            std::stringstream elemStr;
            elemStr << std::left;

            if (!onlyLogical()) {
                elemStr << std::setw(6)
                        << Conversion::toString(i) + ":"
                        << std::right;
            }

            elemStr << std::setw(17) << symbolTypeString(sym->type())
                    << std::right
                    << std::setw(15)
                    << symbolBindString(sym->binding())
                    << std::setw(10) << isAbso
                    << std::setw(12)
                    << sectionString(*sym->section(), false)
                    << std::setw(15) << symValue
                    << std::left
                    << "\t" + strings->chunk2String(sym->name());

            if (onlyLogical()) {
                organizedElements.insert(elemStr.str());
            } else {
                out_ << elemStr.str() << std::endl;
            }

            elemStr.clear();
        }

        dumpStringSet(organizedElements);

        out_ << std::endl
             << "U = Minimum allocateable unit index. "
             << "E = Section element index. "
             << std::endl;
    } break;

    case Section::ST_RELOC: {
        RelocSection& relocSect =
            dynamic_cast<RelocSection&>(sect);

        out_ << "Relocation table: " << std::endl;
        out_ << "Referenced section: "
             << sectionString(*relocSect.referencedSection())
             << std::endl << std::endl;

        out_ << sectionHeader() << std::endl;
        out_ << sectionHeader(sect) << std::endl;

        out_ << std::endl << std::left;

        if (!onlyLogical()) {
            out_ << std::setw(6)  << "index" << std::right;
        }

        out_ << std::setw(17) << "type"
             << std::right
             << std::setw(6) << "size"
             << std::setw(18) << "location"
             << std::setw(18) << "destination"
             << std::setw(17) << "address space"
             << std::left << "\tsymbol"
             << std::endl << std::endl;

        for (Word i = 0; i < sect.elementCount(); i++) {
            RelocElement* reloc =
                dynamic_cast<RelocElement*>(sect.element(i));

            
            std::string locationAddress;

            if (relocSect.referencedSection()->type() == Section::ST_DATA) {
                Chunk* srcChunk = dynamic_cast<Chunk*>(reloc->location());

                DataSection* dataSect =
                    dynamic_cast<DataSection*>(relocSect.referencedSection());

                assert(dataSect != NULL);

                locationAddress = Conversion::toString(
                    dataSect->chunkToMAUIndex(srcChunk)) + " U";

            } else if (relocSect.referencedSection()->type() ==
                       Section::ST_CODE) {

                CodeSection* codeSect =
                    dynamic_cast<CodeSection*>(relocSect.referencedSection());

                assert(codeSect != NULL);

                locationAddress = Conversion::toString(
                    codeSect->indexOfElement(
                        *dynamic_cast<InstructionElement*>(
                            reloc->location()))) + " E";

            } else {
                abortWithError("Section containing relocated elements must"
                               " be DATA or CODE.");
            }
            
            std::string destinationAddress;
            Word dstAddress = 0;
            
            // if relocation is resolved 
            if (reloc->destination() != NULL) {
                
                Section& dstSection =
                    TPEFTools::sectionOfElement(tpef_, *reloc->destination());
                
                if (dstSection.type() == Section::ST_CODE) {
                    CodeSection& codeSect =
                        dynamic_cast<CodeSection&>(dstSection);
                    
                    dstAddress = codeSect.indexOfInstruction(
                        *dynamic_cast<InstructionElement*>(reloc->destination()));
                    
                } else if (dstSection.type() == Section::ST_DATA ||
                           dstSection.type() == Section::ST_UDATA) {
                    
                    UDataSection& uDataSect =
                        dynamic_cast<UDataSection&>(dstSection);
                    
                    assert(uDataSect.belongsToSection(dynamic_cast<Chunk*>(reloc->destination())));
                    
                    dstAddress = uDataSect.chunkToMAUIndex(
                        dynamic_cast<Chunk*>(reloc->destination()));
                    
                } else {
                    abortWithError("Destination of relocation must be DATA, "
                                   "UDATA or CODE section.");
                }

                dstAddress += dstSection.startingAddress();
            }

            std::stringstream elemStr;
            elemStr << std::left;

            if (!onlyLogical()) {
                elemStr << std::setw(6)  << Conversion::toString(i) + ":"
                        << std::right;
            }

            elemStr << std::setw(17) << relocTypeString(reloc->type())
                    << std::right
                    << std::setw(6) << static_cast<Word>(reloc->size())
                    << std::setw(18) << locationAddress;
            
            if (reloc->destination() != NULL) {
                elemStr << std::setw(18)
                        << Conversion::toString(dstAddress) + " A";
            } else {
                elemStr << std::setw(18)
                        << "Unresolved";
            }
            
            elemStr << std::setw(17) << addressSpaceString(*reloc->aSpace())
                    << std::left
                    << "\t" + symbolString(*sect.link(), *reloc->symbol());

            if (onlyLogical()) {
                organizedElements.insert(elemStr.str());
            } else {
                out_ << elemStr.str() << std::endl;
            }
        }

        dumpStringSet(organizedElements);

        out_ << std::endl
             << "E = Section element index. "
             << "U = Minimum allocateable unit Index. "
             << "A = Address of the address space."
             << std::endl << std::endl;

    } break;

    case Section::ST_ADDRSP: {
        out_ << "Address space table:"
             << std::endl << std::endl;

        out_ << sectionHeader() << std::endl;
        out_ << sectionHeader(sect) << std::endl;


        out_ << std::endl << std::left;
        if (!onlyLogical()) {
            out_ << std::setw(6)  << "index" << std::right;
        }

        out_ << std::setw(6) << "MAU"
             << std::right
             << std::setw(10) << "alignment"
             << std::setw(10) << "word size"
             << std::left << "\tname"
             << std::endl << std::endl;

        StringSection* strings = dynamic_cast<StringSection*>(sect.link());

        for (Word i = 0; i < sect.elementCount(); i++) {
            ASpaceElement* aSpace =
                dynamic_cast<ASpaceElement*>(sect.element(i));

            std::stringstream elemStr;

            elemStr << std::left;
            if (!onlyLogical()) {
                elemStr << std::setw(6)  << Conversion::toString(i) + ":"
                        << std::right;
            }

            elemStr << std::setw(6) << static_cast<Word>(aSpace->MAU())
                    << std::right
                    << std::setw(10) << static_cast<Word>(aSpace->align())
                    << std::setw(10) << static_cast<Word>(aSpace->wordSize())
                    << std::left
                    << "\t" + strings->chunk2String(aSpace->name());

            if (onlyLogical()) {
                organizedElements.insert(elemStr.str());
            } else {
                out_ << elemStr.str() << std::endl;
            }
        }

        dumpStringSet(organizedElements);


    } break;

    case Section::ST_MR: {
        out_ << "Machine resource table:"
             << std::endl << std::endl;

        out_ << sectionHeader() << std::endl;
        out_ << sectionHeader(sect) << std::endl;

        if (onlyLogical()) {
            out_ << std::endl << std::left
                 << std::setw(12) << "type"
                 << std::right
                 << std::setw(8) << "info"
                 << std::left << "\tname"
                 << std::endl << std::endl;
        } else {
            out_ << std::endl << std::left
                 << std::setw(6)  << "index"
                 << std::setw(12) << std::right << "type"
                 << std::setw(17) << "id"
                 << std::setw(8) << "info"
                 << std::left << "\tname"
                 << std::endl << std::endl;
        }

        StringSection* strings = dynamic_cast<StringSection*>(sect.link());

        for (Word i = 0; i < sect.elementCount(); i++) {
            ResourceElement* res =
                dynamic_cast<ResourceElement*>(sect.element(i));

            std::stringstream elemStr;

            if (onlyLogical()) {
                elemStr << std::left
                        << std::setw(12)
                        << resourceTypeString(res->type())
                        << std::right
                        << std::setw(8) << res->info()
                        << std::left
                        << "\t" + strings->chunk2String(res->name());

                organizedElements.insert(elemStr.str());

            } else {
                out_ << std::left
                     << std::setw(6)  << Conversion::toString(i) + ":"
                     << std::setw(12) << std::right
                     << resourceTypeString(res->type())
                     << std::setw(17)
                     << resourceIdString(res->id(), res->type())
                     << std::setw(8) << res->info()
                     << std::left
                     << "\t" + strings->chunk2String(res->name())
                     << std::endl;
            }
        }

        dumpStringSet(organizedElements);


    } break;

    case Section::ST_CODE: {
        out_ << "Code section:"
             << std::endl << std::endl;
        out_ << sectionHeader() << std::endl;
        out_ << sectionHeader(sect) << std::endl;

        out_ << std::endl;

        TPEFDisassembler disasm(tpef_);

        for (Word i = 0; i < disasm.instructionCount(); i++) {

            DisassemblyInstruction *instr = disasm.createInstruction(i);

            out_ << std::left << std::setw(10)
                 << Conversion::toString(
                     i + disasm.startAddress()) + ":"
                 << std::left
                 << instr->toString()
                 << std::endl;

            delete instr;
        }

    } break;

    case Section::ST_DATA: {
        out_ << "Data section:"
             << std::endl << std::endl;

        out_ << sectionHeader() << std::endl;
        out_ << sectionHeader(sect) << std::endl;

        DataSection &data = dynamic_cast<DataSection&>(sect);

        for (Word i = 0; i < data.lengthInMAUs(); i++) {

            if (i%16 == 0) {
                out_ << std::endl << std::setw(9) <<
                    Conversion::toString(i + data.startingAddress()) + ":";
            }

            out_ << std::setw(sizeof(MinimumAddressableUnit)*2 + 3)
                 << Conversion::toHexString(data.MAU(i));

        }

        out_ << std::endl;

    } break;

    case Section::ST_STRTAB: {
        out_ << "String section:"
             << std::endl << std::endl;

        out_ << sectionHeader() << std::endl;
        out_ << sectionHeader(sect) << std::endl;

        out_ << std::endl << std::left;

        if (!onlyLogical()) {
            out_ << std::setw(10)  << "offset";
        }

        out_ << std::left << "string "
             << std::endl << std::endl;

        StringSection &strings = dynamic_cast<StringSection&>(sect);

        for (Word i = 0; i < strings.length(); i++) {
            std::stringstream elemStr;

            if (!onlyLogical()) {
                elemStr << std::setw(10) << std::left
                        << Conversion::toString(i) + ":";
            }

            while  (strings.byte(i) != 0) {
                elemStr << strings.byte(i++);
            }

            if (onlyLogical()) {
                organizedElements.insert(elemStr.str());
            } else {
                out_ << elemStr.str() << std::endl;
            }
        }

        dumpStringSet(organizedElements);


    } break;

    case Section::ST_DEBUG: {
        out_ << "Debug section:"
             << std::endl << std::endl;

        out_ << sectionHeader() << std::endl;
        out_ << sectionHeader(sect) << std::endl;

        out_ << std::endl << std::left;

        out_ << std::setw(10)  << "type";
        out_ << std::left << "data "
             << std::endl << std::endl;

        for (Word i = 0; i < sect.elementCount(); i++) {

            std::stringstream elemStr;

            DebugElement* elem = dynamic_cast<DebugElement*>(sect.element(i));

            elemStr << std::setw(10) << std::left
                    << debugElementTypeString(elem->type());

            for (unsigned int j = 0; j < elem->length(); j++) {
                elemStr << std::hex << std::setw(3)
                        << static_cast<int>(elem->byte(j));
            }

            elemStr << "\t"
                    << dynamic_cast<StringSection*>(
                        sect.link())->chunk2String(elem->debugString());

            if (onlyLogical()) {
                organizedElements.insert(elemStr.str());
            } else {
                out_ << elemStr.str() << std::endl;
            }
        }

        dumpStringSet(organizedElements);

    } break;

    default:
        out_ << "Printing section type: " << sectionTypeString(sect.type())
             << " is not implemented." << std::endl;
    }

    out_ << std::endl;
}

/**
 * Returns index of section.
 *
 * If section is not found from tpef returns -1.
 *
 * @param sect Section whose index is returned.
 * @return Index of section or error code.
 */
int
TPEFDumper::findSectionIndex(Section& sect) {

    int index = 0;
    while (static_cast<Word>(index) < tpef_.sectionCount() &&
           tpef_.section(index) != &sect) {

        index++;
    }

    if (static_cast<Word>(index) < tpef_.sectionCount()) {
        for (Word i = 0; i < tpef_.sectionCount(); i++) {
            if (actualIndex(i) == static_cast<Word>(index)) {
                return i;
            }
        }
    }

    return -1;
}

/**
 * Returns index of element.
 *
 * If element is not found from section returns -1.
 *
 * @param sect Section which contains the element.
 * @param elem Element whose index is returned.
 * @return Index of element or error code.
 */
int
TPEFDumper::findElementIndex(Section& sect, SectionElement& elem) {
    int index = 0;

    while (static_cast<Word>(index) < sect.elementCount() &&
           sect.element(index) != &elem) {
        index++;
    }

    if (static_cast<Word>(index) < sect.elementCount()) {
        return index;
    } else {
        return -1;
    }
}

/**
 * Returns section type in human readable form.
 *
 * @param type Type id to convert.
 * @param shortForm If short form (only enum string) is returned.
 * @return Section type id string.
 */
std::string
TPEFDumper::sectionTypeString(
    Section::SectionType type, bool shortForm) {

    std::string typeStr;

    switch (type) {
    case Section::ST_NULL:   typeStr = "NULL";   break;
    case Section::ST_STRTAB: typeStr = "STRTAB"; break;
    case Section::ST_SYMTAB: typeStr = "SYMTAB"; break;
    case Section::ST_DEBUG:  typeStr = "DEBUG";  break;
    case Section::ST_RELOC:  typeStr = "RELOC";  break;
    case Section::ST_LINENO: typeStr = "LINENO"; break;
    case Section::ST_NOTE:   typeStr = "NOTE";   break;
    case Section::ST_ADDRSP: typeStr = "ADDRSP"; break;
    case Section::ST_MR:     typeStr = "MR";     break;
    case Section::ST_CODE:   typeStr = "CODE";   break;
    case Section::ST_DATA:   typeStr = "DATA";   break;
    case Section::ST_UDATA:  typeStr = "UDATA";  break;
    case Section::ST_DUMMY:  typeStr = "DUMMY";  break;
    default: typeStr = "UNKNOWN";
    }

    if (!shortForm) {
        typeStr += " (" + Conversion::toHexString(type, 2) + ")";
    }

    return typeStr;
}

/**
 * Returns file architecture type in human readable form.
 *
 * @param arch Architecture id to convert.
 * @return Architecture id string.
 */
std::string
TPEFDumper::fileArchString(Binary::FileArchitecture arch) {

    std::string str;

    switch (arch) {
    case Binary::FA_NOARCH:  str = "NOARCH";  break;
    case Binary::FA_TTA_MOVE:str = "TTA_MOVE";break;
    case Binary::FA_TTA_TUT: str = "TTA_TUT"; break;
    case Binary::FA_TDS_TI:  str = "TDS_TI";  break;
    default: str = "UNKNOWN";
    }

    return str + " (" + Conversion::toHexString(arch, 2) + ")";
}

/**
 * Returns debug element type string in human readable form.
 *
 * @param type Type id to convert.convert.
 * @return Type in string form.
 */
std::string
TPEFDumper::debugElementTypeString(DebugElement::ElementType type) {

    std::string str;

    switch (type) {
    case DebugElement::DE_STAB:  str = "STAB";  break;
    default: str = "UNKNOWN";
    }

    return str;
}

/**
 * Returns file type in human readable form.
 *
 * @param type File type id to convert.
 * @return File type id string.
 */
std::string
TPEFDumper::fileTypeString(Binary::FileType type) {

    std::string str;

    switch (type) {
    case Binary::FT_NULL:    str = "UNDEF";   break;
    case Binary::FT_OBJSEQ:  str = "OBJSEQ";  break;
    case Binary::FT_PURESEQ: str = "PURESEQ"; break;
    case Binary::FT_LIBSEQ:  str = "LIBSEQ";  break;
    case Binary::FT_MIXED:   str = "MIXED";   break;
    case Binary::FT_PARALLEL:str = "PARALLEL";break;
    default: str = "UNKNOWN";
    }

    return str + " (" + Conversion::toHexString(type, 2) + ")";
}

/**
 * Returns symbol type in human readable form.
 *
 * @param type Symbol type id to convert.
 * @return Symbol type id string.
 */
std::string
TPEFDumper::symbolTypeString(SymbolElement::SymbolType type) {

    std::string str;

    switch (type) {
    case SymbolElement::STT_NOTYPE:    str = "NOTYPE";    break;
    case SymbolElement::STT_CODE:      str = "CODE";      break;
    case SymbolElement::STT_DATA:      str = "DATA";      break;
    case SymbolElement::STT_FILE:      str = "FILE";      break;
    case SymbolElement::STT_SECTION:   str = "SECTION";   break;
    case SymbolElement::STT_PROCEDURE: str = "PROCEDURE"; break;
    default: str = "UNKNOWN";
    }

    return str + " (" + Conversion::toHexString(type, 2) + ")";
}

/**
 * Returns symbol binding type in human readable form.
 *
 * @param bind Symbol binding type id to convert.
 * @return Symbol binding id string.
 */
std::string
TPEFDumper::symbolBindString(SymbolElement::SymbolBinding bind) {

    std::string str;

    switch (bind) {
    case SymbolElement::STB_LOCAL:  str = "LOCAL";  break;
    case SymbolElement::STB_GLOBAL: str = "GLOBAL"; break;
    case SymbolElement::STB_WEAK:   str = "WEAK";   break;
    default: str = "UNKNOWN";
    }

    return str + " (" + Conversion::toHexString(bind, 2) + ")";
}

/**
 * Returns srelocation type in human readable form.
 *
 * @param type Relocation type id to convert.
 * @return Relocation type id string.
 */
std::string
TPEFDumper::relocTypeString(RelocElement::RelocType type) {

    std::string str;

    switch (type) {
    case RelocElement::RT_NOREL: str = "NOREL";  break;
    case RelocElement::RT_SELF:  str = "SELF";  break;
    case RelocElement::RT_PAGE:  str = "PAGE";  break;
    case RelocElement::RT_PCREL: str = "PCREL";  break;
    default: str = "UNKNOWN";
    }

    return str + " (" + Conversion::toHexString(type, 2) + ")";
}

/**
 * Returns resource type in human readable form.
 *
 * @param type Resource type id to convert.
 * @return Resource type id string.
 */
std::string
TPEFDumper::resourceTypeString(ResourceElement::ResourceType type) {
    std::string str;
    switch (type) {
    case ResourceElement::MRT_NULL:str = "NULL";break;
    case ResourceElement::MRT_BUS: str = "BUS"; break;
    case ResourceElement::MRT_UNIT:str = "UNIT";break;
    case ResourceElement::MRT_RF:  str = "RF";  break;
    case ResourceElement::MRT_OP:  str = "OP";  break;
    case ResourceElement::MRT_IMM: str = "IMM"; break;
    case ResourceElement::MRT_SR:  str = "SR";  break;
    case ResourceElement::MRT_PORT:str = "PORT";break;
    default: str = "UNKNOWN";
    }
    return str + " (" + Conversion::toHexString(type, 2) + ")";
}

/**
 * Returns resource id in human readable form.
 *
 * @param id Resource id to convert.
 * @param type Resource type of id. (needed for conversion)
 * @return Resource id string.
 */
std::string
TPEFDumper::resourceIdString(HalfWord id, ResourceElement::ResourceType type) {
    std::string str;

    switch (type) {

    case ResourceElement::MRT_UNIT:
    case ResourceElement::MRT_BUS: {
        if (id == 0) {
            str = "UNIVERSAL";
        }
    } break;

    case ResourceElement::MRT_RF: {
        if (id == 0) {
            str = "ILLEGAL";
        } else if (id == ResourceElement::INT_RF) {
            str = "UNIV_INT";
        } else if (id == ResourceElement::BOOL_RF) {
            str = "UNIV_BOOL";
        } else if (id == ResourceElement::FP_RF) {
            str = "UNIV_FP";
        } else if ((id&ResourceElement::UNIVERSAL_RF_MASK) != 0) {
            str = "UNIV_UNKNWN";
        }
    } break;

    case ResourceElement::MRT_IMM: {
        if (id == ResourceElement::INLINE_IMM) {
            str = "INLINE_IMM";
        }

    } break;

    default:
        return Conversion::toHexString(id, 4);
    }

    if (str != "") {
        return str + " (" + Conversion::toHexString(id, 2) + ")";
    } else {
        return Conversion::toHexString(id, 4);
    }
}

/**
 * Returns address space element in human readable form.
 *
 * @param aSpace Address space to convert.
 * @return Address space string.
 */
std::string
TPEFDumper::addressSpaceString(ASpaceElement& aSpace) {

    ASpaceSection* aSpaces = dynamic_cast<ASpaceSection*>(
        tpef_.section(Section::ST_ADDRSP,0));

    assert(aSpaces != NULL);

    StringSection* strings = dynamic_cast<StringSection*>(aSpaces->link());
    assert(strings != NULL);

    std::string str;
    if (aSpaces->isUndefined(&aSpace)) {
        str = "undefined";
    } else {
        // find index
        int aSpaceIndex = findElementIndex(*aSpaces, aSpace);

        assert(aSpaceIndex != -1);

        if (!onlyLogical()) {
            str = Conversion::toString(aSpaceIndex) + ":";
        }

        str +=
            Conversion::toString(static_cast<Word>(aSpace.MAU())) + "bit:" +
            Conversion::toString(static_cast<Word>(aSpace.align())) + ":" +
            Conversion::toString(static_cast<Word>(aSpace.wordSize()));
    }

    return str;
}

/**
 * Returns symbol in human readable form.
 *
 * @param sect Section containing requested symbol.
 * @param sym Symbol to convert.
 * @return Symbol string.
 */
std::string
TPEFDumper::symbolString(Section& sect, SymbolElement& sym) {

    std::string str;
    int index = findElementIndex(sect, sym);

    StringSection* strings = dynamic_cast<StringSection*>(sect.link());
    assert(strings != NULL);

    if (!onlyLogical()) {
        str = Conversion::toString(index) + ":";
    }

    str += strings->chunk2String(sym.name());

    return str;
}

/**
 * Dumps a set of strings to output stream.
 *
 * Clears dumped set.
 *
 * @param set Set of strings to print.
 */
void
TPEFDumper::dumpStringSet(std::set<std::string> &aSet) const {
    typedef std::set<std::string>::iterator StrSetIter;
    for (StrSetIter iter = aSet.begin(); iter != aSet.end(); iter++) {
        out_ << *iter << std::endl;
    }
    aSet.clear();
}
