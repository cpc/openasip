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
 * @file LabelManager.hh
 *
 * Declarations of LabelManager class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen@tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TCEASM_LABEL_MANAGER_HH
#define TCEASM_LABEL_MANAGER_HH

#include <set>

#include "ParserStructs.hh"
#include "Exception.hh"

namespace TPEF {
    class Binary;
    class RelocSection;
    class SymbolSection;
    class CodeSection;
    class StringSection;
    class SymbolElement;
    class SectionElement;
    class ASpaceElement;
    class Section;
}

class MachineResourceManager;
class Assembler;

/**
 * Keeps track of labels, symbols and relocations.
 *
 * After all label and relocation information if added, class
 * knows how to write SymbolSection and RelocationSectios to TPEF.
 */
class LabelManager {
public:
    LabelManager(
        TPEF::Binary& bin, MachineResourceManager& resources, 
        Assembler* parent);

    ~LabelManager();

    UValue value(std::string& name) 
        throw (SymbolNotFound);

    TPEF::ASpaceElement& aSpaceElement(std::string& labelName) 
        throw (SymbolNotFound);

    std::string aSpaceName(std::string& labelName)
        throw (SymbolNotFound);

    void addLabel(
        TPEF::ASpaceElement& aSpace,
        std::string& name, UValue value) 
        throw (ObjectAlreadyExists);

    void removeLabel(std::string& name)
        throw (SymbolNotFound);

    void addProcedure(
        std::string& name, UValue value) 
        throw (ObjectAlreadyExists);

    void setGlobal(std::string& labelName);

    void addRelocation(
        TPEF::Section& locationSect,
        TPEF::SectionElement& location,
        TPEF::ASpaceElement& dstASpace,
        UValue destination,
        UValue bitWidth);

    void clearLastRelocations();

    void commitLastRelocations();

    void finalize()
        throw (CompileError);

    void cleanup();

    UValue resolveExpressionValue(
        UValue asmLineNumber, LiteralOrExpression& litOrExpr)
        throw (CompileError);

private:
    /**
     * Internal representation of label.
     */
    struct InternalLabel {
        /// Address space of the label.        
        TPEF::ASpaceElement* aSpace;
        /// Address where label is located.
        UValue value;
        /// Name of the label.
        std::string name;
    };

    /**
     * Internal representation of relocation.
     */
    struct InternalRelocation {
        /// Section where relocated immediate or chunk is stored.
        TPEF::Section* locationSect;
        /// Element that is reloacted.
        TPEF::SectionElement* location;
        /// Number of bits that are used to store the value. 
        UValue bitWidth;
        /// Address space of destination address.        
        TPEF::ASpaceElement* dstASpace;
        /// Destination address.
        UValue destination;
    };

    TPEF::Section* findSectionByAddress(InternalLabel *currLabel);

    TPEF::SymbolElement* createSymbolWithReference(
        InternalLabel *currLabel, TPEF::Section* ownerSection);

    void writeProcedureSymbols(
        TPEF::CodeSection* codeSect,
        TPEF::SymbolSection* symbolSect,
        TPEF::StringSection* strings) throw (CompileError);

    TPEF::RelocSection* findOrCreateRelocationSection(
        InternalRelocation* currReloc, TPEF::SymbolSection* symbolSect);

    TPEF::Section* findRelocationDstSection(InternalRelocation* currReloc);


    typedef std::map<std::string, InternalLabel*> SymbolMap;

    /// Contains stored symbols by their name.
    SymbolMap labels_;

    /// Added relocations.
    std::set<InternalRelocation*> relocs_;

    /// Uncommitted relocations.
    std::set<InternalRelocation*> uncommittedRelocs_;

    /// Names of global symbols.
    std::set<std::string> globals_;

    /// Names and addresses of procedure starts.
    std::map<std::string, UValue> procedures_;

    /// For finding sections containing relocated elements.
    TPEF::Binary& bin_;

    /// Common resources of TPEF.
    MachineResourceManager& resources_;

    /// Parent assembler for adding warnings.
    Assembler* parent_;
};

#endif
