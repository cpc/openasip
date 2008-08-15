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
