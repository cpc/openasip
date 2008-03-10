/**
 * @file CodeSection.hh
 *
 * Declaration of CodeSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_CODE_SECTION_HH
#define TTA_CODE_SECTION_HH

#include "Section.hh"

namespace TPEF {
    class InstructionElement;
/**
 * Code section.
 */
class CodeSection : public Section {
public:

    virtual ~CodeSection();

    virtual void addElement(SectionElement* element);

    virtual void setElement(Word index, SectionElement* element);

    virtual SectionType type() const;

    virtual InstructionElement* element(Word index) const;

    void clearInstructionCache() const;
    Word instructionCount() const;
    InstructionElement& instruction(Word index) const;
    Word instructionToSectionIndex(Word index) const;

    Word indexOfElement(const InstructionElement &elem) const;
    Word indexOfInstruction(const InstructionElement &elem) const;

    bool isInSection(const InstructionElement &elem) const;

protected:
    CodeSection(bool init);
    virtual Section* clone() const;

private:
    /// Prototype instance of section.
    static CodeSection proto_;

    /// Cache vector of element indexes of those instruction elements, which
    /// begins a new instruction.
    mutable std::vector<Word> instructionStartCache_;
    
    /// Cache of indexes of instruction elemenets.
    mutable std::map<const InstructionElement*, Word> elementIndexCache_;

    void initInstructionCache() const;

};

}

#endif
