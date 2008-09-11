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
