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
 * @file ResourceVector.cc
 *
 * Definition of ResourceVector class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include "ResourceVector.hh"
#include "Application.hh"
#include "FunctionUnit.hh"
#include "PipelineElement.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "Conversion.hh"
#include "AssocTools.hh"
#include "ExecutionPipeline.hh"

/**
 * Constructor.
 *
 * Builds the resource vector out of MOM ExecutionPipeline.
 *
 * @param pipeline The ExecutionPipeline to build the resource vector for.
 */
ResourceVector::ResourceVector(const TTAMachine::ExecutionPipeline& pipeline) {
    
    resources_.resize(pipeline.latency(), ResourceSet());

    for (int i = 0; i < pipeline.latency(); ++i) {
        TTAMachine::ExecutionPipeline::ResourceSet resourceUsages = 
            pipeline.resourceUsages(i);
        TTAMachine::ExecutionPipeline::ResourceSet::const_iterator res = 
            resourceUsages.begin();
        while (res != resourceUsages.end()) {
            resources_.at(i).insert(std::string("res:") + (*res)->name());
            ++res;
        }        
        TTAMachine::ExecutionPipeline::OperandSet readOperands = 
            pipeline.readOperands(i);
        TTAMachine::ExecutionPipeline::OperandSet writtenOperands = 
            pipeline.writtenOperands(i);
        TTAMachine::ExecutionPipeline::OperandSet::const_iterator op = 
            readOperands.begin();
        while (op != readOperands.end()) {
            const TTAMachine::FUPort* fuPort = 
                pipeline.parentOperation()->port(*op);
                resources_.at(i).insert(
                    std::string("port:") + fuPort->bindingString());
            ++op;
        }

        op = writtenOperands.begin();
        while (op != writtenOperands.end()) {
            const TTAMachine::FUPort* fuPort = 
                pipeline.parentOperation()->port(*op);
                resources_.at(i).insert(
                    std::string("port:") + fuPort->bindingString());
            ++op;
        }
    }
}

/**
 * Destructor.
 */
ResourceVector::~ResourceVector() {
}

/**
 * Returns the set that contains the resources used at the given cycle.
 *
 * @param cycle The cycle.
 * @return The set of resources.
 * @exception OutOfRange If the cycle is out of range.
 */
const ResourceVector::ResourceSet& 
ResourceVector::resourcesUsedAtCycle(unsigned cycle) const
    throw (OutOfRange) {
    if (cycle > resources_.size())
        throw OutOfRange(__FILE__, __LINE__, __func__);
    return resources_.at(cycle);
}

/**
 * Returns a textual description of the resource vector.
 *
 * @return A string describing the vector.
 */
std::string
ResourceVector::toString() const {

    std::string theString = "";
    for (unsigned cycle = 0; cycle < resources_.size(); ++cycle) {
        theString += std::string("[") + Conversion::toString(cycle) + ":{";

        const ResourceSet& resSet = resources_.at(cycle);
        for (ResourceSet::const_iterator i = resSet.begin(); 
             i != resSet.end();) {
            theString += *i;
            ++i;
            if (i != resSet.end())
                theString += ",";
        }
        theString += "}]";
    }
    return theString;
}

/**
 * Returns the width of the resource vector.
 *
 * @return The width.
 */
std::size_t
ResourceVector::width() const {
    return resources_.size();
}

/**
 * Returns true if the given resource vector has conflicting resource usages
 * when issued after given count of cycles.
 *
 * @param other The another resource vector.
 * @param cycle The count of cycles after which the operation is issued.
 * @return True iff there is a conflict.
 */
bool
ResourceVector::conflictsWith(const ResourceVector& other, unsigned cycle) 
    const {

    for (std::size_t c = cycle; c < resources_.size(); ++c) {

        if (other.width() <= c - cycle) 
            return false;

        ResourceSet conflictingResources;
        const ResourceSet& thisResources = resources_.at(c);
        const ResourceSet& otherResources = 
            other.resourcesUsedAtCycle(c - cycle);

        if (thisResources.size() == 0 || otherResources.size() == 0)
            return false;

        // check if any of the resources in another resource set is
        // found in the another
        ResourceSet::const_iterator i = thisResources.begin();
        while (i != thisResources.end()) {
            if (otherResources.find(*i) != otherResources.end())
                return true;
            ++i;
        }
    }
    return false;
}

/**
 * Merges the resource vector with the given resource vector starting from the
 * given cycle.
 *
 * This method is used for producing the composite vector for simulation.
 * The resulting resource vector is "stretched" in case there are no enough 
 * cycle elements in to contain the merged elements.
 *
 * @param other The another resource vector.
 * @param cycle The count of cycles after which the operation is issued.
 */
void
ResourceVector::mergeWith(const ResourceVector& other, unsigned cycle) {

    resources_.resize(std::max(resources_.size(), cycle + other.width()));

    for (std::size_t i = 0; i < other.width(); ++i) {
        const ResourceSet& otherResources = other.resourcesUsedAtCycle(i);
        resources_[cycle + i].insert(
            otherResources.begin(), otherResources.end());
    }
}

/**
 * Shifts the resource vector one step left, i.e., deletes the first column.
 */
void
ResourceVector::shiftLeft() {
    if (resources_.size() > 0)
        resources_.pop_front();
}

/**
 * Clears the resource vector.
 *
 * Result is an empty resource vector with no columns
 */
void
ResourceVector::clear() {
    resources_.clear();
}


/**
 * Compares two ResourceVectors.
 *
 * @param rightHand Right hand operand.
 * @return True is the two vectors match false otherwise.
 */
bool
ResourceVector::operator==(const ResourceVector& rightHand) const {

    if (toString() != rightHand.toString()) {
        return false;
    }
    return true;
}
