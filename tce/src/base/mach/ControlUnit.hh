/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ControlUnit.hh
 *
 * Declaration of ControlUnit class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
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
