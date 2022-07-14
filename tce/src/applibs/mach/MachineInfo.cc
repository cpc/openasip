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
#include "OperationDAGSelector.hh"
#include "MathTools.hh"
#include "MachineConnectivityCheck.hh"
#include "UniversalMachine.hh"

using namespace TTAMachine;

const TCEString MachineInfo::LOCK_READ_ = "lock_read";
const TCEString MachineInfo::TRY_LOCK_ADDR_ = "try_lock_addr";
const TCEString MachineInfo::UNLOCK_ADDR_ = "unlock_addr";

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
            opNames.insert(StringTools::stringToLower(opName));
        }
    }
    
    return opNames;
}


/**
 * Returns opset used by Global Control Unit.
 *
 * @param gcu The Global Control Unit whose opset is requested.
 * @return Opset used by the Global Control Unit.
 */
OperationDAGSelector::OperationSet
MachineInfo::getOpset(const TTAMachine::ControlUnit& gcu) {
    OperationDAGSelector::OperationSet opNames;
    for (int i = 0; i < gcu.operationCount(); i++) {
        const std::string opName = gcu.operation(i)->name();
        opNames.insert(StringTools::stringToLower(opName));
    }
    return opNames;
}


/**
 * Return first occurrence of FUPorts that are bound to operation given by
 * name.
 *
 * Returned list has FUPorts ordered in the order of operands. At index 1 is
 * port bounded to operand 1, at index 2 port bounded to operand 2 and so on.
 * List is empty if operation was not found from machine. Unbounded operands
 * have NULL at the index of operand (0 is always NULL).
 *
 * @param mach The machine.
 * @param operationStr The operation.
 * @return The list of ordered bound ports.
 */
MachineInfo::ConstPortList
MachineInfo::getPortBindingsOfOperation(
    const TTAMachine::Machine& mach,
    const std::string& operationStr) {

    ConstPortList bindedPorts;

    const TTAMachine::FunctionUnit* found = NULL;
    const TTAMachine::Machine::FunctionUnitNavigator fuNav =
            mach.functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit* fu = fuNav.item(i);
        if (fu->hasOperation(operationStr)) {
            found = fu;
            break;
        }
    }

    if (found == NULL) {
        if (mach.controlUnit()->hasOperation(operationStr)) {
            found = mach.controlUnit();
        }
    }
    if (found != NULL) {
        bindedPorts.push_back(NULL); // Shift indexing.
        HWOperation* operation = found->operation(operationStr);
        for (int i = 1; i <= operation->operandCount(); i++) {
            bindedPorts.push_back(operation->port(i));
        }
    }
    return bindedPorts;
}

/**
 * Returns port that is bound to operand index of the operation in the FU.
 *
 * Returns the port, if the FU has the operation and operation has the operand
 * bound to some port. Otherwise, returns nullptr.
 */
