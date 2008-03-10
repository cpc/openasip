/**
 * @file MoveNodeGroup.hh
 * 
 * Declaration of MoveNodeGroup class 
 * 
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MOVE_NODE_GROUP_HH
#define TTA_MOVE_NODE_GROUP_HH

#include <vector>

#include "MoveNode.hh"
#include "Exception.hh"

class DataDependenceGraph;

/**
 * Class which contains group of moves which are to be scheduled.
 */
class MoveNodeGroup {
public:
    MoveNodeGroup();
    MoveNodeGroup(const DataDependenceGraph& ddg);

    virtual ~MoveNodeGroup();

    void addNode(MoveNode& node);
    
    int earliestCycle() const;
    int latestCycle() const throw (NotAvailable);

    int maxSinkDistance() const;
    int maxSourceDistance() const;

    int nodeCount() const;
    MoveNode& node(int index) const;

    bool isScheduled() const;
    std::string toString() const;

private:
    std::vector<MoveNode*> nodes_;
    /// The data dependence graph the moves in this group belong to 
    /// (optional).
    const DataDependenceGraph* ddg_;
};

#endif
