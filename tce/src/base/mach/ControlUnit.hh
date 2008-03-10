/**
 * @file ControlUnit.hh
 *
 * Declaration of ControlUnit class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONTROL_UNIT_HH
#define TTA_CONTROL_UNIT_HH

#include <string>

#include "FunctionUnit.hh"
#include "AddressSpace.hh"
#include "ObjectState.hh"

namespace TTAMachine {

class SpecialRegisterPort;

/**
 * Represents the global control unit in the machine.
 */
class ControlUnit : public FunctionUnit {
public:
    ControlUnit(
        const std::string& name,
        int delaySlots,
        int globalGuardLatency)
        throw (OutOfRange, InvalidName);
    ControlUnit(const ObjectState* state)
        throw (ObjectStateLoadingException);
    virtual ~ControlUnit();

    virtual ControlUnit* copy() const;

    virtual void setMachine(Machine& mach)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();

    int delaySlots() const;
    void setDelaySlots(int delaySlots)
        throw (OutOfRange);

    int globalGuardLatency() const;
    void setGlobalGuardLatency(int latency)
        throw (OutOfRange);

    int specialRegisterPortCount() const;
    bool hasSpecialRegisterPort(const std::string& name) const;
    SpecialRegisterPort* specialRegisterPort(int index) const
        throw (OutOfRange);
    SpecialRegisterPort* specialRegisterPort(const std::string& name) const
        throw (InstanceNotFound);

    void setReturnAddressPort(const SpecialRegisterPort& port)
        throw (IllegalRegistration);
    void unsetReturnAddressPort();
    bool hasReturnAddressPort() const;
    SpecialRegisterPort* returnAddressPort() const
        throw (InstanceNotFound);

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for ControlUnit.
    static const std::string OSNAME_CONTROL_UNIT;
    /// ObjectState attribute key for the number of delay slots.
    static const std::string OSKEY_DELAY_SLOTS;
    /// ObjectState attribute key for the global guard latency.
    static const std::string OSKEY_GUARD_LATENCY;
    /// ObjectState attribute key for the name of the return address port.
    static const std::string OSKEY_RA_PORT;

protected:
    virtual void removePort(Port& port);

private:
    void loadStateWithoutReferences(const ObjectState* state)
        throw (ObjectStateLoadingException);
    static bool hasLocalGuardLatencyOfZero(const Machine& machine);

    /// Number of delay instruction slots on the transport pipeline.
    int delaySlots_;
    
    /// The global guard latency.
    int globalGuardLatency_;
    /// The return address port.
    SpecialRegisterPort* raPort_;
};
}

#include "ControlUnit.icc"

#endif
