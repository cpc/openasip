/**
 * @file ITemplateResource.cc
 *
 * Implementation of ITemplateResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "ITemplateResource.hh"
#include "MapTools.hh"
#include "Application.hh"

/**
 * Constructor.
 *
 * @param name Name of template.
 */
ITemplateResource::ITemplateResource(const std::string& name):
    SchedulingResource(name) {
}

/**
 * Destructor.
 */
ITemplateResource::~ITemplateResource() {
}

/**
 * Test if instruction template resource is used in given cycle.
 *
 * @param cycle Cycle to test.
 * @return True if resource is used in given cycle.
 */
bool
ITemplateResource::isInUse(const int cycle) const {
    if (MapTools::containsKey(resourceRecord_, cycle)) {
        if (MapTools::valueForKey<int>(resourceRecord_, cycle) != 0) {
            return true;
        }
    }
    return false;
}

/**
 * Test if instruction tempalte resource is available in given cycle.
 *
 * @param cycle Cycle to test.
 * @return True if resource available in given cycle.
 */
bool
ITemplateResource::isAvailable(const int cycle) const {
    return !isInUse(cycle);
}

/**
 * The overridden method should be used instead.
 */
void
ITemplateResource::assign(const int, MoveNode&)
    throw (Exception) {
    abortWithError("Not implemented!");
}

/**
 * Assign instruction template resource to given cycle.
 *
 * @param cycle Cycle to assign.
 */
void
ITemplateResource::assign(const int cycle)
    throw (Exception) {
    if (canAssign(cycle)) {
        if (MapTools::containsKey(resourceRecord_, cycle)){
            resourceRecord_[cycle]++;
        } else {
            resourceRecord_[cycle] = 1;
        }
    }
}

/**
 * The overridden method should be used instead.
 */
void
ITemplateResource::unassign(const int, MoveNode&)
    throw (Exception) {
    abortWithError("Not implemented!");
}

/**
 * Unassign instruction template from given cycle.
 *
 * @param cycle Cycle to remove assignment from.
 */
void
ITemplateResource::unassign(const int cycle)
    throw (Exception) {
    if (isInUse(cycle)) {
        resourceRecord_[cycle]--;
    }
}

/**
 * The overridden method should be used instead.
 */
bool
ITemplateResource::canAssign(const int, const MoveNode&) const {
    abortWithError("Not implemented!");
    return false;
}

/**
 * Return true if instruction template resource can be assigned on given cycle.
 *
 * @param cycle Cycle to test.
 * @return True if template can be assigned to given cycle.
 */
bool
ITemplateResource::canAssign(const int cycle) const {
    return !isInUse(cycle);
}

/**
 * Return true always.
 *
 * @return True always.
 */
bool
ITemplateResource::isITemplateResource() const {
    return true;
}

/**
 * Tests if all referred resources in dependent groups are of
 * proper types.
 *
 * @return True If all resources in dependent groups are
 *              immediate units or buses (move slots).
 */
bool
ITemplateResource::validateDependentGroups() {
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (!(dependentResource(i, j).isIUResource() ||
                dependentResource(i, j).isBusResource())) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Tests if related resource groups are empty.
 *
 * @return True if related resource groups are empty.
 * @note This resource type has no related resources.
 */
bool
ITemplateResource::validateRelatedGroups() {
    for (int i = 0; i < relatedResourceGroupCount(); i++) {
        if (relatedResourceCount(i) > 0) {
            return false;
        }
    }
    return true;
}
