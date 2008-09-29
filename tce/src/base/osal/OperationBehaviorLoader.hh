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
 * @file OperationBehaviorLoader.hh
 *
 * Declaration of OperationBehaviorLoader class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#ifndef TTA_OPERATION_BEHAVIOR_LOADER_HH
#define TTA_OPERATION_BEHAVIOR_LOADER_HH

#include <string>
#include <map>

#include "PluginTools.hh"

class OperationIndex;
class OperationBehavior;
class Operation;

/**
 * The purpose of this class is to import operation behavior models
 * from dynamic modules.
 *
 * PluginTools is used to import the operation behavior modules.
 * OperationIndex is used to obtain modules in which certain operation
 * belongs to.
 */
class OperationBehaviorLoader {
public:
    explicit OperationBehaviorLoader(OperationIndex& index);
    virtual ~OperationBehaviorLoader();

    OperationBehavior& importBehavior(const Operation& parent)
        throw (DynamicLibraryException, InstanceNotFound);

private:
    /// Contains operation behavior models indexed by operation names.
    typedef std::map<std::string, OperationBehavior*> BehaviorMap;
    /// Contains all deletion function of operation behavior models.
    typedef std::map<OperationBehavior*, void (*)(OperationBehavior*)>
        DestructionMap;
    
    /// Copying not allowed.
    OperationBehaviorLoader(const OperationBehaviorLoader&);
    /// Assignment not allowed.
    OperationBehaviorLoader& operator=(const OperationBehaviorLoader&);

    void freeBehavior();

    /// The name of the creation function in dynamic module.
    static const std::string CREATE_FUNC;
    /// The name of the deletion function in dynamic module.
    static const std::string DELETE_FUNC;

    /// Indexed table of all modules and operations accessible for this loader.
    OperationIndex& index_;
    /// PluginTools for loading dynamic modules.
    PluginTools tools_;
    /// Container of all loaded operation behavior models.
    BehaviorMap behaviors_;
    /// Container of all destruction functions of behavioral models.
    DestructionMap destructors_;
};

#endif
