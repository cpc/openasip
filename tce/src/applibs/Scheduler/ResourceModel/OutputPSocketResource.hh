/**
 * @file OutputPSocketResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the OutputPSocketResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_OUTPUTPSOCKETRESOURCE_HH
#define TTA_OUTPUTPSOCKETRESOURCE_HH

#include <string>

#include "SchedulingResource.hh"
#include "PSocketResource.hh"
#include "Port.hh"

/**
 * An interface for scheduling resources of Resource Model
 * The derived class OutputPSocketResource
 */

class OutputPSocketResource : public PSocketResource {
public:
    virtual ~OutputPSocketResource();
    OutputPSocketResource(const std::string& name);

    virtual bool isAvailable(const int cycle) const;
    virtual bool isOutputPSocketResource() const;
    virtual void assign(const int cycle, MoveNode& node)
    throw(Exception);
    virtual void unassign(const int cycle, MoveNode& node)
    throw(Exception);
    virtual bool canAssign(const int cycle, const MoveNode& node) const;

protected:
    virtual bool validateDependentGroups();
    virtual bool validateRelatedGroups();

private:
    // Copying forbidden
    OutputPSocketResource(const OutputPSocketResource&);
    // Assignment forbidden
    OutputPSocketResource& operator=(const OutputPSocketResource&);
    // For each cycle stores which Port connected to Socket is read
    std::map<int, const TTAMachine::Port*> storedPorts_;
};

#endif
