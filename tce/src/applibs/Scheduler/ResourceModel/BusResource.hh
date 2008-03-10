/**
 * @file BusResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the BusResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BUSRESOURCE_HH
#define TTA_BUSRESOURCE_HH

#include<string>

#include "SchedulingResource.hh"

/**
 * An interface for scheduling resources of Resource Model
 * The derived class BusResource
 */

class BusResource : public SchedulingResource {
public:
    BusResource(const std::string& name, int width);
    virtual ~BusResource();

    virtual bool isInUse(const int cycle) const;
    virtual bool isAvailable(const int cycle) const;
    virtual bool isInUse(
        const int cycle,
        const SchedulingResource& inputPSocket,
        const SchedulingResource& outputPSocket) const;
    virtual bool isAvailable(
        const int cycle,
        const SchedulingResource& inputPSocket,
        const SchedulingResource& outputPSocket) const;
    virtual bool canAssign(const int cycle, const MoveNode& node) const;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual bool canAssign(
        const int cycle,
        const MoveNode& node,
        const SchedulingResource& inputPSocket,
        const SchedulingResource& outputPSocket) const;
    virtual void assign(
        const int cycle,
        MoveNode& node,
        const SchedulingResource& inputPSocket,
        const SchedulingResource& outputPSocket)
        throw (Exception);
    virtual void unassign(
        const int cycle,
        MoveNode& node,
        const SchedulingResource& inputPSocket,
        const SchedulingResource& outputPSocket)
        throw (Exception);
    virtual bool isBusResource() const;

protected:
    virtual bool validateDependentGroups();
    virtual bool validateRelatedGroups();

private:
    // Copying forbidden
    BusResource(const BusResource&);
    // assignment forbidden
    BusResource& operator=(const BusResource&);
    // Bus width in bits
    int busWidth_;

};

#endif
