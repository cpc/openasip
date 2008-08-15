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
 * @file OperationPoolPimpl.hh
 *
 * Declaration of OperationPoolPimpl (private implementation) class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef OPERATIONPOOL_PIMPL_HH
#define OPERATIONPOOL_PIMPL_HH

#include <string>
#include <map>

class OperationPool;
class OperationBehaviorLoader;
class OperationBehaviorProxy;
class OperationIndex;
class Operation;

/**
 * A private implementation class for OperationPool
 */
class OperationPoolPimpl {
public:
    friend class OperationPool;
    ~OperationPoolPimpl();
    
    Operation& operation(const char* name);
    OperationIndex& index();

    static void cleanupCache();

private:
    OperationPoolPimpl();
    
    /// Container for operations indexed by their names.
    typedef std::map<std::string, Operation*> OperationTable;

    /// Copying not allowed.
    OperationPoolPimpl(const OperationPoolPimpl&);
    /// Assignment not allowed.
    OperationPoolPimpl& operator=(const OperationPoolPimpl&);

    /// Operation pool uses this to load behavior models of the operations.
    static OperationBehaviorLoader* loader_;
    /// Indexed table used to find out which operation module contains the
    /// given operation.
    static OperationIndex* index_;
    
    OperationSerializer serializer_;
    
    /// Contains all operations that have been already requested by the client.
    static OperationTable operationCache_;
    /// Contains all operation behavior proxies.
    static std::vector<OperationBehaviorProxy*> proxies_; 
};

#endif

