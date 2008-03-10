/**
 * @file SegmentBroker.hh
 *
 * Declaration of SegmentBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SEGMENT_BROKER_HH
#define TTA_SEGMENT_BROKER_HH

#include "ResourceBroker.hh"

namespace TTAMachine {
    class Machine;
    class MachinePart;
}

class MoveNode;
class ResourceMapper;
class SchedulingResource;

/**
 * Segment broker.
 */
class SegmentBroker : public ResourceBroker {
public:
    SegmentBroker(std::string);
    virtual ~SegmentBroker();

    virtual void assign(int cycle, MoveNode& node, SchedulingResource& res)
        throw (Exception);
    virtual void unassign(MoveNode& node);

    virtual int earliestCycle(int cycle, const MoveNode& node) const;
    virtual int latestCycle(int cycle, const MoveNode& node) const;
    virtual bool isAlreadyAssigned(int cycle, const MoveNode& node) const;
    virtual bool isApplicable(const MoveNode& node) const;
    virtual void buildResources(const TTAMachine::Machine& target);
    virtual void setupResourceLinks(const ResourceMapper& mapper);
};

#endif
