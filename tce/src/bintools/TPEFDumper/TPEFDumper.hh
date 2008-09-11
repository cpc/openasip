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
