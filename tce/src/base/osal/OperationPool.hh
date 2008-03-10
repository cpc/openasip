/**
 * @file OperationPool.hh
 *
 * Declaration of OperationPool class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#ifndef TTA_OPERATION_POOL_HH
#define TTA_OPERATION_POOL_HH

#include <string>
#include <map>

#include "OperationSerializer.hh"
#include "OperationBehaviorLoader.hh"
#include "OperationIndex.hh"

class OperationBehaviorProxy;
class Operation;

/**
 * OperationPool provides interface for obtaining operations of the target
 * architecture template.
 */
class OperationPool {
public:
    OperationPool();
    virtual ~OperationPool();

    Operation& operation(const std::string& name);
    OperationIndex& index();

    static void cleanupCache();
  
private:
    /// Container for operations indexed by their names.
    typedef std::map<std::string, Operation*> OperationTable;

    /// Copying not allowed.
    OperationPool(const OperationPool&);
    /// Assignment not allowed.
    OperationPool& operator=(const OperationPool&);

    /// Operation pool uses this to load behavior models of the operations.
    static OperationBehaviorLoader* loader_;
    /// Indexed table used to find out which operation module contains the
    /// given operation.
    static OperationIndex* index_;
    /// Used to read static object properties from an XML file.
    OperationSerializer serializer_;
    /// Contains all operations that have been already requested by the client.
    static OperationTable operationCache_;
    /// Contains all operation behavior proxies.
    static std::vector<OperationBehaviorProxy*> proxies_;
};

#include "OperationPool.icc"

#endif
