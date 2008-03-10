/**
 * @file PSocketResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the abstract PSocketResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PSOCKETRESOURCE_HH
#define TTA_PSOCKETRESOURCE_HH

#include <string>
#include <map>

#include "SchedulingResource.hh"
/**
 * An interface for scheduling resources of Resource Model.
 *
 * The derived class PSocketResource represents a combined resource of
 * a FU/RF port and a socket that connects it to a bus.
 */

class PSocketResource : public SchedulingResource {
public:
    virtual ~PSocketResource();
    PSocketResource(const std::string& name);

    virtual bool isInUse(const int cycle) const;
    virtual bool isAvailable(const int cycle) const;
    virtual bool canAssign(const int cycle, const MoveNode& node) const;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);

protected:

private:
    // Recording PSocket usage - cycle = value map
    typedef std::map<int, int> ResourceRecordType;
    // Copying forbidden
    PSocketResource(const PSocketResource&);
    // Assignment forbidden
    PSocketResource& operator=(const PSocketResource&);

    ResourceRecordType resourceRecord_;
};

#endif
