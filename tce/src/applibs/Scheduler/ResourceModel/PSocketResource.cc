/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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

/**
 * Clears bookkeeping of the scheduling resource. 
 * 
 * After this call the state of the resource should be identical to a 
 * newly-created and initialized resource.
 */
void
PSocketResource::clear() {
    SchedulingResource::clear();
    resourceRecord_.clear();
}
