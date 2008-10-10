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
 * @file MoveNodeSelector.cc
 *
 * Implementation of MoveNodeSelector interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include "MoveNodeSelector.hh"
#include "Application.hh"

/**
 * Constructor.
 */
MoveNodeSelector::MoveNodeSelector() {
}

/**
 * Destructor.
 */
MoveNodeSelector::~MoveNodeSelector() {
}

/**
 * Returns a group of move nodes which should be scheduled next.
 *
 * By default returns an empty group. A non-empty group is guaranteed to
 * contain at least one unscheduled move, but not all of them necessarily
 * are.
 *
 * @return Move node group.
 */
MoveNodeGroup
MoveNodeSelector::candidates() {
    return MoveNodeGroup();
}

/**
 * This should be called by the client as soon as a MoveNode is scheduled
 * in order to update the internal state of the selector.
 *
 * @param node The scheduled MoveNode.
 */
void
MoveNodeSelector::notifyScheduled(MoveNode& node) {
    assert(&node && "Should never be called. Here just to avoid warning.");
}

/** 
 * This should be called by the client when some incoming dependence to
 * a moveNode is removed.
 *
 * @param node Node which has lost some incoming dependence
 */
void
MoveNodeSelector::mightBeReady(MoveNode& node) {
    assert(&node && "Should never be called. Here just to avoid warning.");
}
