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
 * @file OutputFUResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the OutputFUResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "OutputFUResource.hh"
#include "FUPort.hh"
#include "Application.hh"
#include "Exception.hh"
#include "MoveNode.hh"
#include "ExecutionPipelineResource.hh"
#include "OutputPSocketResource.hh"

using TTAMachine::FUPort;
using TTAMachine::Port;

/**
 * Constructor with resource name
 * @param name Name of resource
 */
OutputFUResource::OutputFUResource(const std::string& name, int opCount) :
    FUResource(name, opCount) {}

/**
 * Empty destructor
 */
OutputFUResource::~OutputFUResource() {}

/**
 * Not to be used, aborts!
 *
 * Use version with third OutputPSocket operand!
 */
bool
OutputFUResource::canAssign(const int, const MoveNode&) const {
    abortWithError("Wrong method. Use canAssign with PSocket!");
    return false;
}

/**
 * Assign resource to given node for given cycle.
 *
 * There is actually nothing assigned here, only test
 * that the OutputPSocket of given FU can be assigned.
 * @param cycle Cycle to assign
 * @param node MoveNode assigned
 */
void
OutputFUResource::assign(const int cycle, MoveNode& node)
    throw (Exception) {

    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).isExecutionPipelineResource()) {
                SchedulingResource* res = &dependentResource(i, j);
                ExecutionPipelineResource* epRes =
                    dynamic_cast<ExecutionPipelineResource*>(res);
                epRes->assignSource(cycle, node);
                return;
            }
        }
    }
}

/**
* Unassign resource from given node for given cycle.
*
* It only tests that OutputPSocket to be unassigned is
* connected to given FU.
* @param cycle Cycle to remove assignment from
* @param node MoveNode to remove assignment from
*/
void
OutputFUResource::unassign(
    const int cycle,
    MoveNode& node)
    throw (Exception) {
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).isExecutionPipelineResource()) {
                SchedulingResource* res = &dependentResource(i, j);
                ExecutionPipelineResource* epRes =
                    dynamic_cast<ExecutionPipelineResource*>(res);
                epRes->unassignSource(cycle, node);
                return;
            }
        }
    }
}

/**
* Return true if resource can be assigned for given node in given cycle
* @param cycle Cycle to test
* @param node MoveNode to test
* @return true if node can be assigned to cycle
*/
bool
OutputFUResource::canAssign(
    const int cycle,
    const MoveNode& node,
    const TTAMachine::Port& resultPort) const {

    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).isExecutionPipelineResource()) {
                SchedulingResource* res = &dependentResource(i, j);
                ExecutionPipelineResource* epRes =
                    dynamic_cast<ExecutionPipelineResource*>(res);
                if (!epRes->canAssignSource(cycle, node, resultPort)){
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * Allways return true
 * @return true
 */
bool
OutputFUResource::isOutputFUResource() const {
    return true;
}

/**
 * Tests if all referred resources in dependent groups are of
 * proper types
 * @return true If all resources in dependent groups are
 *              input or output PSockets or ExecutionPipeline
 */
bool
OutputFUResource::validateDependentGroups() {
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
 * Tests if related resource groups are empty
 * @return true If all resources in related resource groups are
 *              empty
 */
bool
OutputFUResource::validateRelatedGroups() {
    for (int i = 0; i < relatedResourceGroupCount(); i++) {
        if (relatedResourceCount(i) > 0) {
            return false;
        }
    }
    return true;
}
