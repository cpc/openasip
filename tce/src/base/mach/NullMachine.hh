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
