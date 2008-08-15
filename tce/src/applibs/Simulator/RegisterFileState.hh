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
 * @file RegisterFileState.hh
 *
 * Declaration of RegisterFileState class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_REGISTER_FILE_STATE_HH
#define TTA_REGISTER_FILE_STATE_HH

#include <vector>

#include "Exception.hh"

class RegisterState;

//////////////////////////////////////////////////////////////////////////////
// RegisterFileState
//////////////////////////////////////////////////////////////////////////////

/**
 * Class that manages RegisterState objects.
 */ 
class RegisterFileState {
public:
    explicit RegisterFileState(int size, int width);
    virtual ~RegisterFileState();

    virtual RegisterState& registerState(int index)
        throw (OutOfRange);

    virtual std::size_t registerCount() const;

private:
    /// Copying not allowed.
    RegisterFileState(const RegisterFileState&);
    /// Assignment not allowed.
    RegisterFileState& operator=(const RegisterFileState);

    /// Contains all the registers of the state.
    std::vector<RegisterState*> registerStates_;
};

//////////////////////////////////////////////////////////////////////////////
// NullRegisterFileState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models non-existing RegisterFileState.
 */
class NullRegisterFileState : public RegisterFileState {
public:
    static NullRegisterFileState& instance();

    virtual ~NullRegisterFileState();

    virtual RegisterState& registerState(int index)
        throw (OutOfRange);

private:
    NullRegisterFileState();
    /// Copying not allowed.
    NullRegisterFileState(const NullRegisterFileState&);
    /// Assignment not allowed.
    NullRegisterFileState& operator=(const NullRegisterFileState&);

    /// Unique instance of NullRegisterFileState.
    static NullRegisterFileState* instance_;
};

#endif
