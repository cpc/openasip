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
 * @file ResourceVectorSet.hh
 *
 * Declaration of ResourceVectorSet class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESOURCE_VECTOR_SET_HH
#define TTA_RESOURCE_VECTOR_SET_HH

#include "FiniteStateAutomaton.hh"
#include "ResourceVector.hh"
#include "FunctionUnit.hh"
#include "Exception.hh"

/**
 * Represents a set of resource vectors used in building the states in case
 * of an function unit FSA.
 */
class ResourceVectorSet {
public:
    ResourceVectorSet(const TTAMachine::FunctionUnit& functionUnit) 
        throw (InvalidData);

    virtual ~ResourceVectorSet();

    const ResourceVector& resourceVector(
        const std::string& operationName) const
        throw (KeyNotFound);

    std::size_t operationIndex(
        const std::string& operationName) const
        throw (KeyNotFound);

    std::size_t resourceVectorCount() const;

    const ResourceVector& resourceVector(std::size_t index) const;

    std::string operationName(std::size_t index) const;

    std::size_t width() const;
    
    bool operator==(const ResourceVectorSet& rightHand) const;

private:
    /// Container for indexing the resource vectors by the operation name.
    typedef std::map<std::string, ResourceVector*> ResourceVectorIndex;
    /// Storage for the resource vectors.
    ResourceVectorIndex vectors_;
    /// Width of the longest resource vector.
    std::size_t width_;
};

#endif
