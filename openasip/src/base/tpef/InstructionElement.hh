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

    /// Maximum number of bytes that annotation may contain.
    static const size_t MAX_ANNOTATION_BYTES;
    
private:
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
