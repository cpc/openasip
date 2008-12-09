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
 * @file ResourceVectorSet.cc
 *
 * Definition of ResourceVectorSet class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include <algorithm>

#include "ResourceVectorSet.hh"
#include "Application.hh"
#include "ExecutionPipeline.hh"
#include "PipelineElement.hh"
#include "HWOperation.hh"
#include "StringTools.hh"
#include "AssocTools.hh"

/**
 * Builds the resource vectors for the given FU.
 *
 * Resource vectors are built for all operations in the FU. 
 *
 * @param functionUnit The function unit to build the resource vectors for.
 * @exception InvalidData In case the function unit is incomplete to build
 * resource vectors for (e.g., missing operand-port bindings).
 */
ResourceVectorSet::ResourceVectorSet(
    const TTAMachine::FunctionUnit& functionUnit) 
    throw (InvalidData) : width_(0) {

    try {
        // add the port usages of each operation and resource usages
        for (int i = 0; i < functionUnit.operationCount(); ++i) {
            const TTAMachine::HWOperation* op = functionUnit.operation(i);
            ResourceVector* rv = 
                new ResourceVector(*op->pipeline());
            vectors_[StringTools::stringToUpper(op->name())] = rv;
            width_ = std::max(width_, rv->width());
        }
    } catch (const Exception& e) {
        InvalidData io(
            __FILE__, __LINE__, __func__, 
            "Error building the resource vectors");
        io.setCause(e);
        throw io;
    }
}

/**
 * Destructor.
 */
ResourceVectorSet::~ResourceVectorSet() {
    AssocTools::deleteAllValues(vectors_);
}

/**
 * Returns the resource vector associated with the given operation.
 *
 * @param operationName The operation name.
 * @return The resource vector.
 * @exception KeyNotFound If the operation is not found.
 */
const ResourceVector& 
ResourceVectorSet::resourceVector(const std::string& operationName) const
    throw (KeyNotFound) {

    const std::string opName = StringTools::stringToUpper(operationName);
    if (!AssocTools::containsKey(vectors_, opName)) {
        std::string message = "No resource vector found for operation " +
            operationName + ".";
        throw KeyNotFound(
            __FILE__, __LINE__, __func__, message);
    }
    return *(*vectors_.find(opName)).second;
}

/**
 * Returns the count of resource vectors in the set.
 *
 * @return The count.
 */
std::size_t
ResourceVectorSet::resourceVectorCount() const {
    return vectors_.size();
}

/**
 * Returns the resource vector at the given index.
 *
 * @param index The index.
 * @return The resource vector.
 */
const ResourceVector& 
ResourceVectorSet::resourceVector(std::size_t index) const {

    int counter = index;
    for (ResourceVectorIndex::const_iterator i = vectors_.begin(); 
         i != vectors_.end(); ++i) {
        if (counter == 0)
            return *((*i).second);
        --counter;
    }
    // should never get here
    return *((*vectors_.end()).second);
}

/**
 * Returns the name of the operation of the resource vector at the given index.
 *
 * @param index The index.
 * @return The operation name.
 */
std::string 
ResourceVectorSet::operationName(std::size_t index) const {

    int counter = index;
    for (ResourceVectorIndex::const_iterator i = vectors_.begin(); 
         i != vectors_.end(); ++i) {
        if (counter == 0)
            return (*i).first;
        --counter;
    }
    // should never get here
    return (*vectors_.end()).first;
}

/**
 * Returns the index of the operation with the given name.
 *
 * @param operationName The name of the operation.
 * @return Index of the operation.
 * @exception KeyNotFound If the operation is not found.
 */
std::size_t
ResourceVectorSet::operationIndex(const std::string& operationName) const
    throw (KeyNotFound) {

    int counter = 0;
    for (ResourceVectorIndex::const_iterator i = vectors_.begin(); 
         i != vectors_.end(); ++i) {
        if ((*i).first == operationName)
            return counter;
        ++counter;
    }
    throw KeyNotFound(__FILE__, __LINE__, __func__, "Operation not found.");
}


/**
 * Returns the width of the longest resource vector in the resource vector set.
 *
 * @return The width.
 */
std::size_t
ResourceVectorSet::width() const {
    return width_;
}


/**
 * Compares two ResourceVectorSets.
 *
 * @param rightHand Right hand operand.
 * @return True is the two sets match false otherwise.
 */
bool
ResourceVectorSet::operator==(const ResourceVectorSet& rightHand) const {

    if (resourceVectorCount() != rightHand.resourceVectorCount()) {
        return false;
    }
    ResourceVectorIndex::const_iterator iter = vectors_.begin();
    for (; iter != vectors_.end(); iter++) {
        try {
            if (!((*(*iter).second) == 
                  rightHand.resourceVector((*iter).first))) {
                return false;
            }
        } catch (KeyNotFound& e) {
            return false;
        }
    }
    return true;
}
