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
 * @file InstructionElement.hh
 *
 * Declaration of InstructionElement and InstructionAnnotation classes.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_INSTRUCTION_ELEMENT_HH
#define TTA_INSTRUCTION_ELEMENT_HH

#include "BaseType.hh"
#include "SectionElement.hh"

namespace TPEF {

/**
 * Instruction annotation.
 *
 * Inline annotations allow to argument an instruction element
 * with additional application-dependent information without
 * disrupting the normal work of application.
 */
class InstructionAnnotation {
public:
    InstructionAnnotation(Word anId, const std::vector<Byte>& payload);
    InstructionAnnotation(Word anId);
    ~InstructionAnnotation();

    Word id() const;

    Byte byte(Byte index) const;
    const std::vector<Byte>& payload() const;

    void addByte(Byte aByte);

    Byte size() const;

private:
    /// Maximum number of bytes that annotation may contain.
    static const int MAX_ANNOTATION_BYTES;
    /// Identifcation code of annotation.
    Word id_;
    /// Data of annotation.
    std::vector<Byte> payLoad_;
};

/**
 * Abstract base class for all instructions of CodeSection.
 */
class InstructionElement : public SectionElement {
public:
    virtual ~InstructionElement();

    bool isMove() const;
    bool isImmediate() const;

    bool begin() const;
    void setBegin(bool isBegin);

    InstructionAnnotation* annotation(Word index) const;
    void addAnnotation(InstructionAnnotation *anAnnotation);
    Word annotationCount() const;

protected:
    InstructionElement(bool isMove);

private:
    /// Is the first element of whole instruction.
    bool begin_;
    /// Is move or immediate.
    bool isMove_;

    /// All annotations.
    std::vector<InstructionAnnotation*> annotations_;
};
}

#include "InstructionElement.icc"

#endif
