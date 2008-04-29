/**
 * @file CycleLookBackSoftwareBypasser.hh
 *
 * Declaration of CycleLookBackSoftwareBypasser interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CYCLE_LOOK_BACK_SOFTWARE_BYPASSER_HH
#define TTA_CYCLE_LOOK_BACK_SOFTWARE_BYPASSER_HH

#include <map>
#include "SoftwareBypasser.hh"

class MoveNodeSelector;
class MoveNode;
/**
 * A simple implementation of software bypassing that reschedules operand
 * writes as bypassed moves in case the result has been produced in n
 * previous cycles.
 */
class CycleLookBackSoftwareBypasser : public SoftwareBypasser {
public:
    CycleLookBackSoftwareBypasser(int cyclesToLookBack=1,
                                  bool killDeadResult=true);
    virtual ~CycleLookBackSoftwareBypasser();

    virtual int bypass(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm);

    virtual void removeBypass(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm);
    virtual int removeDeadResults(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm);

    void setSelector(MoveNodeSelector* selector);
    
private:
    /// count of cycles before the operand write to look for the producer
    /// of the read value
    int cyclesToLookBack_;

    // whether dead resutls should be killed.
    bool killDeadResults_;

    /// Stores sources and bypassed moves in case they
    /// have to be unassigned (case when operands are scheduled
    /// and bypassed but result can not be scheduled with such operands
    // First is bypassed node, second is original source
    std::map<MoveNode*, MoveNode*> storedSources_;
    MoveNodeSelector* selector_;
};

#endif
