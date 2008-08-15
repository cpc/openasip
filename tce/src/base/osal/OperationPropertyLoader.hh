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
 * @file OperationPropertyLoader.hh
 *
 * Declaration of OperationPropertyLoader.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#ifndef TTA_OPERATION_PROPERTY_LOADER_HH
#define TTA_OPERATION_PROPERTY_LOADER_HH

#include <map>
#include <vector>
#include <string>

#include "OperationSerializer.hh"
#include "Exception.hh"

class Operation;
class ObjectState;
class OperationModule;

/**
 * Imports the static information contents of an operation from a data file.
 */
class OperationPropertyLoader {
public:
    OperationPropertyLoader();
    virtual ~OperationPropertyLoader();

    void loadOperationProperties(
        Operation& operation, 
        const OperationModule& module)
        throw (InstanceNotFound);

private:
    /// Container containing already read ObjectState trees.
    typedef std::map<std::string, std::vector<ObjectState*> > ObjectStateCache;
    
    /// Iterator for map containing already read ObjectState trees.
    typedef ObjectStateCache::iterator MapIter;
    /// value_type for map containing already read ObjectState trees.
    typedef ObjectStateCache::value_type ValueType;

    /// Copying not allowed.
    OperationPropertyLoader(const OperationPropertyLoader&);
    /// Assignment not allowed.
    OperationPropertyLoader& operator=(const OperationPropertyLoader&);

    void loadModule(const OperationModule& module) 
        throw (InstanceNotFound);

    /// Serializer instance.
    OperationSerializer serializer_;
    /// Cache for already read ObjectState trees.
    ObjectStateCache operations_;
};

#endif
