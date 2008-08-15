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
 * @file DisassemblyImmediateAssignment.hh
 *
 * Declaration of DisassemblyImmediateAssignment class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@.cs.tut.fi)
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
