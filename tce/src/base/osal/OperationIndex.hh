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
 * @file OperationIndex.hh
 *
 * Declaration of OperationIndex class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#ifndef TTA_OPERATION_INDEX_HH
#define TTA_OPERATION_INDEX_HH

#include <map>
#include <vector>
#include <string>
#include <set>

#include "OperationSerializer.hh"
#include "OperationBehaviorLoader.hh"
#include "Exception.hh"

class OperationModule;
class ObjectState;
class Operation;
class TCEString;
class OperationBehaviorProxy;

/**
 * Class that holds information about search paths, modules and operations.
 *
 * An indexed table of all operations available organized by modules and
 * indexed by names.
 */
class OperationIndex {
public:

    static const std::string PROPERTY_FILE_EXTENSION;

    OperationIndex();
    virtual ~OperationIndex();

    void addPath(const std::string& path);
    std::string path(int i) const;
    int pathCount() const;

    OperationModule& module(int i);
    int moduleCount() const;
    OperationModule& module(int i, const std::string& path);
    int moduleCount(const std::string& path) const;
    void addModule(OperationModule* module, const std::string& path);
    void removeModule(const std::string& path, const std::string& modName);
    void refreshModule(const std::string& path, const std::string& modName);

    OperationModule& moduleOf(const std::string& name);
    std::string operationName(int i, const OperationModule& om);
    int operationCount(const OperationModule& om);

    Operation* effectiveOperation(const TCEString& name);

private:
    /// Contains all operation modules indexed by full path names.
    typedef std::map<std::string, std::vector<OperationModule*> >
        ModuleTable;
    /// Contains all object state trees of modules indexed by operation 
    ///definition module names.
    typedef std::map<std::string, ObjectState*> DefinitionTable;
   
    /// Copying not allowed.
    OperationIndex(const OperationIndex&);
    /// Assignment not allowed.
    OperationIndex& operator=(const OperationIndex&);

    void readOperations(const OperationModule& module);

    OperationModule& moduleOf(
        const std::string& path, 
        const std::string& operName);

    /// List of paths searched for the operation modules.
    std::vector<std::string> paths_;
    /// Contains all operation modules found in a search path organized by path
    /// names.
    ModuleTable modulesInPath_;
    /// Contains all operation definitions defined in available operation
    /// modules indexed by module names.
    DefinitionTable opDefinitions_;
    /// Container holding all modules.
    std::vector<OperationModule*> modules_;
    /// Reads the operation property definitions.
    OperationSerializer serializer_;
    OperationBehaviorLoader loader_;
    std::vector<OperationBehaviorProxy*> proxies_;
    std::set<const OperationModule*> brokenModules_;
};

#include "OperationIndex.icc"

#endif
