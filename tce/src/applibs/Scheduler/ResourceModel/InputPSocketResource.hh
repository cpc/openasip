/**
 * @file InputPSocketResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the InputPSocketResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_INPUTPSOCKETRESOURCE_HH
#define TTA_INPUTPSOCKETRESOURCE_HH

#include<string>

#include "SchedulingResource.hh"
#include "PSocketResource.hh"

/**
 * An interface for scheduling resources of Resource Model
 * The derived class InputPSocketResource
 */

class InputPSocketResource : public PSocketResource {
public:
    virtual ~InputPSocketResource();
    InputPSocketResource(const std::string& name);

    virtual bool isInputPSocketResource() const;

protected:
    virtual bool validateDependentGroups() ;
    virtual bool validateRelatedGroups() ;

private:
    // Copying forbidden
    InputPSocketResource(const InputPSocketResource&);
    // Assignment forbidden
    InputPSocketResource& operator=(const InputPSocketResource&);
};

#endif
