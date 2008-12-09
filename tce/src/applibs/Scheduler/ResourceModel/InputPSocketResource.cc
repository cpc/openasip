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
 * @file InputPSocketResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the InputPSocketResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
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
