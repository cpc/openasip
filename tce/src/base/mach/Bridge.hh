/**
 * @file Bridge.hh
 *
 * Declaration of Bridge class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
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
