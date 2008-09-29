/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file AssignmentPlan.hh
 *
 * Declaration of AssignmentPlan class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme-no.spam-tut.fi)
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
