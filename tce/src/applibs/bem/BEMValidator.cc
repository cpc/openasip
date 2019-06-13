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
 * @file BEMValidator.cc
 *
 * Implementation of BEMValidator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#include <string>
#include <boost/format.hpp>

#include "BEMValidator.hh"

#include "Machine.hh"
#include "Segment.hh"
#include "Socket.hh"
#include "HWOperation.hh"
#include "Guard.hh"
#include "FUPort.hh"
#include "ControlUnit.hh"

#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "DestinationField.hh"
#include "SourceField.hh"
#include "GuardField.hh"
#include "SocketEncoding.hh"
#include "SocketCodeTable.hh"
#include "ImmediateSlotField.hh"
#include "ImmediateControlField.hh"
#include "LImmDstRegisterField.hh"

#include "AssocTools.hh"
#include "SetTools.hh"
#include "ContainerTools.hh"
#include "MathTools.hh"

using std::string;
using boost::format;
using namespace TTAMachine;

/**
 * The constructor.
 *
 * @param bem The binary encoding map.
 * @param machine The machine.
 */
BEMValidator::BEMValidator(
    const BinaryEncoding& bem,
    const TTAMachine::Machine& machine) :
    bem_(bem), machine_(machine) {
}


/**
 * The destructor.
 */
BEMValidator::~BEMValidator() {
}


/**
 * Validates the binary encoding map against the machine.
 *
 * Updates the error and warning messages that can be queried with 
 * {error,warning}Count and {error,warning}Message methods.
 *
 * @return True if the BEM is valid for the machine, otherwise false.
 */
bool
BEMValidator::validate() {

    errorMessages_.clear();

    // check move slots for each bus
    Machine::BusNavigator busNav = machine_.busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        checkMoveSlot(*bus);
    }

    // check immediate slots
    Machine::ImmediateSlotNavigator immSlotNav = 
        machine_.immediateSlotNavigator();
    for (int i = 0; i < immSlotNav.count(); i++) {
        ImmediateSlot* immSlot = immSlotNav.item(i);
        checkImmediateSlot(*immSlot);
    }

    // check immediate control field
    checkImmediateControlField();   

    // check long immediate destination register fields
    checkLImmDstRegisterFields();

    TTAMachine::ControlUnit* gcu = machine_.controlUnit();
    if (gcu == NULL) {
        string errorMsg("Error: Couldn't find GCU from machine");
        errorMessages_.push_back(errorMsg);
    } else {
        // no reason to check this as we are generating the
        // instruction memory width according to the instruction
        // width always (instruction indexed imem assumed)
        // checkImemMauWidth(*gcu);
    }

    if (errorCount() == 0) {
        return true;
    } else {
        return false;
    }
}


/**
 * Returns the number of errors found.
 *
 * @return The number of errors.
 */
int
BEMValidator::errorCount() const {
    return errorMessages_.size();
}


/**
 * Returns an error message by the given index.
 *
 * @param index The index.
 * @return The error message by the given index.
 * @exception OutOfRange If the given index is negative or not smaller
 *                       than the number of errors.
 */
std::string
BEMValidator::errorMessage(int index) const {
    if (index < 0 || index >= errorCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    } else {
        return errorMessages_[index];
    }
}

/**
 * Returns the number of warnings found.
 *
 * @return The number of warnings.
 */
int
BEMValidator::warningCount() const {
    return warningMessages_.size();
}

/**
 * Returns an warning message by the given index.
 *
 * @param index The index.
 * @return The warning message by the given index.
 * @exception OutOfRange If the given index is negative or not smaller
 *                       than the number of errors.
 */
std::string
BEMValidator::warningMessage(int index) const {

    if (index < 0 || index >= warningCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    } else {
        return warningMessages_[index];
    }
}

/**
 * Checks that the move slot for the given bus is valid in BEM.
 *
 * If errors are found, error messages are inserted to the vector of
 * error messages.
 *
 * @param bus The bus.
 */
