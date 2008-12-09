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
 * @file CodeSection.hh
 *
 * Declaration of CodeSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
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
