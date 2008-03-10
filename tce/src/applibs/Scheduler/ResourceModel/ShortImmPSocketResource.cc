/**
 * @file ShortImmPSocketResource.cc
 *
 * Implementation of ShortImmPSocketResource class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "ShortImmPSocketResource.hh"

/**
 * Constructor.
 *
 * @param name Name of socket.
 */
ShortImmPSocketResource::ShortImmPSocketResource(
    const std::string& name,
    int immediateWidth, bool signExtends):
        OutputPSocketResource(name), immediateWidth_(immediateWidth),
        signExtends_(signExtends) {}

/**
 * Destructor.
 */
ShortImmPSocketResource::~ShortImmPSocketResource() {}

/**
 * Return true always.
 *
 * @return True always.
 */
bool
ShortImmPSocketResource::isShortImmPSocketResource() const {
    return true;
}

/**
 * Return the width of immediate carried by the related bus.
 *
 * @return The width of immediate carried by the related bus.
 */
int
ShortImmPSocketResource::immediateWidth() const {
    return immediateWidth_;
}

/**
 * Return true if the related bus sign extends.
 *
 * @return True if the related bus sign extends.
 */
bool
ShortImmPSocketResource::signExtends() const {
    return signExtends_;
}

/**
 * Return true if the related bus does not sign extend.
 *
 * @return True if the related bus does not sign extend.
 */
bool
ShortImmPSocketResource::zeroExtends() const {
    return !signExtends_;
}

/**
 * Tests if all referred resources in related groups are of
 * proper types.
 *
 * @return true If all resources in related groups are bus resources.
 */
bool
ShortImmPSocketResource::validateRelatedGroups() {
    for (int i = 0; i < relatedResourceGroupCount(); i++) {
        for (int j = 0; j < relatedResourceCount(i); j++) {
            if (!relatedResource(i, j).isBusResource()) {
                return false;
            }
        }
    }
    return true;
}
