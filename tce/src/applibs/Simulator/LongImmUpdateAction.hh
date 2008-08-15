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
 * @file LongImmUpdateAction.hh
 *
 * Declaration of LongImmUpdateAction class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_LONG_IMM_UPDATE_ACTION_HH
#define TTA_LONG_IMM_UPDATE_ACTION_HH

#include "SimValue.hh"
class LongImmediateRegisterState;

/**
 * Updates the value of the long immediate register state.
 */
class LongImmUpdateAction {
public:
    LongImmUpdateAction(SimValue value, LongImmediateRegisterState& state);
    virtual ~LongImmUpdateAction();

    void execute();

private:
    /// Copying not allowed.
    LongImmUpdateAction(const LongImmUpdateAction&);
    /// Assignment not allowed.
    LongImmUpdateAction& operator=(const LongImmUpdateAction&);

    /// Value to be updated.
    SimValue value_;
    /// Destination register.
    LongImmediateRegisterState& state_;
};

#endif