void
BEMValidator::checkMoveSlot(const TTAMachine::Bus& bus) {

    if (!bem_.hasMoveSlot(bus.name())) {
        format errorMsg("BEM does not contain move slot for bus %1%.");
        errorMsg % bus.name();
        errorMessages_.push_back(errorMsg.str());
        return;
    }

    checkSourceField(bus);
    checkDestinationField(bus);
    checkGuardField(bus);
}


/**
 * Checks the source field of the given bus for errors.
 *
 * If errors are found, inserts the error messages to the vector of error
 * messages.
 */
void
BEMValidator::checkSourceField(const TTAMachine::Bus& bus) {

    MoveSlot& moveSlot = bem_.moveSlot(bus.name());

    // collect the output sockets to a set
    typedef std::set<Socket*> SocketSet;
    SocketSet outputSockets;
    
    for (int i = 0; i < bus.segmentCount(); i++) {
        Segment* segment = bus.segment(i);
        for (int i = 0; i < segment->connectionCount(); i++) {
            Socket* socket = segment->connection(i);
            if (socket->portCount() > 0 &&
                socket->direction() == Socket::OUTPUT) {
                outputSockets.insert(socket);
            }
        }
    }

    if (needsSourceField(moveSlot)) {
        if (!moveSlot.hasSourceField()) {
            format errorMsg(
                "Move slot of bus %1% does not contain source field.");
            errorMsg % bus.name();
            errorMessages_.push_back(errorMsg.str());
        } else {
            // check the source field for socket encodings
            SourceField& srcField = moveSlot.sourceField();
            for (SocketSet::const_iterator iter = outputSockets.begin();
                 iter != outputSockets.end(); iter++) {
                Socket* socket = *iter;
                if (!srcField.hasSocketEncoding(socket->name())) {
                    format errorMsg(
                        "Source field of move slot of bus %1% does not"
                        "have an encoding for socket %2%.");
                    errorMsg % bus.name() % socket->name();
                    errorMessages_.push_back(errorMsg.str());
                } else {
                    SocketEncoding& socketEnc = srcField.socketEncoding(
                        socket->name());
                    if (needsSocketCodeTable(socketEnc)) {
                        if (!socketEnc.hasSocketCodes()) {
                            format errorMsg(
                                "Encoding of socket %1% in source "
                                "field of move slot %2% does not refer to "
                                "any socket code table.");
                            errorMsg % socket->name() % bus.name();
                            errorMessages_.push_back(errorMsg.str());
                        } else {
                            checkSocketCodeTable(socketEnc);
                        }
                    }
                }
            }
        
            // check the source field for bridge encodings
            Machine::BridgeNavigator bridgeNav = machine_.bridgeNavigator();
            for (int i = 0; i < bridgeNav.count(); i++) {
                Bridge* bridge = bridgeNav.item(i);
                if (bridge->destinationBus() == &bus && 
                    !moveSlot.sourceField().hasBridgeEncoding(
                        bridge->name())) {
                    format errorMsg(
                        "Source field of bus %1% does not have encoding "
                        "for bridge %2%.");
                    errorMsg % bus.name() % bridge->name();
                    errorMessages_.push_back(errorMsg.str());
                }
            }
            
            // check the source field for immediate encoding
            if (bus.immediateWidth() > 0 &&
                !srcField.hasImmediateEncoding()) {
                format errorMsg(
                    "Source field of bus %1% does not have an immediate "
                    "encoding.");
                errorMsg % bus.name();
                errorMessages_.push_back(errorMsg.str());
            }
        }
    }                       
}


/**
 * Checks the destination field of the given bus for errors.
 *
 * If errors are found, inserts the error messages to the vector of error
 * messages.
 */
