/**
 * @file InputPSocketResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the InputPSocketResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "InputPSocketResource.hh"

/**
 * Constructor defining resource name
 * @param name Name of resource
 */
InputPSocketResource::InputPSocketResource(const std::string& name) :
    PSocketResource(name) {}

/**
 * Empty destructor
 */
InputPSocketResource::~InputPSocketResource() {}

/**
 * Allways return true
 * @return true
 */
bool
InputPSocketResource::isInputPSocketResource() const {
    return true;
}

/**
 * Tests if all referred resources in dependent groups are of
 * proper types
 * @return true Allways true, dep. groups are empty
 */
bool
InputPSocketResource::validateDependentGroups() {
    for (int i = 0; i < dependentResourceGroupCount(); i++) {
        if (dependentResourceCount(i) > 0) {
            return false;
        }
    }
    return true;
}

/**
 * Tests if all referred resources in related groups are of
 * proper types
 * @return true If all resources in related groups are
 *              Segment or InputFU resources
 */
bool
InputPSocketResource::validateRelatedGroups() {
    for (int i = 0; i < relatedResourceGroupCount(); i++) {
        for (int j = 0, count = relatedResourceCount(i); j < count; j++) {
            if (!(relatedResource(i, j).isInputFUResource() ||
                relatedResource(i, j).isSegmentResource()))
                return false;
        }
    }
    return true;
}
