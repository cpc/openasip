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
 * @file DisassemblyImmediateAssignment.hh
 *
 * Declaration of DisassemblyImmediateAssignment class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-.cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_IMMEDIATE_ASSIGNMENT_HH
#define TTA_DISASSEMBLY_IMMEDIATE_ASSIGNMENT_HH

#include "DisassemblyInstructionSlot.hh"
#include "DisassemblyElement.hh"
#include "SimValue.hh"

/**
 * Represents piece of long immediate assignment in the disassembler.
 *
 * If the long immediate value is given using the constructor with value
 * parameter, disassembly string displays the value.
 */
class DisassemblyImmediateAssignment: public DisassemblyInstructionSlot {
public:
    DisassemblyImmediateAssignment(
	SimValue value,
	DisassemblyElement* destination);
    explicit DisassemblyImmediateAssignment(DisassemblyElement* destination);

    virtual ~DisassemblyImmediateAssignment();
    virtual std::string toString() const;

private:
    /// Coying not allowed.
    DisassemblyImmediateAssignment(const DisassemblyImmediateAssignment&);
    /// Assignment not allowed.
    DisassemblyImmediateAssignment& operator=(
	const DisassemblyImmediateAssignment&);

    /// Value of the immediate.
    SimValue value_;
    /// The destination register.
    DisassemblyElement* destination_;
    /// True, if the immediate has value set.
    bool hasValue_;
};

#endif
