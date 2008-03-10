/**
 * @file FUResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the abstract FUResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
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
