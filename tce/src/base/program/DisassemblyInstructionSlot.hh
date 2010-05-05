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
 * @file DisassemblyInstructionSlot.hh
 *
 * Declaration of DisassemblyInstructionSlot class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-.cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_INSTRUCTION_SLOT_HH
#define TTA_DISASSEMBLY_INSTRUCTION_SLOT_HH

#include <string>

#include "DisassemblyAnnotation.hh"

/**
 * An abstract base class for instruction disassembly slots.
 */
class DisassemblyInstructionSlot {
public:
    virtual ~DisassemblyInstructionSlot();
    virtual std::string toString() const = 0;

    DisassemblyAnnotation& annotation(int index) const;
    void addAnnotation(DisassemblyAnnotation* annotation);
    int annotationCount() const;

protected:
    DisassemblyInstructionSlot();

private:
    /// Copying not allowed.
    DisassemblyInstructionSlot(const DisassemblyInstructionSlot&);
    /// Assignment not allowed.
    DisassemblyInstructionSlot& operator=(const DisassemblyInstructionSlot&);
    /// Annotationes of move or long immediates.
    std::vector<DisassemblyAnnotation*> annotationes_;
};

#endif
