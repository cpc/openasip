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
        throw (DynamicLibraryException, InstanceNotFound, FileNotFound,
               SymbolNotFound);

    void freeBehavior();

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
