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
 * @file InstructionReferenceImpl.hh
 *
 * Declaration of InstructionReferenceImpl class.
 *
 * This class provides the internal tracking of instruction references to
 * a single instruction. This class contains information about
 * all references pointing to this instructions. When the reference count
 * reaches 0, this class is automatically deleted.
 *
 * @author Heikki Kultala 2009 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_REFERENCE_IMPL_HH
#define TTA_INSTRUCTION_REFERENCE_IMPL_HH

#include <set>

namespace TTAProgram {
    class Instruction;
    class InstructionReference;
    class InstructionReferenceManager;

class InstructionReferenceImpl {
public:
    InstructionReferenceImpl(
        TTAProgram::Instruction& ins, 
        TTAProgram::InstructionReferenceManager& irm);

    ~InstructionReferenceImpl();
    void nullify();
    void addRef(InstructionReference& ref);
    bool removeRef(InstructionReference& ref);
    InstructionReferenceManager& referencemanager();
    void setInstruction(Instruction& ins);
    void merge(InstructionReferenceImpl& other);
    inline Instruction& instruction();
    inline unsigned int count();
    const InstructionReference& ref();
private:
    Instruction* ins_;
    std::set<InstructionReference*> refs_;
    InstructionReferenceManager* refMan_;
    InstructionReferenceImpl();
    InstructionReferenceImpl(const InstructionReferenceImpl&);
};

#include "InstructionReferenceImpl.icc"
}
#endif
