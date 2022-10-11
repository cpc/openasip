/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file SoftwareBypasser.cc
 *
 * Definition of SoftwareBypasser interface.
 *
 * @author Pekka Jääskeläinen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "SoftwareBypasser.hh"

#include "MoveNodeGroup.hh"
#include "DataDependenceGraph.hh"
#include "ResourceManager.hh"

/**
 * Empty constructor.
 */
SoftwareBypasser::SoftwareBypasser() : bypassCount_(0), deadResultCount_(0){
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
 * @param bypassTrigger whether to bypass the trigger or not.
 * @return The count of bypassed moves.
 */
int
SoftwareBypasser::bypass(
    MoveNodeGroup& candidates,
    DataDependenceGraph& ddg,
    ResourceManager& rm, 
    bool /*bypassTrigger*/) {

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
 * @param removedMoves The dead result eliminated moves and their original 
 * cycles, to allow rescheduling of moves that were previously
 * resource (RF write port usually) constrained by the removed moves.
 * @return number of dead results killed
 */
int
SoftwareBypasser::removeDeadResults(
    MoveNodeGroup& candidates,
    DataDependenceGraph& ddg,
    ResourceManager& rm,
    std::set<std::pair<TTAProgram::Move*, int> >& removedMoves) {

    // use the arguments just to avoid compiler warnings with strict flags
    candidates.nodeCount();
    ddg.nodeCount();
    rm.machine();
    removedMoves.begin();
    return 0;
}

/*
 * Registers the selector being used to the bypasser.
 *
 * If the bypasser has been registered to the selector,
 * bypasses can notify the selector about dependence changes
 * (like WAR/WAW edge removal duo bypassing)
 *
 * Default implementation is empty, only actual bypasser implementations 
 * implement this.
 *
 * @param selector selector which bypasser notifies on some dependence changes.
 */
void 
SoftwareBypasser::setSelector(MoveNodeSelector*) {
}
