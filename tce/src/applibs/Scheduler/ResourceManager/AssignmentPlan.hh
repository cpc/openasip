/**
 * @file AssignmentPlan.hh
 *
 * Declaration of AssignmentPlan class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ASSIGNMENT_PLAN_HH
#define TTA_ASSIGNMENT_PLAN_HH

#include <vector>
#include "AssignmentPlan.hh"
#include "Exception.hh"

class ResourceBroker;
class PendingAssignment;
class MoveNode;

/**
 * Keeps the order in which single resource-specific brokers are
 * invoked to perform their bit of the allocation or assignment. Also
 * records the current state of tentative and potential assignments
 * for each type of resource.
 */
class AssignmentPlan {
public:
    AssignmentPlan();
    virtual ~AssignmentPlan();

    void insertBroker(ResourceBroker& broker);
    void setRequest(int cycle, MoveNode& node);
    ResourceBroker& firstBroker();
    ResourceBroker& nextBroker(ResourceBroker& pos);
    ResourceBroker& lastBroker();
    ResourceBroker& currentBroker();
    void advance();
    void backtrack();
    void tryNextAssignment();
    bool isTestedAssignmentPossible();
    void resetAssignments();
    void resetAssignments(MoveNode& node) throw (InvalidData);

    int brokerCount() const;
    ResourceBroker& broker(int index) const throw (OutOfRange);

private:
    /// Sequence of pending assignments.
    std::vector<PendingAssignment*> assignments_;
    /// Sequence of applicable pending assignments.
    std::vector<PendingAssignment*> applicableAssignments_;
    /// Sequence of resource brokers.
    std::vector<ResourceBroker*> brokers_;
    /// Move of current resource assignment request.
    MoveNode* node_;
    /// Cycle in which current node should be placed.
    int cycle_;
    /// Current broker.
    int currentBroker_;
    /// True if a valid resource of current broker has been assigned.
    bool resourceFound_;
};

#endif
