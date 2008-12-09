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
 * @file ShortImmPSocketResource.cc
 *
 * Implementation of ShortImmPSocketResource class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme-no.spam-tut.fi)
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
