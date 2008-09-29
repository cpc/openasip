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
 * @file PSocketResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the abstract PSocketResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */
#include <iostream>
#include "PSocketResource.hh"
#include "MapTools.hh"
#include "Conversion.hh"
/**
 * Constructor defining name of resource
 * @param name Name of resource
 */
PSocketResource::PSocketResource(const std::string& name) :
    SchedulingResource(name) {}

/**
 * Empty constructor
 */
PSocketResource::~PSocketResource() {}

/**
 * Test if resource PSocketResource is used in given cycle
 *
 * The PSocket is inUse if it is read from or written to at
 * least once.
  *
 * @param cycle Cycle which to test
 * @return True if p-socket is already used in cycle
 */
bool
PSocketResource::isInUse(const int cycle) const {
    if (MapTools::containsKey(resourceRecord_, cycle)) {
        if (MapTools::valueForKey<int>(resourceRecord_, cycle) > 0) {
            return true;
        }
    }
    return false;
}

/**
 * Test if resource PSocketResource is available
 *
 * @param cycle Cycle which to test
 * @return True if p-socket is available in cycle
 */
bool
PSocketResource::isAvailable(const int cycle) const {
    return !isInUse(cycle);
}

/**
 * Assign resource to given node for given cycle.
 *
 * @param cycle Cycle to assign
 * @param node MoveNode to assign
 * @throw In case PSocket can not be assigned
 * @note Exception is internal error, resource should be tested
 * with canAssign() before assign() is called.
 */
void
PSocketResource::assign(const int cycle, MoveNode&)
    throw (Exception) {
    if (MapTools::containsKey(resourceRecord_, cycle)) {
        resourceRecord_[cycle]++;
    } else {
        resourceRecord_[cycle] = 1;
    }
    increaseUseCount();
    return;
}

/**
 * Unassign resource from given node for given cycle.
 *
 * @param cycle Cycle to remove assignment from
 * @param node MoveNode to remove assignment from
 * @throw In case the PSocket was not assigned before.
 */
void
PSocketResource::unassign(const int cycle, MoveNode&)
    throw (Exception) {
    if (isInUse(cycle)) {
        resourceRecord_[cycle]--;
        decreaseUseCount();
        return;
    }
    std::string msg = "PSocket was not assigned so it can not be unassigned!";
    throw InvalidData(__FILE__, __LINE__, __func__, msg);
}

/**
 * Return true if resource can be assigned for given resource in given cycle.
 *
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @return true if node can be assigned to cycle
 */
bool
PSocketResource::canAssign(const int cycle, const MoveNode&) const {
    if (isAvailable(cycle)) {
        return true;
    } else {
        return false;
    }
}
