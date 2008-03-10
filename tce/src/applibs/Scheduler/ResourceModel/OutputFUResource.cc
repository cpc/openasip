/**
 * @file OutputFUResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the OutputFUResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "OutputFUResource.hh"
#include "FUPort.hh"
#include "Application.hh"
#include "Exception.hh"
#include "MoveNode.hh"
#include "ExecutionPipelineResource.hh"

using TTAMachine::FUPort;
using TTAMachine::Port;

/**
 * Constructor with resource name
 * @param name Name of resource
 */
OutputFUResource::OutputFUResource(const std::string& name) :
    FUResource(name) {}

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
OutputFUResource::assign(
    const int cycle,
    MoveNode& node)
    throw (Exception) {
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).isExecutionPipelineResource()) {
                SchedulingResource* res = &dependentResource(i, j);
                ExecutionPipelineResource* epRes =
                    dynamic_cast<ExecutionPipelineResource*>(res);
                epRes->assign(cycle, node, true);
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
                epRes->unassign(cycle, node, true);
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
    const OutputPSocketResource& pSocket) const {

    if (!hasDependentResource(pSocket)) {
        std::string msg = "OutputPSocket ";
        msg += pSocket.name();
        msg += " is not connected to ";
        msg += name();
        msg += "!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
        return false;
    }
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).isExecutionPipelineResource()) {
                SchedulingResource* res = &dependentResource(i, j);
                ExecutionPipelineResource* epRes =
                    dynamic_cast<ExecutionPipelineResource*>(res);
                if (!epRes->canAssign(cycle, node, pSocket)){
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
