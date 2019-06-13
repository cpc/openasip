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
 * @file NullMachine.hh
 *
 * Declaration of NullMachine class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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

    virtual void addBus(Bus& bus);
    virtual void addSocket(Socket& socket);
    void addUnit(Unit& unit);
    virtual void addFunctionUnit(FunctionUnit& unit);
    virtual void addImmediateUnit(ImmediateUnit& unit);
    virtual void addRegisterFile(RegisterFile& unit);
    virtual void addAddressSpace(AddressSpace& as);
    virtual void addBridge(Bridge& bridge);
    virtual void addInstructionTemplate(InstructionTemplate& instrTempl);
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
    virtual void deleteAddressSpace(AddressSpace& as);

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
    virtual void loadState(const ObjectState* state);
    virtual ObjectState* saveState() const;

private:
    NullMachine();
    virtual ~NullMachine();

    /// The only instance of NullMachine.
    static NullMachine machine_;
};
}

#endif
