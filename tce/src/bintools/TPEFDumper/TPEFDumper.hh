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
 * @file TPEFDumper.cc
 *
 * Class for outputting TPEF sections in textual format.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-tut.fi)
 * @rating red
 */

#include <ostream>

#include "Binary.hh"
#include "SymbolElement.hh"
#include "RelocElement.hh"
#include "ResourceElement.hh"
#include "DebugElement.hh"

namespace TPEF {
    class SectionElement;
    class Section;
    class SectionElement;
    class ASpaceElement;
}

/**
 * Outputs TPEF hierarchy in textual form.
 */
class TPEFDumper {

public:
    TPEFDumper(TPEF::Binary& tpef, std::ostream& out);
    virtual ~TPEFDumper();

    void setOnlyLogical(bool flag);
    bool onlyLogical() const;

    void fileHeaders();
    void sectionHeaders();
    void memoryInfo();
    void relocationTables();
    void symbolTables();
    void section(Word sectionIndex);

private:
    void createLogicalIndexes();
    Word actualIndex(Word logicalIndex) const;

    std::string sectionHeader();
    std::string sectionHeader(TPEF::Section &sect, bool noIndex = false);
    void section(TPEF::Section &sect);

    int findSectionIndex(TPEF::Section& sect);
    int findElementIndex(TPEF::Section& sect, TPEF::SectionElement& elem);

    std::string sectionString(TPEF::Section &sect, bool shortForm = false);
    std::string addressSpaceString(TPEF::ASpaceElement& aSpace);
    std::string symbolString(TPEF::Section& sect, TPEF::SymbolElement& sym);

    std::string sectionTypeString(
	TPEF::Section::SectionType type, bool shortForm = false);

    std::string fileArchString(TPEF::Binary::FileArchitecture arch);

    std::string debugElementTypeString(TPEF::DebugElement::ElementType type);

    std::string fileTypeString(TPEF::Binary::FileType type);

    std::string symbolTypeString(TPEF::SymbolElement::SymbolType type);

    std::string symbolBindString(TPEF::SymbolElement::SymbolBinding bind);

    std::string relocTypeString(TPEF::RelocElement::RelocType type);

    std::string resourceTypeString(TPEF::ResourceElement::ResourceType type);

    std::string resourceIdString(
        HalfWord id, TPEF::ResourceElement::ResourceType type);

    void dumpStringSet(std::set<std::string> &aSet) const;

    std::vector<Word> actualIndexes_;

    TPEF::Binary &tpef_;
    std::ostream &out_;

    bool onlyLogicalInfo_;
};
