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
 * @file ResourceVector.hh
 *
 * Declaration of ResourceVector class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESOURCE_VECTOR_HH
#define TTA_RESOURCE_VECTOR_HH

#include <vector>
#include <set>
#include <deque>
#include "ExecutionPipeline.hh"

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
