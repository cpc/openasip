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
 * @file ResourceVectorSet.hh
 *
 * Declaration of ResourceVectorSet class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
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
