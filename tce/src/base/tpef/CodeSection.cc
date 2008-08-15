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
 * @file CodeSection.cc
 *
 * Definition of CodeSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "CodeSection.hh"
#include "InstructionElement.hh"

namespace TPEF {

CodeSection CodeSection::proto_(true);

/**
 * Constructor.
 *
 * @param init True if registeration is wanted.
 */
CodeSection::CodeSection(bool init) :
    Section() {
    if (init) {
        Section::registerSection(this);
    }
    setFlagVLen();
    unsetFlagNoBits();
}

/**
 * Destructor.
 */
CodeSection::~CodeSection() {
}

/**
 * Returns section's type.
 *
 * @return Type of section.
 */
Section::SectionType
CodeSection::type() const {
    return ST_CODE;
}

/**
 * Creates an instance of CodeSection.
 *
 * @return Newly created section.
 */
Section*
CodeSection::clone() const {
    return new CodeSection(false);
}

/**
 * Returns requested instruction element.
 *
 * @param index Index of requested element.
 * @return Requested element.
 */
InstructionElement*
CodeSection::element(Word index) const {
    return dynamic_cast<InstructionElement*>(Section::element(index));
}

/**
 * Clears instruction startpoint cache.
 */
void
CodeSection::clearInstructionCache() const {
    instructionStartCache_.clear();
    elementIndexCache_.clear();
}

/**
 * Initializes instruction cache.
 *
 * Scans through section and marks element indexes of those
 * instruction elements that starts a new instruction.
 *
 * Also sets section element index for every element.
 */
void
CodeSection::initInstructionCache() const {

    // both caches must be initialized or uninitilized
    assert(instructionStartCache_.empty() == elementIndexCache_.empty());

    if (instructionStartCache_.empty()) {
        for (Word i = 0; i < elementCount(); i++) {
            InstructionElement* elem = element(i);

            // cache start index of instruction
            if (elem->begin()) {
                instructionStartCache_.push_back(i);
            }

            // cache element index of instruction element
            elementIndexCache_[elem] = i;

        }
    }

    // there must be at least one instruction start
    assert(!instructionStartCache_.empty());
    assert(!elementIndexCache_.empty());

}

/**
 * Returns number of instructions stored in section.
 *
 * Each instruction elements that starts new instruction are marked
 * with begin() flag.
 *
 * If section elements are changed, cache must be cleared before
 * calling this function.
 *
 * @return Number of instructions in code section.
 */
Word
CodeSection::instructionCount() const {
    initInstructionCache();
    return instructionStartCache_.size();
}

/**
 * Returns starting element of requested instruction.
 *
 * See. instructionCount() for more information.
 *
 * If section elements are changed, cache must be cleared before
 * calling this function.
 *
 * @param index Index of requested instruction.
 * @return Starting element of requested instruction.
 */
InstructionElement&
CodeSection::instruction(Word index) const {
    return *element(instructionToSectionIndex(index));
}

/**
 * Returns element index of starting element of requested instruction.
 *
 * I.e. converts instruction index to instruction element index.
 *
 * See. instructionCount() for more information.
 *
 * If section elements are changed, cache must be cleared before
 * calling this function.
 *
 * @param index Index of instruction
 * @return Instruction element index of requested instruction.
 */
Word
CodeSection::instructionToSectionIndex(Word index) const {
    initInstructionCache();
    return instructionStartCache_[index];
}

/**
 * Returns index of instruction element.
 *
 * If section elements are changed, cache must be cleared before
 * calling this function.
 *
 * @param elem Element whose index is returned.
 * @return index of instruction element.
 */
Word
CodeSection::indexOfElement(const InstructionElement &elem) const {
    initInstructionCache();
    return MapTools::valueForKey<Word>(elementIndexCache_, &elem);

}

/**
 * Returns true if element is found from section.
 *
 * If section elements are changed, cache must be cleared before
 * calling this function.
 *
 * @param elem Element to check.
 * @return True if element is found from section.
 */
bool
CodeSection::isInSection(const InstructionElement &elem) const {
    initInstructionCache();
    return MapTools::containsKey(elementIndexCache_, &elem);
}

/**
 * Returns index of instruction of requested element.
 *
 * If requested element is in middle of instruction, then index of that
 * instruction is returned.
 *
 * If section elements are changed, cache must be cleared before
 * calling this function.
 *
 * @param elem Element whose instruction index is returned.
 * @return index of instruction element.
 */
Word
CodeSection::indexOfInstruction(const InstructionElement &elem) const {
    initInstructionCache();

    // get instruction element index
    Word elementIndex = 
        MapTools::valueForKey<Word>(elementIndexCache_, &elem);

    Word instrCount = instructionCount();
    int first = 0;
    int last = instrCount - 1;
    Word key = elementIndex;

    int middle = 0;

    // binary search from instructionStartCache for finding instruction index
    while (first <= last) {
        middle = (first + last) / 2;
        if (key > instructionStartCache_[middle]) {
            first = middle + 1;
        } else if (key < instructionStartCache_[middle]) {
            last = middle - 1;
        } else {
            break;
        }
    }

    return middle;
}

/**
 * Adds an element to section and clears internal caches.
 *
 * @param element Element that is added to section.
 */
void 
CodeSection::addElement(SectionElement* element) {
    Section::addElement(element);
    clearInstructionCache();
}

/**
 * Sets replaces an element with another and clears internal caches.
 *
 * @param index Index of element that is replaced.
 * @param element Element that is set to given index.
 */
void 
CodeSection::setElement(Word index, SectionElement* element) {
    Section::setElement(index, element);
    clearInstructionCache();
}

}
