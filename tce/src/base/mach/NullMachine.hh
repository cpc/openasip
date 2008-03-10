/**
 * @file NullMachine.hh
 *
 * Declaration of NullMachine class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_NULL_MACHINE_HH
#define TTA_NULL_MACHINE_HH

#include "Machine.hh"

namespace TTAMachine {

/**
 * A singleton class which represents a null machine.
 */
class NullMachine : public Machine {
public:
    static NullMachine& instance();

    virtual void addBus(Bus& bus)
        throw (ComponentAlreadyExists);
    virtual void addSocket(Socket& socket)
        throw (ComponentAlreadyExists);
    void addUnit(Unit& unit)
        throw (ComponentAlreadyExists, IllegalParameters);
    virtual void addFunctionUnit(FunctionUnit& unit)
        throw (ComponentAlreadyExists, IllegalParameters);
    virtual void addImmediateUnit(ImmediateUnit& unit)
        throw (ComponentAlreadyExists);
    virtual void addRegisterFile(RegisterFile& unit)
        throw (ComponentAlreadyExists);
    virtual void addAddressSpace(AddressSpace& as)
        throw (ComponentAlreadyExists);
    virtual void addBridge(Bridge& bridge)
        throw (ComponentAlreadyExists);
    virtual void addInstructionTemplate(InstructionTemplate& instrTempl)
        throw (ComponentAlreadyExists);
    virtual void setGlobalControl(ControlUnit& unit)
        throw (ComponentAlreadyExists);
    virtual void unsetGlobalControl();

    virtual ControlUnit* controlUnit() const;

    virtual void removeBus(Bus& bus)
        throw (InstanceNotFound);
    virtual void removeSocket(Socket& socket)
        throw (InstanceNotFound);
    virtual void removeUnit(Unit& unit)
        throw (InstanceNotFound, IllegalParameters);
    virtual void removeFunctionUnit(FunctionUnit& unit)
        throw (InstanceNotFound);
    virtual void removeImmediateUnit(ImmediateUnit& unit)
        throw (InstanceNotFound);
    virtual void removeRegisterFile(RegisterFile& unit)
        throw (InstanceNotFound);
    virtual void deleteBridge(Bridge& bridge)
        throw (InstanceNotFound);
    virtual void deleteInstructionTemplate(InstructionTemplate& instrTempl)
        throw (InstanceNotFound);
    virtual void deleteAddressSpace(AddressSpace& as)
        throw (InstanceNotFound);

    virtual BusNavigator busNavigator() const;
    virtual SocketNavigator socketNavigator() const;
    virtual FunctionUnitNavigator functionUnitNavigator() const;
    virtual AddressSpaceNavigator addressSpaceNavigator() const;
    virtual BridgeNavigator bridgeNavigator() const;
    virtual ImmediateUnitNavigator immediateUnitNavigator() const;
    virtual InstructionTemplateNavigator instructionTemplateNavigator()
        const;
    virtual RegisterFileNavigator registerFileNavigator() const;

    // functions inherited from Serializable interface
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

private:
    NullMachine();
    virtual ~NullMachine();

    /// The only instance of NullMachine.
    static NullMachine machine_;
};
}

#endif
