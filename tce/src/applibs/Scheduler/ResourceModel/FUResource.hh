/**
 * @file FUResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the abstract FUResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FURESOURCE_HH
#define TTA_FURESOURCE_HH

#include<string>

#include "SchedulingResource.hh"

/**
 * An interface for scheduling FU resources of Resource Model
 *
 */

class FUResource : public SchedulingResource {
public:
    FUResource(const std::string& name);
    virtual ~FUResource();

    virtual bool isInUse(const int cycle) const;
    virtual bool isAvailable(const int cycle) const;
    virtual bool canAssign(const int cycle, const MoveNode& node) const;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);

private:
    // Copying forbidden
    FUResource(const FUResource&);
    // Assignment forbidden
    FUResource& operator=(const FUResource&);
};

#endif
