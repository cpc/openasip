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
 * @file NullInstruction.hh
 *
 * Declaration of NullInstruction class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_INSTRUCTION_HH
#define TTA_NULL_INSTRUCTION_HH

#include "Instruction.hh"

namespace TTAProgram {

/**
 * A singleton class that represents a null instruction.
 *
 * Calling any method causes the program to abort.
 */
class NullInstruction : public Instruction {
public:
    virtual ~NullInstruction();
    static NullInstruction& instance();

    Procedure& parent() const throw (IllegalRegistration);
    void setParent(const Procedure& proc);
    bool isInProcedure() const;

    void addMove(Move& move) throw (ObjectAlreadyExists);
    int moveCount();
    const Move& move(int i) const throw (OutOfRange);

    void addImmediate(Immediate& imm) throw (ObjectAlreadyExists);
    int immediateCount();
    Immediate& immediate(int i) throw (OutOfRange);

    Address& address() const throw (IllegalRegistration);

    int size() const;

protected:
    NullInstruction();

private:
    /// Copying not allowed.
    NullInstruction(const NullInstruction&);
    /// Assignment not allowed.
    NullInstruction& operator=(const NullInstruction&);

    /// Unique instance of NullInstruction.
    static NullInstruction instance_;
};

}

#endif
