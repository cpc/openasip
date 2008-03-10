/**
 * @file OperationIndex.hh
 *
 * Declaration of OperationIndex class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#ifndef TTA_OPERATION_INDEX_HH
#define TTA_OPERATION_INDEX_HH

#include <map>
#include <vector>
#include <string>

#include "OperationSerializer.hh"
#include "Exception.hh"

class OperationModule;
class ObjectState;

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
    std::string path(int i) const throw (OutOfRange);
    int pathCount() const;

    OperationModule& module(int i) throw (OutOfRange);
    int moduleCount() const;
    OperationModule& module(int i, const std::string& path)
        throw (OutOfRange, PathNotFound);
    int moduleCount(const std::string& path) const
        throw (PathNotFound);
    void addModule(OperationModule* module, const std::string& path)
        throw (PathNotFound);
    void removeModule(const std::string& path, const std::string& modName) 
        throw (PathNotFound, InstanceNotFound);
    void refreshModule(const std::string& path, const std::string& modName) 
        throw (PathNotFound, InstanceNotFound);

    OperationModule& moduleOf(const std::string& name);
    std::string operationName(int i, const OperationModule& om)
        throw (OutOfRange, BadOperationModule);
    int operationCount(const OperationModule& om)
        throw (BadOperationModule);
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

    void readOperations(const OperationModule& module)
        throw (SerializerException);
    
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
};

#include "OperationIndex.icc"

#endif