void
BEMValidator::checkDestinationField(const TTAMachine::Bus& bus) {

    MoveSlot& moveSlot = bem_.moveSlot(bus.name());

    // collect the input sockets to a set
    typedef std::set<Socket*> SocketSet;
    SocketSet inputSockets;
    
    for (int i = 0; i < bus.segmentCount(); i++) {
        Segment* segment = bus.segment(i);
        for (int i = 0; i < segment->connectionCount(); i++) {
            Socket* socket = segment->connection(i);
            if (socket->portCount() > 0 &&
                socket->direction() == Socket::INPUT) {
                inputSockets.insert(socket);
            }
        }
    }
    
    // check the destination field
    if (inputSockets.size() > 1) {
        if (!moveSlot.hasDestinationField()) {
            format errorMsg(
                "Move slot of bus %1% does not contain destination field.");
            errorMsg % bus.name();
            errorMessages_.push_back(errorMsg.str());
        } else {
            DestinationField& dstField = moveSlot.destinationField();
            for (SocketSet::const_iterator iter = inputSockets.begin();
                 iter != inputSockets.end(); iter++) {
                Socket* socket = *iter;
                if (!dstField.hasSocketEncoding(socket->name())) {
                    format errorMsg(
                        "Destination field of move slot of bus %1% does not"
                        "have an encoding for socket %2%.");
                    errorMsg % bus.name() % socket->name();
                    errorMessages_.push_back(errorMsg.str());
                } else {
                    SocketEncoding& socketEnc = dstField.socketEncoding(
                        socket->name());
                    if (needsSocketCodeTable(socketEnc)) {
                        if (!socketEnc.hasSocketCodes()) {
                            format errorMsg(
                                "Encoding of socket %1% in destination "
                                "field of move slot %2% does not refer to "
                                "any socket code table.");
                            errorMsg % socket->name() % bus.name();
                            errorMessages_.push_back(errorMsg.str());
                        } else {
                            checkSocketCodeTable(socketEnc);
                        }
                    }
                }
            }
        }
    }
}


/**
 * Checks the guard field of the given bus for errors.
 *
 * If errors are found, inserts the error messages to the vector of error
 * messages.
 *
 * @param bus The bus.
 */
void
BEMValidator::checkGuardField(const TTAMachine::Bus& bus) {

    MoveSlot& slot = bem_.moveSlot(bus.name());

    if (bus.guardCount() < 2) {
        return;
    } else if (!slot.hasGuardField()) {
        format errorMsg("Move slot %1% does not contain guard field.");
        errorMsg % bus.name();
        errorMessages_.push_back(errorMsg.str());
        return;
    }

    GuardField& grdField = slot.guardField();

    for (int i = 0; i < bus.guardCount(); i++) {
        Guard* guard = bus.guard(i);
        UnconditionalGuard* ucGuard = 
            dynamic_cast<UnconditionalGuard*>(guard);
        PortGuard* portGuard = dynamic_cast<PortGuard*>(guard);
        RegisterGuard* regGuard = dynamic_cast<RegisterGuard*>(guard);
        if (ucGuard != NULL && 
            !grdField.hasUnconditionalGuardEncoding(ucGuard->isInverted())) {
            format errorMsg(
                "Guard field of move slot %1% does not have encoding "
                "for always-%2% guard.");
            errorMsg % bus.name();
            if (ucGuard->isInverted()) {
                errorMsg % "false";
            } else {
                errorMsg % "true";
            }
            errorMessages_.push_back(errorMsg.str());
        
        } else if (portGuard != NULL) {
            FUPort* port = portGuard->port();
            FunctionUnit* fu = port->parentUnit();
            if (!grdField.hasFUGuardEncoding(
                    fu->name(), port->name(), portGuard->isInverted())) {
                format errorMsg(
                    "Guard field of move slot %1% does not have encoding "
                    "for %2% FU port guard of port %3% of FU %4%.");
                errorMsg % bus.name();
                if (portGuard->isInverted()) {
                    errorMsg % "inverted";
                } else {
                    errorMsg % "non-inverted";
                }
                errorMsg % port->name() % fu->name();
                errorMessages_.push_back(errorMsg.str());
            }
        } else if (regGuard != NULL) {
            string regFile = regGuard->registerFile()->name();
            if (!grdField.hasGPRGuardEncoding(
                    regFile, regGuard->registerIndex(), 
                    regGuard->isInverted())) {
                format errorMsg(
                    "Guard field of move slot %1% does not have encoding "
                    "for %2% GPR guard of register %3% of register file "
                    "%4%.");
                errorMsg % bus.name();
                if (regGuard->isInverted()) {
                    errorMsg % "inverted";
                } else {
                    errorMsg % "non-inverted";
                }
                errorMsg % regGuard->registerIndex() % regFile;
                errorMessages_.push_back(errorMsg.str());
            }
        }
    }
}   


