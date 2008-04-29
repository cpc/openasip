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

