/**
 * @file FUResourceConflictDetector.hh
 *
 * Declaration of FUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_RESOURCE_CONFLICT_DETECTOR_HH
#define TTA_FU_RESOURCE_CONFLICT_DETECTOR_HH

#include <map>
#include <string>

#include "Exception.hh"
#include "ClockedState.hh"

class Operation;

/**
 * Interface for classes used for detecting FU resource conflicts.
 */
class FUResourceConflictDetector : public ClockedState {
public:

    /// Type for identifying operations in the conflict detector interface.
    typedef int OperationID;

    FUResourceConflictDetector();
    virtual ~FUResourceConflictDetector();

    virtual bool issueOperation(OperationID id);

    virtual void endClock();
    virtual void advanceClock();
    virtual bool advanceCycle();

    virtual bool isIdle();

    virtual void reset();

    virtual OperationID operationID(const std::string& operationName) const
        throw (KeyNotFound);

protected:
    /// Type for mapping Operations to OperationIDs.
    typedef std::map<std::string, OperationID> OperationIDIndex;
    /// Map for finding indices for operations quickly.
    OperationIDIndex operationIndices_;

};

#endif
