/**
 * @file OperationBehaviorProxy.hh
 *
 * Declaration of OperationBehaviorProxy class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#ifndef TTA_OPERATION_BEHAVIOR_PROXY_HH
#define TTA_OPERATION_BEHAVIOR_PROXY_HH

#include <vector>
#include <set>

#include "OperationBehavior.hh"
#include "OperationDAGBehavior.hh"

class Operation;
class SimValue;
class OperationContext;
class OperationBehaviorLoader;

/**
 * This class is used to create a behavior model of an operation.
 *
 * A first time operation calls simulateTrigger(), lateResult()... etc.
 * proxy creates the appropriate operation behavior model for the operation.
 * Proxy replaces itself in the operation with the newly created behavior 
 * model. That new model then executes all simulation functions.
 */
class OperationBehaviorProxy : public OperationBehavior {
public:
    OperationBehaviorProxy(
        Operation& targetOperation, 
        OperationBehaviorLoader& loader);
    
    virtual ~OperationBehaviorProxy();

    virtual bool simulateTrigger(
        SimValue** io,
        OperationContext& context) const;
    virtual bool lateResult(
        SimValue** io,
        OperationContext& context) const;

    virtual bool canBeSimulated() const;

    virtual void createState(OperationContext& context) const;
    virtual void deleteState(OperationContext& context) const;

private:
    /// Copying not allowed.
    OperationBehaviorProxy(const OperationBehaviorProxy&);
    /// Assignment not allowed.
    OperationBehaviorProxy& operator=(const OperationBehaviorProxy&);

    void initializeBehavior() const;
   
    /// Operation that owns this proxy;
    Operation* target_;
    /// Used to load behavior model for operation.
    OperationBehaviorLoader* loader_;
    /// Flag indicating whether proxy is initialized or not.
    mutable bool initialized_;

    /// Clean up list for created OperationDAGBehaviors
    mutable std::set<OperationDAGBehavior*> cleanUs_;
};

#endif
