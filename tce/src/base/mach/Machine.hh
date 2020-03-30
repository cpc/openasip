/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file Machine.hh
 *
 * Declaration of Machine class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 16 Jun 2004 by ml, tr, jm, ll
 * @note rating: red
 */

#ifndef TTA_MACHINE_HH
#define TTA_MACHINE_HH

#include <vector>
#include <string>

#include "Exception.hh"
#include "Serializable.hh"
#include "TCEString.hh"

class MachineTester;
class DummyMachineTester;

namespace TTAMachine {

class Bus;
class Socket;
class AddressSpace;
class Unit;
class Bridge;
class InstructionTemplate;
class FunctionUnit;
class ImmediateUnit;
class RegisterFile;
class Machine;
class ControlUnit;
class Component;
class ImmediateSlot;

template <typename ComponentType>
class ComponentContainer;

template <typename ComponentType>
class Navigator;

/**
 * Represents a complete TTA processor.
 */
class Machine : public Serializable {
public:

    /**
     * Extension mode applied to a number when it is narrower than the place
     * where it is placed e.g. bus.
     */
    enum Extension {
        ZERO, ///< Zero extension.
        SIGN  ///< Sign extension.
    };

    Machine();
    Machine(const Machine& old);
    virtual ~Machine();

    virtual bool isUniversalMachine() const;

    virtual void addBus(Bus& bus);
    virtual void addSocket(Socket& socket);
    void addUnit(Unit& unit);
    virtual void addFunctionUnit(FunctionUnit& unit);
    virtual void addImmediateUnit(ImmediateUnit& unit);
    virtual void addRegisterFile(RegisterFile& unit);
    virtual void addAddressSpace(AddressSpace& as);
    virtual void addBridge(Bridge& bridge);
    virtual void addInstructionTemplate(InstructionTemplate& instrTempl);
    virtual void addImmediateSlot(ImmediateSlot& slot);
    virtual void setGlobalControl(ControlUnit& unit);
    virtual void unsetGlobalControl();

    virtual ControlUnit* controlUnit() const;

    virtual void removeBus(Bus& bus);
    virtual void removeSocket(Socket& socket);
    virtual void removeUnit(Unit& unit);
    virtual void removeFunctionUnit(FunctionUnit& unit);
    virtual void removeImmediateUnit(ImmediateUnit& unit);
    virtual void removeRegisterFile(RegisterFile& unit);
    virtual void deleteBridge(Bridge& bridge);
    virtual void deleteInstructionTemplate(InstructionTemplate& instrTempl);
    virtual void deleteImmediateSlot(ImmediateSlot& slot);
    virtual void deleteAddressSpace(AddressSpace& as);

    void setBusPosition(const Bus& bus, int newPosition);

    MachineTester& machineTester() const;

    bool alwaysWriteResults() const;
    bool triggerInvalidatesResults() const;
    bool isFUOrdered() const;
    void setAlwaysWriteResults(bool);
    void setTriggerInvalidatesResults(bool);
    void setFUOrdered(bool);
    
    int maximumLatency() const;

    // functions inherited from Serializable interface
    virtual void loadState(const ObjectState* state);
    virtual ObjectState* saveState() const;
    
    virtual void copyFromMachine(Machine& machine);

    static Machine* loadFromADF(const std::string& adfFileName);

    void writeToADF(const std::string& adfFileName) const;

    TCEString hash() const;

    bool hasOperation(const TCEString& opName) const;

    /**
     * A template class which contains machine components.
     */
    template <typename ComponentType>
    class ComponentContainer {
    public:
        ComponentContainer();
        ~ComponentContainer();

        void addComponent(ComponentType* component);
        void removeComponent(ComponentType* component);
        void deleteAll();
        ComponentType* item(int index) const;
        ComponentType* item(const std::string& name) const;
        int count() const;
        void moveToPosition(const ComponentType* component, int position);

        /// Table of ComponentType pointers.
        typedef std::vector<ComponentType*> ComponentTable;

        typedef typename ComponentTable::const_iterator const_iterator;

        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;

    private:
        /// Contains all the components of the container.
        ComponentTable components_;
    };

    /**
     * A template class which is a handle to a ComponentContainer.
     *
     * An instance of this class is intended to return outside the Machine
     * class to allow clients to access individual components in Machine.
     */
    template <typename ComponentType>
    class Navigator {
    public:
        Navigator(
            const Machine::ComponentContainer<ComponentType>& container);
        ~Navigator();

        Navigator(const Navigator& old);
        Navigator& operator=(const Navigator& old);

        ComponentType* item(int index) const;
        ComponentType* item(const std::string& name) const;
        bool hasItem(const std::string& name) const;
        int count() const;

        typedef typename
        Machine::ComponentContainer<ComponentType>::const_iterator
        const_iterator;

        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;

    private:
        /// The container which this navigator handles.
        const Machine::ComponentContainer<ComponentType>* container_;
    };

