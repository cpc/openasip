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
 * @file SoftwareBypasser.cc
 *
 * Definition of SoftwareBypasser interface.
 *
 * @author Pekka Jääskeläinen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "SoftwareBypasser.hh"

#include "MoveNodeGroup.hh"
#include "DataDependenceGraph.hh"
#include "ResourceManager.hh"

/**
 * Empty constructor.
 */
SoftwareBypasser::SoftwareBypasser() {
}

/**
 * Empty destructor.
 */
SoftwareBypasser::~SoftwareBypasser() {
}

/**
 * Apply software bypassing to as many moves in the given MoveNodeGroup
 * as possible.
 *
 * The given DDG can be used to inspect whether software bypassing is possible.
 *
 * @param candidates The moves to which apply software bypassing, if possible.
 * @param ddg The data dependence grap in which the movenodes belong to.
 * @param rm The resource manager which is used to check for resource
 *        availability.
 * @return The count of bypassed moves.
 */
int
SoftwareBypasser::bypass(
    MoveNodeGroup& candidates,
    DataDependenceGraph& ddg,
    ResourceManager& rm) {

    // use the arguments just to avoid compiler warnings with strict flags
    candidates.nodeCount();
    ddg.nodeCount();
    rm.machine();
    return 0;
}

/**
 * Remove software bypassing from as many moves in the given MoveNodeGroup
 * as possible.
 *
 * @param candidates The moves from which to remove software bypassing, if
 * possible.
 * @param ddg The data dependence grap in which the movenodes belong to.
 * @param rm The resource manager which is used to check for resource
 *        availability.
 */
void
SoftwareBypasser::removeBypass(
    MoveNodeGroup& candidates,
    DataDependenceGraph& ddg,
    ResourceManager& rm) {

    // use the arguments just to avoid compiler warnings with strict flags
    candidates.nodeCount();
    ddg.nodeCount();
    rm.machine();
    return;
}

/**
 * Remove dead result moves for each bypassed move of given MoveNodeGroup
 * if possible. That is, no other result reads of result exists.
 * This assumes that result moves in MoveNodeGroup are also scheduled.
 *
 * @param candidates The moves for which remove dead result writes
 * @param ddg The data dependence grap in which the movenodes belong to.
 * @param rm The resource manager which is used to check for resource
 *        availability.
 */
int
SoftwareBypasser::removeDeadResults(
    MoveNodeGroup& candidates,
    DataDependenceGraph& ddg,
    ResourceManager& rm) {

    // use the arguments just to avoid compiler warnings with strict flags
    candidates.nodeCount();
    ddg.nodeCount();
    rm.machine();
    return 0;
}

/*
 * Registers the selector being used to the bypasser.
 *
 * If the bypasser has been registered to the selector,
 * bypasses can notify the selector about dependence changes
 * (like WAR/WAW edge removal duo bypassing)
 *
 * Default implementation is empty, only actual byöpasser implementations 
 * implement this.
 *
 * @param selector selector which bypasser notifies on some dependence changes.
 */
void 
SoftwareBypasser::setSelector(MoveNodeSelector*) {
}
