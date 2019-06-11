/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file ResourceVector.hh
 *
 * Declaration of ResourceVector class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESOURCE_VECTOR_HH
#define TTA_RESOURCE_VECTOR_HH

#include <vector>
#include <set>
#include <deque>
#include <string>

#include "Exception.hh"

namespace TTAMachine {
    class ExecutionPipeline;
}

/**
 * Represents a resource vector used in building the states in case
 * of an function unit FSA.
 */
class ResourceVector {
public:
    /// Resources are stored in a set as strings. Normal pipeline resource
    /// usages are prefixed with "res:", port usages with "port:".
    typedef std::set<std::string> ResourceSet;

    ResourceVector() {};
    ResourceVector(const TTAMachine::ExecutionPipeline& pipeline);
    virtual ~ResourceVector();

    const ResourceSet& resourcesUsedAtCycle(unsigned cycle) const
        throw (OutOfRange);

    std::size_t width() const;

    std::string toString() const;

    virtual bool conflictsWith(const ResourceVector& other, unsigned cycle) 
        const;

    virtual void mergeWith(const ResourceVector& other, unsigned cycle);

    virtual void shiftLeft();

    virtual void clear();

    virtual bool operator==(const ResourceVector& rightHand) const;

private:
    /// Each resource usage is stored as a string.
    typedef std::deque<ResourceSet> ResourceUsageIndex;
    /// The storage for the resource usages.
    ResourceUsageIndex resources_;
};

#endif
