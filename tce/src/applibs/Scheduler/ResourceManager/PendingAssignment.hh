/**
 * @file PendingAssignment.hh
 *
 * Declaration of PendingAssignment class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PENDING_ASSIGNMENT_HH
#define TTA_PENDING_ASSIGNMENT_HH

#include "SchedulingResource.hh"

class ResourceBroker;
class MoveNode;

/**
 */
class PendingAssignment {
public:
    PendingAssignment(ResourceBroker& broker);
    virtual ~PendingAssignment();

    ResourceBroker& broker();
	void setRequest(int cycle, MoveNode& node);
    bool isAssignmentPossible();
    void tryNext() throw (ModuleRunTimeError);
    void undoAssignment();
    void forget();

private:
    /// Corresponding resource broker.
    ResourceBroker& broker_;
	int cycle_;
	MoveNode* node_;
    bool assignmentTried_;
    SchedulingResourceSet availableResources_;
    int lastTriedAssignment_;
};

#endif
