/**
 * @file ImmediateSlot.hh
 *
 * Declaration of ImmediateSlot class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMMEDIATE_SLOT_HH
#define TTA_IMMEDIATE_SLOT_HH

#include "MachinePart.hh"

namespace TTAMachine {

/**
 * Immediate slot is a machine component and represents a dedicated long
 * immediate field in the TTA instruction.
 */
class ImmediateSlot : public Component {
public:
    ImmediateSlot(const std::string& name, Machine& parent)
        throw (InvalidName, ComponentAlreadyExists);
    ImmediateSlot(const ObjectState* state, Machine& parent)
        throw (ObjectStateLoadingException, ComponentAlreadyExists);
    virtual ~ImmediateSlot();

    int width() const;

    virtual void setMachine(Machine& machine)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for ImmediateSlot.
    static const std::string OSNAME_IMMEDIATE_SLOT;
};
}

#endif
