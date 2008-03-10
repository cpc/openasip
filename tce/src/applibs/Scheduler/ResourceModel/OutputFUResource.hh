/**
 * @file OutputFUResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the OutputFUResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_OUTPUTFURESOURCE_HH
#define TTA_OUTPUTFURESOURCE_HH

#include <string>

#include "SchedulingResource.hh"
#include "FUResource.hh"
#include "OutputPSocketResource.hh"

/**
 * An interface for scheduling resources of Resource Model
 * The derived class OutputFUResource
 */

class OutputFUResource : public FUResource {
public:
    virtual ~OutputFUResource();
    OutputFUResource(const std::string& name);

    virtual bool canAssign(const int cycle, const MoveNode& node) const;
    virtual bool canAssign(
        const int cycle,
        const MoveNode& node,
        const OutputPSocketResource& pSocket) const;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual bool isOutputFUResource() const;

protected:
    virtual bool validateDependentGroups();
    virtual bool validateRelatedGroups();
private:
    // Copying forbidden
    OutputFUResource(const OutputFUResource&);
    //Assignemnt forbidden
    OutputFUResource& operator=(const OutputFUResource&);
};

#endif
