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
 * @file MachineInfo.cc
 *
 * Implementation of MachineInfo class.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#include "MachineInfo.hh"

#include "StringTools.hh"
#include "Machine.hh"
#include "HWOperation.hh"
#include "OperationPool.hh"
#include "ControlUnit.hh"
#include "RegisterFile.hh"
#include "Guard.hh"
#include "Operation.hh"
#include "Operand.hh"
#include "FUPort.hh"
#include "InstructionTemplate.hh"
#include "MathTools.hh"

using namespace TTAMachine;

/**
 * Checks that the operands used in the operations of the given FU are
 * bound to some port.
 *
 * @param mach The machine whose opset is requested.
 * @return Opset supported by machine hardware.
 */
OperationDAGSelector::OperationSet
MachineInfo::getOpset(const TTAMachine::Machine &mach) {

    OperationDAGSelector::OperationSet opNames;

    const TTAMachine::Machine::FunctionUnitNavigator fuNav =
        mach.functionUnitNavigator();

    OperationPool opPool;

    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit* fu = fuNav.item(i);
        for (int o = 0; o < fu->operationCount(); o++) {
            const std::string opName = fu->operation(o)->name();
            opNames.insert(StringTools::stringToUpper(opName));
        }
    }
    
    return opNames;
}

/**
 * Finds the default data address space for given machine.
 *
 * @param mach The machine whose default data address space is requested.
 * @return Default data address space for given machine
 */
TTAMachine::AddressSpace*
MachineInfo::defaultDataAddressSpace(const TTAMachine::Machine& mach) {

    const AddressSpace& instrAS = *mach.controlUnit()->addressSpace();
    
    TTAMachine::Machine::AddressSpaceNavigator asNav =
        mach.addressSpaceNavigator();
    int asCount = asNav.count();
    for (int i = 0; i < asCount; i++) {
        // if there are more than two address spaces, choose one which
        // contains numerical id 0
        // otherwise choose the one which is not the instruction address space
        if (asCount > 2) {
            if (asNav.item(i)->hasNumericalId(0)) return asNav.item(i);
        } else {
            if (asNav.item(i) != &instrAS) return asNav.item(i);
        }
    }
    
    // no data address space found
    throw IllegalMachine(
        __FILE__, __LINE__, __func__,
        "Target machine has no data address space");
    return NULL;
}

int 
MachineInfo::longestGuardLatency(
    const TTAMachine::Machine& mach) {
    int ggLatency = mach.controlUnit()->globalGuardLatency();

    const TTAMachine::Machine::BusNavigator busNav =
        mach.busNavigator();

    for (int i = 0; i < busNav.count(); i++) {
        const TTAMachine::Bus* bus = busNav.item(i);
        for (int j = 0; j < bus->guardCount(); j++) {
            Guard* guard = bus->guard(j);
            RegisterGuard* rg = dynamic_cast<RegisterGuard*>(guard);
            if (rg != NULL) {
                int rgLat = rg->registerFile()->guardLatency();
                if (rgLat != 0) {
                    assert(rgLat == 1);
                    return ggLatency + 1;
                }
            } else {
                if (dynamic_cast<PortGuard*>(guard) != NULL) {
                    return ggLatency + 1;
                }
            }
        }
    }
    return ggLatency;
}

/**
 * Returns Operand object for the given hardware operation attached to the port.
 *
 * InstanceNotFound exception is thrown, if the hardware operation doesn't have
 * an operand in the given port. 
 * 
 * @param hwOp Hardware operation.
 * @param port The port containing the desired Operand. 
 * @return Reference to the Operand object.
 */
Operand& 
MachineInfo::operandFromPort(
    const TTAMachine::HWOperation& hwOp,
    const TTAMachine::FUPort& port) {
 
    const TCEString& opName = hwOp.name();
    OperationPool opPool;
    const Operation& op = opPool.operation(opName.c_str());

    assert(&op != &NullOperation::instance() && "Invalid operation name.");

    int opndIndex = hwOp.io(port);
    return op.operand(opndIndex);
}

/**
 * Checks if slot is used in any of the instruction templates defined in ADF.
 *
 * @param mach The Machine.
 * @param slotName The name of the slot.
 * @return True if any template includes given slot. False otherwise.
 */
