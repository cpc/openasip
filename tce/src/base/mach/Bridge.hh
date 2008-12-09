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
 * @file Bridge.hh
 *
 * Declaration of Bridge class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 10 Jun 2004 by vpj, am, tr, ll
 * @note rating: red
 */

#ifndef TTA_BRIDGE_HH
#define TTA_BRIDGE_HH

#include <string>

#include "MachinePart.hh"
#include "Exception.hh"
#include "ObjectState.hh"


namespace TTAMachine {

class Bus;

/**
 * Represents a bridge in the machine.
 */
class Bridge : public Component {
public:
    Bridge(
        const std::string& name,
        Bus& sourceBus,
        Bus& destinationBus)
        throw (IllegalRegistration, ComponentAlreadyExists,
               IllegalConnectivity, IllegalParameters, InvalidName);
    Bridge(const ObjectState* state, Machine& mach)
        throw (ObjectStateLoadingException);
    virtual ~Bridge();

    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    Bus* sourceBus() const;
    Bus* destinationBus() const;
    Bus* previousBus() const;
    Bus* nextBus() const;

    virtual void setMachine(Machine& mach)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for bridge.
    static const std::string OSNAME_BRIDGE;
    /// ObjectState attribute key for source bus name.
    static const std::string OSKEY_SOURCE_BUS;
    /// ObjectState attribute key for destination bus name.
    static const std::string OSKEY_DESTINATION_BUS;

private:
    void adjustChainDirection(
        const Bus& sourceBus,
        const Bus& destinationBus);
    void setSourceAndDestination(Bus& sourceBus, Bus& destination);
    static void setFirstOfChain(const Bus& bus);

    /// Source bus.
    Bus* sourceBus_;
    /// Destination bus.
    Bus* destinationBus_;
    /// Indicates which of the buses is previous and next.
    bool sourcePrevious_;
};
}

#include "Bridge.icc"

#endif
