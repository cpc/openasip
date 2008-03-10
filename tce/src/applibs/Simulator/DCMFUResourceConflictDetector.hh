/**
 * @file DCMFUResourceConflictDetector.hh
 *
 * Declaration of DCMFUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DCM_FU_RESOURCE_CONFLICT_DETECTOR_HH
#define TTA_DCM_FU_RESOURCE_CONFLICT_DETECTOR_HH

#include <map>
#include <string>

#include "Exception.hh"
#include "FunctionUnit.hh"
#include "FUResourceConflictDetector.hh"
#include "FUCollisionMatrixIndex.hh"
#include "CollisionMatrix.hh"

class Operation;

namespace TTAMachine {
    class FunctionUnit;
}

/**
 * A Dynamic Collision Matrix (DCM) implementation of an FU resource conflict 
 * detector.
 */
class DCMFUResourceConflictDetector : 
    public FUResourceConflictDetector {
public:

    DCMFUResourceConflictDetector(
        const TTAMachine::FunctionUnit& fu)
        throw (InvalidData);
    virtual ~DCMFUResourceConflictDetector();

    virtual bool issueOperation(OperationID id);
    virtual bool advanceCycle();
    virtual void reset();

    bool issueOperationInline(OperationID id);
    bool advanceCycleInline();

    virtual OperationID operationID(const std::string& operationName) const
        throw (KeyNotFound);

private:
    /// The reservation tables of operations.
    FUCollisionMatrixIndex collisionMatrices_;
    /// The global collision matrix.
    CollisionMatrix globalCollisionMatrix_;
    /// The modeled FU.
    const TTAMachine::FunctionUnit& fu_;
};

#include "DCMFUResourceConflictDetector.icc"

#endif
