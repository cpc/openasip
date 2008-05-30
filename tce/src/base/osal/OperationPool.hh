/**
 * @file OperationPool.hh
 *
 * Declaration of OperationPool class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: yellow
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#ifndef TTA_OPERATION_POOL_HH
#define TTA_OPERATION_POOL_HH

class OperationBehaviorLoader;
class OperationBehaviorProxy;
class OperationIndex;
class Operation;
class OperationPoolPimpl;

/**
 * OperationPool provides interface for obtaining operations of the target
 * architecture template.
 */
class OperationPool {
public:
    OperationPool();
    virtual ~OperationPool();

    Operation& operation(const char* name);
    OperationIndex& index();

    static void cleanupCache();
  
private:
    /// Copying not allowed.
    OperationPool(const OperationPool&);
    /// Assignment not allowed.
    OperationPool& operator=(const OperationPool&);
    
    /// Private implementation in a separate source file.
    OperationPoolPimpl* pimpl_;
};

#endif
