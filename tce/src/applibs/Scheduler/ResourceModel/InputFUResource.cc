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
 * @file InputFUResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the InputFUResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "InputFUResource.hh"
#include "FUPort.hh"
#include "Application.hh"
#include "Exception.hh"
#include "MoveNode.hh"
#include "ExecutionPipelineResource.hh"

using TTAMachine::BaseFUPort;
using TTAMachine::FUPort;
using TTAMachine::Port;

/**
 * Constructor defining name of resource.
 *
 * @param name Name of resource
 */
InputFUResource::InputFUResource(const std::string& name) : FUResource(name) {
}

/**
 * Empty destructor
 */
InputFUResource::~InputFUResource() {    
}

/**
 * Not to be used, aborts!
 *
 * Use version with third InputPSocket operand!
 */
bool
InputFUResource::canAssign(const int, const MoveNode&) const {
    abortWithError("Wrong method. Use canAssign with PSocket!");
    return false;
}

/**
 * Assign resource to given node for given cycle
 * as a side effect, assign also execution pipeline for triggering moves.
 *
 * @param cycle Cycle to assign
 * @param node MoveNode assigned
 * @param pSocket InputPSocket used for writting operand
 * @throw In case the assignment is impossible, should have been tested
 * with canAssing first.
 */
void
InputFUResource::assign(
    const int cycle,
    MoveNode& node)
    throw (Exception) {

    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).isExecutionPipelineResource()) {
                SchedulingResource* res = &dependentResource(i, j);
                ExecutionPipelineResource* epRes =
                    dynamic_cast<ExecutionPipelineResource*>(res);
                epRes->assign(cycle, node, false);
                increaseUseCount();
                return;
            }
        }
    }
    abortWithError("InputFUResource has no execution pipeline registered!");
}

/**
 * Unassign resource from given node for given cycle,
 * for triggering moves also unassign execution pipeline.
 *
 * @param cycle Cycle to remove assignment from
 * @param node MoveNode to remove assignment from
 * @param pSocket InputPSocket used for writting operand
 * @throw In case PSocket is not connected to FU
 */
void
InputFUResource::unassign(
    const int cycle,
    MoveNode& node)
    throw (Exception) {
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).isExecutionPipelineResource()) {
                SchedulingResource* res = &dependentResource(i, j);
                ExecutionPipelineResource* epRes =
                    dynamic_cast<ExecutionPipelineResource*>(res);
                epRes->unassign(cycle, node, false);
                decreaseUseCount();
                return;
            }
        }
    }
    abortWithError("InputFUResource has no execution pipeline registered!");
}

/**
 * Return true if resource can be assigned for given node in given cycle.
 *
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @param pSocket InputPSocket used for writting operand
 * @return true if node can be assigned to cycle
 * @throw Internal error, PSocket is not conencted to FU or ExecutionPipeline
 */
bool
InputFUResource::canAssign(
    const int cycle,
    const MoveNode& node,
    const InputPSocketResource& pSocket,
    const bool triggers) const
    throw (Exception) {

    if (!hasDependentResource(pSocket)) {
        std::string msg = "InputPSocket ";
        msg += pSocket.name();
        msg += " is not connected to ";
        msg += name();
        msg += "!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
        return false;
    }

    if (!pSocket.canAssign(cycle, node)) {
        return false;
    }

    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).isExecutionPipelineResource()) {
                SchedulingResource* res = &dependentResource(i, j);
                ExecutionPipelineResource* epRes =
                    dynamic_cast<ExecutionPipelineResource*>(res);
                if (!(epRes->canAssign(cycle, node, pSocket, triggers))) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Test if resource InputFUResource is available.
 *
 * Not all the PSocket are inUse and there is some operation possible for
 * triggering moves.
 *
 * @param cycle Cycle which to test
 * @return True if FUResource is available in cycle
 */
bool
InputFUResource::isAvailable(const int cycle) const {
    // Test availability of PSockets using parent class
    if (!FUResource::isAvailable(cycle)) {
        return false;
    }
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).isExecutionPipelineResource() &&
                dependentResource(i, j).isAvailable(cycle)) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Test if resource InputFUResource is used in given cycle.
 *
 * True if some of InputPSockets are already used or there is
 * operation already in execution pipeline.
 *
 * @param cycle Cycle which to test
 * @return True if InputFUResource is already used in cycle
 */
bool
InputFUResource::isInUse(const int cycle) const {
    // Test isInUse of PSockets using parent class
    if (FUResource::isInUse(cycle)) {
        return true;
    }
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).isExecutionPipelineResource() &&
                dependentResource(i, j).isInUse(cycle)) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Allways return true
 *
 * @return true
 */
bool
InputFUResource::isInputFUResource() const {
    return true;
}

/**
 * Tests if all referred resource in dependent groups are of
 * proper types.
 *
 * @return true If all resources in dependent groups are
 *              input or output PSockets or executionPipeline
 */
bool
InputFUResource::validateDependentGroups() {
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (!(dependentResource(i, j).isInputPSocketResource() ||
                dependentResource(i, j).isOutputPSocketResource() ||
                dependentResource(i, j).isExecutionPipelineResource())) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Tests if related resource groups are empty.
 *
 * @return true If all related resource groups are
 *              empty
 */
bool
InputFUResource::validateRelatedGroups() {
    for (int i = 0; i < relatedResourceGroupCount(); i++) {
        if (relatedResourceCount(i) > 0) {
            return false;
        }
    }
    return true;
}
