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
