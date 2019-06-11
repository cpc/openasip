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
 * @file LineNumSection.cc
 *
 * Non-inline definitions of LineNumSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "LineNumSection.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

LineNumSection LineNumSection::proto_(true);

/**
 * Constructor.
 *
 * @param init True if instance should register itself to base class.
 */
LineNumSection::LineNumSection(bool init) : Section(),
    codeSection_(&SafePointer::null) {

    if (init) {
        Section::registerSection(this);
    }

    unsetFlagNoBits();
    unsetFlagVLen();
    setStartingAddress(0);
}

/**
 * Destructor.
 */
LineNumSection::~LineNumSection() {
}

/**
 * Returns section's type.
 *
 * @return Type of section.
 */
Section::SectionType
LineNumSection::type() const {
    return ST_LINENO;
}

/**
 * Creates an instance of section.
 *
 * @return Newly created section.
 */
Section*
LineNumSection::clone() const {
    return new LineNumSection(false);
}

/**
 * Sets text section which instructions are referred in this section.
 *
 * @param Section to set.
 */
void
LineNumSection::setCodeSection(
    const ReferenceManager::SafePointer* codeSect) {
    codeSection_ = codeSect;
}

/**
 * Sets text section which instructions are referred in this section.
 *
 * @param Section to set.
 */
void
LineNumSection::setCodeSection(CodeSection* codeSect) {
    codeSection_ = SafePointer::replaceReference(codeSection_, codeSect);
}

/**
 * Returns text section which lines are stored in this section.
 *
 * @return Text section which lines are stored in this section.
 */
CodeSection*
LineNumSection::codeSection() const {
    return dynamic_cast<CodeSection*>(codeSection_->pointer());
}

}