bool
MachineInfo::templatesUsesSlot(
    const TTAMachine::Machine& mach,
    const std::string& slotName) {

    std::set<InstructionTemplate*> affectingInstTemplates;
    Machine::InstructionTemplateNavigator itNav =
        mach.instructionTemplateNavigator();
    for (int i = 0; i < itNav.count(); i++) {
        InstructionTemplate* iTemp = itNav.item(i);
        if (iTemp->usesSlot(slotName)) {
            return true;
        }
    }

    return false;
}

/**
 * Returns set of pointers to instruction templates that uses the given slot.
 *
 * @param mach The Machine.
 * @param slotName The name of the slot.
 * @return List of found templates or empty none was found.
 */
std::set<TTAMachine::InstructionTemplate*>
MachineInfo::templatesUsingSlot(
    const TTAMachine::Machine& mach,
    const std::string& slotName) {
    std::set<InstructionTemplate*> affectingInstTemplates;
    Machine::InstructionTemplateNavigator itNav =
        mach.instructionTemplateNavigator();
    for (int i = 0; i < itNav.count(); i++) {
        InstructionTemplate* iTemp = itNav.item(i);
        if (iTemp->usesSlot(slotName)) {
            affectingInstTemplates.insert(iTemp);
        }
    }
    return affectingInstTemplates;
}


bool 
MachineInfo::supportsOperation(
    const TTAMachine::Machine& mach, TCEString operation) {
    OperationDAGSelector::OperationSet opNames = 
        MachineInfo::getOpset(mach);
    return opNames.find(operation.upper()) != opNames.end();
}

/**
 * Checks if the given immediate can be transferred at all
 * in the given machine.
 *
 * Takes in account the move slots' or the immediate unit's
 * extension mode when considering the encoding of the given 
 * constant's bits. In case this function returns true,
 * the register copy adder or similar pass should be able to
 * route the constant to the wanted destination(s), in case
 * no direct bus with the immediate support is found.
 */
bool
MachineInfo::canEncodeImmediateInteger(
    const TTAMachine::Machine& mach, int64_t imm) {

    const Machine::BusNavigator& busNav = mach.busNavigator();

    size_t requiredBitsSigned = MathTools::requiredBitsSigned(imm);
    size_t requiredBitsUnsigned = MathTools::requiredBits(imm);
    // first check the short immediate slots
    for (int bi = 0; bi < busNav.count(); ++bi) {
        const Bus& bus = *busNav.item(bi);
        size_t requiredBits = bus.signExtends() ? 
            requiredBitsSigned : requiredBitsUnsigned;
        // In case the short immediate can write all bits in
        // the bus, let's assume this is the word width to the
        // target operation and the extension mode can be
        // assumed to be 'signed' (the targeted operation can 
        // interpret the value in the bus either way), we
        // just have to be sure there is no information loss
        // in the upper bits. This breaks with
        // multibitwidth scalar machines, e.g., ones with
        // INT32 datapath combined with FLOAT64 because now
        // it assumes the constant can be written in case
        // there's a 32b immediate slot for the INT32 bus. (*)
        if (bus.width() == bus.immediateWidth() &&
            requiredBitsSigned < requiredBits)
            requiredBits = requiredBitsSigned;
        if (bus.immediateWidth() >= requiredBits)
            return true;
    }

    // then the long immediate units
    TTAMachine::Machine::ImmediateUnitNavigator nav = 
        mach.immediateUnitNavigator();
    for (int i = 0; i < nav.count(); i++) {
        const TTAMachine::ImmediateUnit& iu = *nav.item(i);
        size_t requiredBits = iu.signExtends() ? 
            requiredBitsSigned : requiredBitsUnsigned;
        TTAMachine::Machine::InstructionTemplateNavigator inav = 
            mach.instructionTemplateNavigator();
        for (int t = 0; t < inav.count(); ++t) {
            const TTAMachine::InstructionTemplate& itempl = *inav.item(t);
            size_t supportedW = itempl.supportedWidth(iu);
            // see above (*). Same applies here: if the template encodes
            // as many bits as the IU is wide, the extension mode is
            // meaningless -> can interpret it as one wishes here.
            if (supportedW == iu.width() &&
                requiredBitsSigned < requiredBits)
                requiredBits = requiredBitsSigned;

            if (supportedW >= requiredBits)
                return true;
        }
    } 
    
    return false;
}
