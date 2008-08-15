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
 * @file UnboundRegisterFileState.hh
 *
 * Declaration of UnboundRegisterFileState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_UNBOUND_REGISTER_FILE_STATE_HH
#define TTA_UNBOUND_REGISTER_FILE_STATE_HH

#include <vector>

#include "RegisterFileState.hh"

class RegisterState;

/**
 * Holds unlimited number of RegisterState objects.
 */
class UnboundRegisterFileState : public RegisterFileState {
public:
    UnboundRegisterFileState();
    virtual ~UnboundRegisterFileState();

    virtual RegisterState& registerState(int index)
        throw (OutOfRange);

    virtual std::size_t registerCount() const;

    bool isRegisterInUse(std::size_t index) const;

private:
    /// Copying not allowed.
    UnboundRegisterFileState(const UnboundRegisterFileState&);
    /// Assignment not allowed.
    UnboundRegisterFileState& operator=(const UnboundRegisterFileState&);

    /// Contains all the registers of the register file.
    std::vector<RegisterState*> registerStates_;
};

#endif