    /// Navigator type for BusNavigator.
    typedef Navigator<Bus> BusNavigator;
    /// Navigator type for SocketNavigator.
    typedef Navigator<Socket> SocketNavigator;
    /// Navigator type for FunctionUnitNavigator.
    typedef Navigator<FunctionUnit> FunctionUnitNavigator;
    /// Navigator type for AddressSpaceNavigator.
    typedef Navigator<AddressSpace> AddressSpaceNavigator;
    /// Navigator type for BridgeNavigator.
    typedef Navigator<Bridge> BridgeNavigator;
    /// Navigator type for ImmediateUnitNavigator.
    typedef Navigator<ImmediateUnit> ImmediateUnitNavigator;
    /// Navigator type for InstructionTemplateNavigator.
    typedef Navigator<InstructionTemplate> InstructionTemplateNavigator;
    /// Navigator type for RegisterFileNavigator.
    typedef Navigator<RegisterFile> RegisterFileNavigator;
    /// Navigator type for ImmediateSlotNavigator.
    typedef Navigator<ImmediateSlot> ImmediateSlotNavigator;

    virtual BusNavigator busNavigator() const;
    virtual SocketNavigator socketNavigator() const;
    virtual FunctionUnitNavigator functionUnitNavigator() const;
    virtual AddressSpaceNavigator addressSpaceNavigator() const;
    virtual BridgeNavigator bridgeNavigator() const;
    virtual ImmediateUnitNavigator immediateUnitNavigator() const;
    virtual InstructionTemplateNavigator instructionTemplateNavigator()
        const;
    virtual RegisterFileNavigator registerFileNavigator() const;
    virtual ImmediateSlotNavigator immediateSlotNavigator() const;

    /// ObjectState name for Machine.
    static const std::string OSNAME_MACHINE;
    /// ObjectState attribute key for always-write-back-results.
    static const std::string OSKEY_ALWAYS_WRITE_BACK_RESULTS;
    /// ObjectState attribute key for trigger-invalidates-old-results
    static const std::string OSKEY_TRIGGER_INVALIDATES_OLD_RESULTS;
    /// ObjectState attribute key for function units ordered in order
    /// of their sequential presence in ADF
    static const std::string OSKEY_FUNCTION_UNITS_ORDERED;

    bool isLittleEndian() const { return littleEndian_; }
    void setLittleEndian(bool flag) { littleEndian_ = flag; }

private:
    /// Assignment not allowed.
    Machine& operator=(const Machine&);

    template <typename ContainerType, typename ComponentType>
    void addComponent(ContainerType& container, ComponentType& toAdd);

    template <typename ContainerType, typename ComponentType>
    void addRegisteredComponent(ContainerType& container, ComponentType& toAdd);

    template <typename ContainerType, typename ComponentType>
    void removeComponent(ContainerType& container, ComponentType& toRemove);

    template <typename ContainerType, typename ComponentType>
    void deleteComponent(ContainerType& container, ComponentType& toDelete);

    template <typename ContainerType>
    static void saveComponentStates(
        ContainerType& container,
        ObjectState* parent);

    /// Contains all the busses attached to the machine.
    ComponentContainer<Bus> busses_;
    /// Contains all the sockets attached to the machine.
    ComponentContainer<Socket> sockets_;
    /// Contains all the instruction templates of the machine.
    ComponentContainer<InstructionTemplate> instructionTemplates_;
    /// Contains all the register files of the machine.
    ComponentContainer<RegisterFile> registerFiles_;
    /// Contains all the immediate units of the machine.
    ComponentContainer<ImmediateUnit> immediateUnits_;
    /// Contains all the function units of the machine.
    ComponentContainer<FunctionUnit> functionUnits_;
    /// Contains all the address spaces of the machine.
    ComponentContainer<AddressSpace> addressSpaces_;
    /// Contains all the bridges of the machine.
    ComponentContainer<Bridge> bridges_;
    /// Contains all the immediate slots of the machine.
    ComponentContainer<ImmediateSlot> immediateSlots_;

    /// Global control unit.
    ControlUnit* controlUnit_;

    /// Tells whether to do validity checks or not.
    bool doValidityChecks_;
    /// Machine tester for the machine.
    MachineTester* machineTester_;
    /// Dummy machine tester for the machine.
    DummyMachineTester* dummyMachineTester_;

    // Name of the empty instruction template created by default.
    const std::string EMPTY_ITEMP_NAME_;
    
    // Result move have to be always written to register. Effectively
    // disable dead result read elimination.
    bool alwaysWriteResults_;
    // Triggering invalidates content of register where value of result will be
    // written. Previous value can not be read any more.
    bool triggerInvalidatesResults_;
    // Function units relative order can be important (SPU).
    // If set to 1, each FU will be given order id based on their sequential
    // ordering in the ADF.
    bool fuOrdered_;
    // True in case the machine is little-endian, big-endian otherwise.
    bool littleEndian_;
};
}

#include "Machine.icc"

#endif