/**
 * Checks that the socket code table of the given socket encoding is valid.
 *
 * If errors are found, inserts the error messages to the vector of error
 * messages.
 *
 * @param socketEnc The socket encoding.
 */
void
BEMValidator::checkSocketCodeTable(const SocketEncoding& socketEnc) {

    SlotField* slotField = socketEnc.parent();
    bool srcField = (dynamic_cast<SourceField*>(slotField) != NULL);
    string busName = slotField->parent()->name();
    string socketName = socketEnc.socketName();

    assert(socketEnc.hasSocketCodes());
    SocketCodeTable& table = socketEnc.socketCodes();
    Machine::SocketNavigator socketNav = machine_.socketNavigator();
    assert(socketNav.hasItem(socketName));
    Socket* socket = socketNav.item(socketName);

    for (int i = 0; i < socket->portCount(); i++) {
        Port* port = socket->port(i);
        Unit* parentUnit = port->parentUnit();
        FunctionUnit* fu = dynamic_cast<FunctionUnit*>(parentUnit);
        RegisterFile* rf = dynamic_cast<RegisterFile*>(parentUnit);
        ImmediateUnit* iu = dynamic_cast<ImmediateUnit*>(parentUnit);

        if (fu != NULL) {
            BaseFUPort* fuPort = dynamic_cast<BaseFUPort*>(port);
            assert(fuPort != NULL);
            if (fuPort->isOpcodeSetting()) {
                for (int i = 0; i < fu->operationCount(); i++) {
                    HWOperation* operation = fu->operation(i);
                    if (!table.hasFUPortCode(
                            fu->name(), port->name(), operation->name())) {
                        format errorMsg(
                            "Socket code table of socket %1% in %2% field "
                            "of move slot %3% does not contain FU port code "
                            "for operation %4% for port %5% of FU %6%.");
                        errorMsg % socketEnc.socketName();
                        if (srcField) {
                            errorMsg % "source";
                        } else {
                            errorMsg % "destination";
                        }
                        errorMsg % busName % operation->name() % 
                            port->name() % fu->name();
                        errorMessages_.push_back(errorMsg.str());
                    }
                }
            } else {
                if (!table.hasFUPortCode(fu->name(), port->name())) {
                    format errorMsg(
                        "Socket code table of socket %1% in %2% field of "
                        "move slot %3% does not contain FU port code for "
                        "port %4% of FU %5%.");
                    errorMsg % socketEnc.socketName();
                    if (srcField) {
                        errorMsg % "source";
                    } else {
                        errorMsg % "destination";
                    }
                    errorMsg % busName % port->name() % fu->name();
                    errorMessages_.push_back(errorMsg.str());
                }
            }

        } else if (rf != NULL && iu == NULL 
                              && !table.hasRFPortCode(rf->name())) {
            format errorMsg(
                "Socket code table of socket %1% in %2% field of move "
                "slot %3% does not contain RF port code for RF %4%.");
            errorMsg % socketName;
            if (srcField) {
                errorMsg % "source";
            } else {
                errorMsg % "destination";
            }
            errorMsg % busName % rf->name();
            errorMessages_.push_back(errorMsg.str());
        } else if (iu != NULL && !table.hasIUPortCode(iu->name())) {
            format errorMsg(
                "Socket code table of socket %1% in %2% field of move slot "
                "%3% does not contain IU port code for IU %4%.");
            errorMsg % socketName;
            if (srcField) {
                errorMsg % "source";
            } else {
                errorMsg % "destination";
            }
            errorMsg % busName % iu->name();
            errorMessages_.push_back(errorMsg.str());
        }
    }
}