const TTAMachine::FUPort*
MachineInfo::getBoundPort(
    const TTAMachine::FunctionUnit& fu,
    const std::string& opName, int operandIndex) {

    if (fu.hasOperation(opName)) {
        const HWOperation* hwOp = fu.operation(opName);
        if (hwOp->isBound(operandIndex)) {
            return hwOp->port(operandIndex);
        }
    }
    return nullptr;
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
 * Returns byte width of the widest load/store operation.
 *
 * @return Maximum memory alignment according to the widest memory operation.
 */
int
MachineInfo::maxMemoryAlignment(const TTAMachine::Machine& mach) {
    int byteAlignment = 4; // Stack alignment is four bytes at minimum.

    OperationDAGSelector::OperationSet opNames = getOpset(mach);
    OperationDAGSelector::OperationSet::const_iterator it;

    for (it = opNames.begin(); it != opNames.end(); ++it) {
        const TCEString opName = StringTools::stringToLower(*it);
        if (opName.length() > 2 && isdigit(opName[2])) {
            // Assume operations named ldNNxMM and stNNxMM are operations
            // with alignment of NN (or ldNN / stNN). 
            // @todo fix this horrible hack by adding the alignment info explicitly 
            // to OSAL. 

            // At least check for the name string format to match the above
            // before parsing the number.
            size_t xpos = opName.find("x", 3);
            if (xpos == std::string::npos) {
                for (size_t pos = 2; pos < opName.size(); ++pos)
                    if (!isdigit(opName[pos])) continue;

                if (opName.startsWith("ld")) {
                    int loadByteWidth = Conversion::toInt(opName.substr(2)) / 8; 
                    if (loadByteWidth > byteAlignment) {
                        byteAlignment = loadByteWidth;
                    }
                } else if (opName.startsWith("st")) {
                    int storeByteWidth = Conversion::toInt(opName.substr(2)) / 8; 
                    if (storeByteWidth > byteAlignment) {
                        byteAlignment = storeByteWidth;
                    }
                }
            } else {
                for (size_t pos = xpos + 1; pos < opName.size(); ++pos)
                    if (!isdigit(opName[pos])) continue;

                if (opName.startsWith("ld")) {
                    int loadByteWidth = Conversion::toInt(opName.substr(2, xpos - 2)) / 8; 
                    if (loadByteWidth > byteAlignment) {
                        byteAlignment = loadByteWidth;
                    }
                } else if (opName.startsWith("st")) {
                    int storeByteWidth = Conversion::toInt(opName.substr(2, xpos - 2)) / 8; 
                    if (storeByteWidth > byteAlignment) {
                        byteAlignment = storeByteWidth;
                    }
                }
            }
        } 
    }

    if (!(byteAlignment > 1 && !(byteAlignment & (byteAlignment - 1)))) {
        std::cerr << "Stack alignment: " << byteAlignment << std::endl;
        assert(false && "Error: stack alignment must be a power of 2.");
    }

    return byteAlignment;
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

bool
MachineInfo::canEncodeImmediateInteger(
    const TTAMachine::Bus& bus, int64_t imm, unsigned destWidth) {

    size_t requiredBitsSigned =
        std::min((unsigned)MathTools::requiredBitsSigned((long int)imm), destWidth);
    size_t requiredBitsUnsigned =
        std::min((unsigned)MathTools::requiredBits(imm), destWidth);

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
    if (static_cast<size_t>(bus.immediateWidth()) >= requiredBits)
        return true;
    return false;
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
 * no direct bus with the immediate support is found. In
 * case the destination (port) is known, its width should 
 * be set to destWidth to constraint the required immediate
 * extension width (in case the move tries to write to a port narrower
 * than the immediate actually requires bits, the bug is earlier 
 * in the compilation).
 *
 * A complex example: a move where a negative constant with minimal
 * encoding of 7b is transported through a 32b bus to a 8b destination
 * with a 8b move slot that uses zero extension mode. The zero extension
 * mode does not fill up the upper bits with 1, but it does not
 * lead to information loss as the destination uses only the lower
 * 8 bits which can be encoded directly to the immediate field,
 * thus the extension is not needed.
 */
bool
MachineInfo::canEncodeImmediateInteger(
    const TTAMachine::Machine& mach, int64_t imm, unsigned destWidth) {

    const Machine::BusNavigator& busNav = mach.busNavigator();

    // first check the short immediate slots
    for (int bi = 0; bi < busNav.count(); ++bi) {
        const Bus& bus = *busNav.item(bi);
        if (canEncodeImmediateInteger(bus, imm, destWidth))
            return true;
    }

    // then the long immediate templates
    for (const TTAMachine::InstructionTemplate* temp: mach.instructionTemplateNavigator())
        if (canEncodeImmediateInteger(*temp, imm, destWidth))
            return true;

    return false;
}

bool
MachineInfo::canEncodeImmediateInteger(
    const TTAMachine::InstructionTemplate& temp, int64_t imm, 
    unsigned destWidth) {
    size_t requiredBitsSigned =
        std::min((unsigned)MathTools::requiredBitsSigned(
                     static_cast<SLongWord>(imm)), destWidth);
    size_t requiredBitsUnsigned =
        std::min((unsigned)MathTools::requiredBits(
             static_cast<ULongWord>(imm)), destWidth);

    const TTAMachine::Machine& mach = *temp.machine();
    const TTAMachine::Machine::ImmediateUnitNavigator nav = 
        mach.immediateUnitNavigator();
    for (const TTAMachine::ImmediateUnit* iu: mach.immediateUnitNavigator()) {
        size_t requiredBits = iu->signExtends() ? 
            requiredBitsSigned : requiredBitsUnsigned;
        size_t supportedW = temp.supportedWidth(*iu);
        // see above (*). Same applies here: if the template encodes
        // as many bits as the buses that the IU can write to are wide, 
        // the extension mode is meaningless -> can interpret it as one wishes 
        // here.
        size_t maxBusW = 0;
        std::set<const TTAMachine::Bus*> buses;
        MachineConnectivityCheck::appendConnectedDestinationBuses(
            *iu, buses);
        for (auto bus: buses) {
            if (static_cast<size_t>(bus->width()) > maxBusW)
                maxBusW = bus->width();
        }

        if (supportedW == maxBusW &&
            requiredBitsSigned < requiredBits)
            requiredBits = requiredBitsSigned;
        if (supportedW >= requiredBits)
            return true;

    }

    return false;
}

int 
MachineInfo::triggerIndex(
    const TTAMachine::FunctionUnit& fu, const Operation& op) {
    if (fu.hasOperation(op.name())) {
        TTAMachine::HWOperation* hwop = 
            fu.operation(op.name());
        for (int j = 0; j < fu.operationPortCount(); j++) {
            TTAMachine::FUPort* port = fu.operationPort(j);
            if (port->isTriggering()) {
                return hwop->io(*port);
            }
        } 
    }
    // operation not found in this FU
    return 0;
}

/**
 * Finds the operand index of trigger of given operation in the machine.
 *
 * If the operation is not found from the machine, return 0.
 * If the index is ambiguos return -1.
 */
int
MachineInfo::triggerIndex(
    const TTAMachine::Machine& machine, const Operation& op) {
    TTAMachine::Machine::FunctionUnitNavigator nav = 
        machine.functionUnitNavigator();
    if (&machine == &UniversalMachine::instance()) {
        return op.numberOfInputs(); // last input
    }
    int index = 0;
    for (int i = 0; i < nav.count(); i++) {
        TTAMachine::FunctionUnit* fu = nav.item(i);
        int curIndex = triggerIndex(*fu, op);
        if (curIndex > 0) {
            if (index > 0 && curIndex != index) {
                return -1;
            } else {
                index = curIndex;
            }
        }
    }
    int curIndex = triggerIndex(*machine.controlUnit(), op);
    if (curIndex > 0) {
        if (index > 0 && curIndex != index) {
            return -1;
        } else {
            index = curIndex;
        }
    }
    return index;
}

/**
 * Finds the widest operand available in the machine.
 *
 * Helps finding the widest usable register in the machine.
 */
unsigned
MachineInfo::findWidestOperand(
    const TTAMachine::Machine& machine,
    bool) {
    OperationPool pool;
    unsigned widestOperand = 0;

    TTAMachine::Machine::FunctionUnitNavigator FUNavigator =
        machine.functionUnitNavigator();

    OperationDAGSelector::OperationSet opNames =
        MachineInfo::getOpset(machine);
    for (TCETools::CIStringSet::iterator it = opNames.begin();
         it != opNames.end(); ++it) {

        const Operation& op = pool.operation(it->c_str());

        for (int j = 1; j < op.operandCount() + 1; ++j) {
            if ((unsigned)(op.operand(j).width()) > widestOperand)
                widestOperand = (unsigned)(op.operand(j).width());
        }
    }
    return widestOperand;
}

/**
 * Counts registers of given width.
 */
unsigned
MachineInfo::numberOfRegisters(
    const TTAMachine::Machine& machine, unsigned width) {

    unsigned numRegisters = 0;
    TTAMachine::Machine::RegisterFileNavigator RFNavigator =
        machine.registerFileNavigator();

    // Search register files of desired width and count registers
    for (int i = 0; i < RFNavigator.count(); i++) {
        TTAMachine::RegisterFile *rf = RFNavigator.item(i);
        if ((unsigned)(rf->width()) == width) {
            numRegisters += rf->size();
        }
    }
    return numRegisters;
}

/**
 * Returns true if the machine has predicatable jump.
 *
 */
bool
MachineInfo::supportsBoolRegisterGuardedJumps(
    const TTAMachine::Machine& machine) {
    const ControlUnit* cu = machine.controlUnit();
    if (cu == nullptr) {
        return false;
    }

    if (!cu->hasOperation("jump")) {
        return false;
    }

    const FUPort* jumpPort = getBoundPort(*cu, "jump", 1);
    if (jumpPort == nullptr) {
        return false;
    }

    std::vector<const Bus*> guardedBuses;
    for (const Bus* bus : machine.busNavigator()) {
        for (int i = 0; i < bus->guardCount(); i++) {

            const TTAMachine::RegisterGuard *regGuard =
                dynamic_cast<TTAMachine::RegisterGuard*>(bus->guard(i));
            if (regGuard && regGuard->registerFile()->width() == 1) {
                guardedBuses.push_back(bus);
                break;
            }
        }
    }

    for (const Bus* bus : guardedBuses) {
        if (MachineConnectivityCheck::busConnectedToPort(*bus, *jumpPort)) {
            return true;
        }
        // Todo: Check if bus has sufficient transport capability for jumping?
        // That is, it is at least connected to a RF, IU or can transport
        // short immediates.
    }

    return false;
}


/**
 * Returns true if the machine has predicatable jump.
 *
 */
bool
MachineInfo::supportsPortGuardedJumps(const TTAMachine::Machine& machine) {
    const ControlUnit* cu = machine.controlUnit();
    if (cu == nullptr) {
        return false;
    }

    if (!cu->hasOperation("jump")) {
        return false;
    }

    const FUPort* jumpPort = getBoundPort(*cu, "jump", 1);
    if (jumpPort == nullptr) {
        return false;
    }

    std::vector<const Bus*> guardedBuses;
    for (const Bus* bus : machine.busNavigator()) {
        for (int i = 0; i < bus->guardCount(); i++) {
            const TTAMachine::PortGuard *portGuard =
                dynamic_cast<TTAMachine::PortGuard*>(bus->guard(i));
            if (portGuard) {
                // TODO: should check what ops found from the source FU
                guardedBuses.push_back(bus);
            }
        }
    }

    for (const Bus* bus : guardedBuses) {
        if (MachineConnectivityCheck::busConnectedToPort(*bus, *jumpPort)) {
            return true;
        }
        // Todo: Check if bus has sufficient transport capability for jumping?
        // That is, it is at least connected to a RF, IU or can transport
        // short immediates.
    }

    return false;
}


bool MachineInfo::supportsPortGuardedJump(
    const TTAMachine::Machine& machine, bool inverted, const TCEString& opName) {

    const ControlUnit* cu = machine.controlUnit();
    if (cu == nullptr) {
        return false;
    }

    if (!cu->hasOperation("jump")) {
        return false;
    }

    const FUPort* jumpPort = getBoundPort(*cu, "jump", 1);
    if (jumpPort == nullptr) {
        return false;
    }

    std::vector<const Bus*> guardedBuses;
    for (const Bus* bus : machine.busNavigator()) {
        for (int i = 0; i < bus->guardCount(); i++) {
            TTAMachine::Guard* guard = bus->guard(i);
            if (guard->isInverted() != inverted) continue;
            const TTAMachine::PortGuard *portGuard =
                dynamic_cast<TTAMachine::PortGuard*>(guard);
            if (portGuard) {
                TTAMachine::FUPort* fup = portGuard->port();
                auto fu = fup->parentUnit();
                if (fu->hasOperation(opName)) {
                // TODO: should check what ops found from the source FU
                    guardedBuses.push_back(bus);
                }
            }
        }
    }

    for (const Bus* bus : guardedBuses) {
        if (MachineConnectivityCheck::busConnectedToPort(*bus, *jumpPort)) {
            return true;
        }
        // Todo: Check if bus has sufficient transport capability for jumping?
        // That is, it is at least connected to a RF, IU or can transport
        // short immediates.
    }

    return false;
}

/**
 * Searches for lock unit FUs and returns them.
 *
 * Lock unit FU must have the correct operations and an address space
 *
 * @param machine Architecture to be searched
 * @return Vector of found lock unit FUs
 */
std::vector<const TTAMachine::FunctionUnit*>
MachineInfo::findLockUnits(const TTAMachine::Machine& machine) {
    std::vector<TCEString> requiredOperations;
    requiredOperations.push_back(LOCK_READ_);
    requiredOperations.push_back(TRY_LOCK_ADDR_);
    requiredOperations.push_back(UNLOCK_ADDR_);

    std::vector<const FunctionUnit*> lockUnits;
    Machine::FunctionUnitNavigator fuNav = machine.functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        const FunctionUnit* fu = fuNav.item(i);
        bool hasCorrectOperations = true;
        for (unsigned int i = 0; i < requiredOperations.size(); i++) {
            if (!fu->hasOperation(requiredOperations.at(i))) {
                hasCorrectOperations = false;
                break;
            }
        }
        if (hasCorrectOperations) {
            if (!fu->hasAddressSpace()) {
                TCEString msg;
                msg << "Lock Unit " << fu->name() << " has no address space";
                throw InvalidData(__FILE__, __LINE__, __func__, msg);
            }
            lockUnits.push_back(fu);
        }
    }
    return lockUnits;
}

/**
 * Convenience function for getting OSAL operation from HWOperation description.
 *
 * Throws InstanceNotFound if OSAL operation is not found.
 */
Operation&
MachineInfo::osalOperation(const TTAMachine::HWOperation& hwOp) {
    OperationPool opPool;

    Operation& op = opPool.operation(hwOp.name().c_str());
    if (op.isNull()) {
        THROW_EXCEPTION(InstanceNotFound,
            "Operation '" + hwOp.name() + "' was not found in OSAL.");
    }
    return op;
}


int MachineInfo::maxLatency(const TTAMachine::Machine& mach, TCEString& opName) {
    int maxl = -1;
    for (auto fu: mach.functionUnitNavigator()) {
        if (fu->hasOperation(opName)) {
            const auto op = fu->operation(opName);
            maxl = std::max(maxl, op->latency());
        }
    }
    if (mach.controlUnit()->hasOperation(opName)) {
        const auto op = mach.controlUnit()->operation(opName);
        maxl = std::max(maxl, op->latency());
    }
    return maxl;
}
