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
 * @file LabelManager.hh
 *
 * Declarations of LabelManager class.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
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
};

#endif
