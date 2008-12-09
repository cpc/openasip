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
 * @file FUResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the abstract FUResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "FUResource.hh"
#include "Application.hh"
#include "Conversion.hh"
/**
 * Constructor defining resource name
 * @param name Name of resource
 */
FUResource::FUResource(const std::string& name) : SchedulingResource(name) {}

/**
 * Empty destructor
 */
FUResource::~FUResource() {}

/**
 * Test if resource FUResource is used in given cycle, meaning
 * InputPSocket or OutputPSocket is in use or ExecutionPipeline
 * is inUse.
 * @param cycle Cycle which to test
 * @return True if FUResource is already used in cycle
 */
bool
FUResource::isInUse(const int cycle) const {
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).isInUse(cycle)) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Test if resource FUResource is available.
 * Not all of the PSocket are inUse.
 * @param cycle Cycle which to test
 * @return True if FUResource is available in cycle
 */
bool
FUResource::isAvailable(const int cycle) const {
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).isInputPSocketResource() ||
                dependentResource(i, j).isOutputPSocketResource()) {
                if (!dependentResource(i, j).isInUse(cycle)) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Assign resource to given node for given cycle
 * @param cycle Cycle to assign
 * @param node MoveNode assigned
 */
void
FUResource::assign(const int, MoveNode&)
    throw (Exception) {
    // Implemented in derived classes
    abortWithError("assign of FUResource called!");
}

/**
 * Unassign resource from given node for given cycle
 * @param cycle Cycle to remove assignment from
 * @param node MoveNode to remove assignment from
 */
void
FUResource::unassign(const int, MoveNode&)
    throw (Exception) {
    // Implemented in derived classes
    abortWithError("unassign of FUResource called!");
}

/**
 * Return true if resource can be assigned for given resource in given cycle
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @return true if node can be assigned to cycle
 */
bool
FUResource::canAssign(const int, const MoveNode&) const {
    // Implemented in derived classes
    abortWithError("canAssign of FUResource called!");
    return false;
}
