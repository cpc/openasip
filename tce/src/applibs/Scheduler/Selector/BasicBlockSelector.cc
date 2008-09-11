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
 * @file BasicBlockSelector.hh
 *
 * Dummy implementation of BasicBlockSelector interface.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include "BasicBlockSelector.hh"
#include "Application.hh"

/**
 * Constructor for BasicBlockSelector class.
 */
BasicBlockSelector::BasicBlockSelector() {
}

/**
 * Destructor for BasicBlockSelector class.
 */
BasicBlockSelector::~BasicBlockSelector() {
}

/**
 * Builds the scopes. 
 *
 * This function has to be called before getScope can be called.
 *
 * @note Why? What's wrong with constructors?
 */
void 
BasicBlockSelector::buildScopes() {
    abortWithError("Not yet implemented.");
}

/**
 * Returns a basic block to schedule next.
 *
 * The client is responsible for deallocating the returned basic block 
 * after using. This returns all basic blocks in the program one-by-one, one
 * procedure at a time, until all basic blocks have been returned.
 *
 * @return SchedulingScope, or NULL if all scopes already given.
 */
SchedulingScope*
BasicBlockSelector::getScope() {
    abortWithError("Not yet implemented.");
    return NULL;
}