/**
 * Checks that the BEM contains a valid immediate slot field for the given
 * immediate slot.
 *
 * If errors are found, inserts the error messages to the vector of error
 * messages.
 *
 * @param immSlot The immediate slot.
 */
void
BEMValidator::checkImmediateSlot(const TTAMachine::ImmediateSlot& immSlot) {

    if (immSlot.width() == 0) {
        return;
    }

    if (!bem_.hasImmediateSlot(immSlot.name())) {
        format errorMsg(
            "BEM does not contain field for immediate slot %1%.");
        errorMsg % immSlot.name();
        errorMessages_.push_back(errorMsg.str());
    } else {
        ImmediateSlotField& field = bem_.immediateSlot(immSlot.name());
        if (field.width() < immSlot.width()) {
            format errorMsg("Immediate slot %1% is too narrow in BEM.");
            errorMessages_.push_back(errorMsg.str());
        }
    }
}


/**
 * Checks that the BEM contains a valid immediate control field.
 *
 * If errors are found, inserts the error messages to the vector of error
 * messages.
 */
void
BEMValidator::checkImmediateControlField() {
    Machine::InstructionTemplateNavigator iTempNav = 
        machine_.instructionTemplateNavigator();
    if (iTempNav.count() > 1) {
        if (!bem_.hasImmediateControlField()) {
            string errorMsg("BEM does not contain immediate control field.");
            errorMessages_.push_back(errorMsg);
        } else {
            ImmediateControlField& field = bem_.immediateControlField();
            for (int i = 0; i < iTempNav.count(); i++) {
                InstructionTemplate* iTemp = iTempNav.item(i);
                if (!field.hasTemplateEncoding(iTemp->name())) {
                    format errorMsg(
                        "Immediate control field does not have encoding "
                        "for instruction template %1%.");
                    errorMsg % iTemp->name();
                    errorMessages_.push_back(errorMsg.str());
                }
            }
        }
    }
}


/**
 * Checks that the BEM contains valid long immediate destination register
 * fields.
 *
 * If errors are found, inserts the error messages to the vector of error
 * messages.
 */
void
BEMValidator::checkLImmDstRegisterFields() {

    Machine::InstructionTemplateNavigator itNav = 
        machine_.instructionTemplateNavigator();
    Machine::ImmediateUnitNavigator iuNav = 
        machine_.immediateUnitNavigator();

    for (int i = 0; i < itNav.count(); i++) {

        InstructionTemplate* iTemp = itNav.item(i);
        typedef std::set<std::string> StringSet;

        StringSet iTempDestinations;
        for (int i = 0; i < iuNav.count(); i++) {
            ImmediateUnit* iu = iuNav.item(i);
            if (iTemp->isOneOfDestinations(*iu) &&
                iu->numberOfRegisters() > 1) {
                iTempDestinations.insert(iu->name());
            }
        }
        
        StringSet destinationsInBEM;
        for (int i = 0; i < bem_.longImmDstRegisterFieldCount(); i++) {
            LImmDstRegisterField& field = bem_.longImmDstRegisterField(i);
            if (field.usedByInstructionTemplate(iTemp->name())) {
                string dstIU = field.immediateUnit(iTemp->name());
                destinationsInBEM.insert(dstIU);
                if (AssocTools::containsKey(iTempDestinations, dstIU)) {
                    ImmediateUnit* iu = iuNav.item(dstIU);
                    int regIndexWidth = MathTools::bitLength(
                        iu->numberOfRegisters() - 1);
                    if (field.width() < regIndexWidth) {
                        format errorMsg(
                            "Long immediate destination register field is "
                            "too narrow for destination %1% in instruction "
                            "template %2%.");
                        errorMsg % dstIU % iTemp->name();
                        errorMessages_.push_back(errorMsg.str());
                    }
                }
            }
        }

        StringSet intersection;
        SetTools::intersection(
            iTempDestinations, destinationsInBEM, intersection);
        if (intersection.size() < iTempDestinations.size()) {
            format errorMsg(
                "Long immediate destination register fields do not "
                "cover all the destinations of instruction template %1%.");
            errorMsg % iTemp->name();
            errorMessages_.push_back(errorMsg.str());
        }
    }
}
        
