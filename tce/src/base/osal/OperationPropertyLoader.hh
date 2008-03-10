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
