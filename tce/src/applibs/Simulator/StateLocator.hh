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
 * @file StateLocator.hh
 *
 * Declaration of StateLocator class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_STATE_LOCATOR_HH
#define TTA_STATE_LOCATOR_HH

#include <map>

#include "Exception.hh"

class StateData;

namespace TTAMachine {
    class MachinePart;
}

/**
 * Class that makes it possible to lookup state objects corresponding to
 * MOM component instances.
 */
class StateLocator {
public:
    StateLocator();
    virtual ~StateLocator();

    void addState(
        const TTAMachine::MachinePart& momComponent, 
        StateData& state);
    StateData& state(const TTAMachine::MachinePart& momComponent) const
        throw (IllegalParameters);
private:
    /// Copying not allowed.
    StateLocator(const StateLocator&);
    /// Assignment not allowed.
    StateLocator& operator=(const StateLocator&);

    /// State objects indexed by MOM components.
    typedef std::map<const TTAMachine::MachinePart*, StateData*> 
    StateContainer;
    
    /// Contains all the states indexed by MOM components.
    StateContainer states_;
};

#endif