/**
 * Checks that the instruction memory width (MAU) is greater or equal than
 * the width of one instruction.
 *
 * @param gcu Global Control Unit of the machine
 */
void
BEMValidator::checkImemMauWidth(TTAMachine::ControlUnit& gcu) {
    int imemWidth = 0;
    if (gcu.hasAddressSpace()) {
        imemWidth = gcu.addressSpace()->width();
    } else {
        string errorMsg("GCU does not have an address space");
        errorMessages_.push_back(errorMsg);
    }
    int instructionWidth = bem_.width();
    
    if (imemWidth < instructionWidth) {
        string warningMsg(
            "Warning: Instruction width is greater than the instruction "
            "memory width.");
        warningMessages_.push_back(warningMsg);
    }
}

/**
 * Tells whether the given move slot needs source field.
 *
 * @param slot The move slot.
 * @return True if the move slot needs source field, otherwise false.
 */
bool
BEMValidator::needsSourceField(const MoveSlot& slot) const {
    
    string busName = slot.name();
    Machine::BusNavigator busNav = machine_.busNavigator();
    assert(busNav.hasItem(busName));
    Bus* bus = busNav.item(busName);
    
    // check output sockets
    for (int i = 0; i < bus->segmentCount(); i++) {
        Segment* segment = bus->segment(i);
        for (int i = 0; i < segment->connectionCount(); i++) {
            Socket* socket = segment->connection(i);
            if (socket->portCount() > 0 &&
                socket->direction() == Socket::OUTPUT) {
                return true;
            }
        }
    }

    // check bridges
    if (bus->hasNextBus()) {
        Bus* nextBus = bus->nextBus();
        if (bus->canRead(*nextBus)) {
            return true;
        }
    }

    if (bus->hasPreviousBus()) {
        Bus* prevBus = bus->previousBus();
        if (bus->canRead(*prevBus)) {
            return true;
        }
    }

    // check immediate
    if (bus->immediateWidth() > 0) {
        return true;
    }

    return false;
}   
    

/**
 * Tells whether the given socket encoding needs a socket code table.
 *
 * @param socketEnc The socket encoding.
 * @return True if the socket encoding needs a socket code table, otherwise
 *         false.
 */
bool
BEMValidator::needsSocketCodeTable(const SocketEncoding& socketEnc) const {

    string socketName = socketEnc.socketName();
    Machine::SocketNavigator socketNav = machine_.socketNavigator();
    assert(socketNav.hasItem(socketName));
    Socket* socket = socketNav.item(socketName);

    if (socket->portCount() > 1) {
        return true;
    } else if (socket->portCount() == 0) {
        return false;
    }

    Port* port = socket->port(0);
    Unit* unit = port->parentUnit();
    BaseRegisterFile* rf = dynamic_cast<BaseRegisterFile*>(unit);
    if (rf != NULL && rf->numberOfRegisters() > 1) {
        return true;
    }

    FunctionUnit* fu = dynamic_cast<FunctionUnit*>(unit);
    if (fu != NULL) {
        BaseFUPort* fuPort = dynamic_cast<BaseFUPort*>(port);
        assert(fuPort != NULL);
        if (fuPort->isOpcodeSetting() && fu->operationCount() > 1) {
            return true;
        }
    }

    return false;
}
