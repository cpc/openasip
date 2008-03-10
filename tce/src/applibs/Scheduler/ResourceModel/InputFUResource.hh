/**
 * @file InputFUResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the InputFUResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_INPUTFURESOURCE_HH
#define TTA_INPUTFURESOURCE_HH

#include <string>

#include "SchedulingResource.hh"
#include "FUResource.hh"
#include "InputPSocketResource.hh"

/**
 * An interface for scheduling resources of Resource Model.
 * The derived class InputFUResource.
 */

class InputFUResource : public FUResource {
public:
    virtual ~InputFUResource();
    InputFUResource(const std::string& name);

    virtual bool canAssign(const int cycle, const MoveNode& node) const;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual bool isAvailable(const int cycle) const;
    virtual bool isInUse(const int cycle) const;
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual bool canAssign(
        const int cycle,
        const MoveNode& node,
        const InputPSocketResource& pSocket,
        const bool triggers = false) const
        throw (Exception);
    virtual bool isInputFUResource() const;

protected:
    virtual bool validateDependentGroups();
    virtual bool validateRelatedGroups();

private:
    // Copying forbidden
    InputFUResource(const InputFUResource&);
    // Assignment forbidden
    InputFUResource& operator=(const InputFUResource&);
};

#endif
