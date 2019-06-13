/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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

class ObjectState;

namespace TTAMachine {

class SpecialRegisterPort;

/**
 * Represents the global control unit in the machine.
 */
class ControlUnit : public FunctionUnit {
public:
    ControlUnit(
        const std::string& name, int delaySlots, int globalGuardLatency);
    ControlUnit(const ObjectState* state);
    virtual ~ControlUnit();

    virtual ControlUnit* copy() const;

    virtual void setMachine(Machine& mach);
    virtual void unsetMachine();

    int delaySlots() const;
    void setDelaySlots(int delaySlots);

    int globalGuardLatency() const;
    void setGlobalGuardLatency(int latency);

    int specialRegisterPortCount() const;
    bool hasSpecialRegisterPort(const std::string& name) const;
    SpecialRegisterPort* specialRegisterPort(int index) const;
    SpecialRegisterPort* specialRegisterPort(const std::string& name) const;

    void setReturnAddressPort(const SpecialRegisterPort& port);
    void unsetReturnAddressPort();
    bool hasReturnAddressPort() const;
    SpecialRegisterPort* returnAddressPort() const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state);

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
    void loadStateWithoutReferences(const ObjectState* state);
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
