/**
 * @file TPEFDumper.cc
 *
 * Class for outputting TPEF sections in textual format.
 *
 * @author Mikael Lepistö 2005 (tmlepist@tut.fi)
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
