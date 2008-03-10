/**
 * @file SegmentResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the SegmentResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SEGMENTRESOURCE_HH
#define TTA_SEGMENTRESOURCE_HH

#include <string>
#include <map>

#include "SchedulingResource.hh"

/**
 * An interface for scheduling resources of Resource Model
 * The derived class SegmentResource
 */

class SegmentResource : public SchedulingResource {
public:
    virtual ~SegmentResource();
    SegmentResource(const std::string& name);

    virtual bool isInUse(const int cycle) const;
    virtual bool isAvailable(const int cycle) const;
    virtual bool canAssign(const int cycle, const MoveNode& node) const;
    virtual bool canAssign(
        const int cycle,
        const MoveNode& node,
        const SchedulingResource& pSocket) const;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual bool isSegmentResource() const;

protected:
    virtual bool validateDependentGroups();
    virtual bool validateRelatedGroups();

private:
    // map contains <Cycle : testCounter>
    typedef std::map<int, int> ResourceRecordType;
    // Copying forbidden
    SegmentResource(const SegmentResource&);
    // assignment forbidden
    SegmentResource& operator=(const SegmentResource&);

    ResourceRecordType resourceRecord_;
};

#endif
