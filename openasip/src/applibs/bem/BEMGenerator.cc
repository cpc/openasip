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
 * @file BEMGenerator.cc
 *
 * Implementation of BEMGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <set>
#include <map>
#include "BEMGenerator.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "ImmediateSlotField.hh"
#include "ImmediateControlField.hh"
#include "LImmDstRegisterField.hh"
#include "DestinationField.hh"
#include "SourceField.hh"
#include "GuardField.hh"
#include "SocketEncoding.hh"
#include "BridgeEncoding.hh"
#include "ImmediateEncoding.hh"
#include "FUGuardEncoding.hh"
#include "GPRGuardEncoding.hh"
#include "UnconditionalGuardEncoding.hh"
#include "NOPEncoding.hh"
#include "SocketCodeTable.hh"
#include "FUPortCode.hh"
#include "RFPortCode.hh"
#include "IUPortCode.hh"
#include "MathTools.hh"
#include "Machine.hh"
#include "Segment.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "Guard.hh"
#include "MapTools.hh"
#include "AssocTools.hh"
#include "TemplateSlot.hh"
#include "RISCVFields.hh"
#include "InstructionFormat.hh"
#include "OperationTriggeredFormat.hh"
#include "OperationTriggeredEncoding.hh"
#include "OperationTriggeredField.hh"
#include "TCEString.hh"

using std::string;
using std::pair;
using std::set;
using std::vector;
using std::multiset;
using std::map;
using namespace TTAMachine;

/**
 * The constructor.
 *
 * @param machine The machine for which the binary encoding map will be
 *                generated.
 */
BEMGenerator::BEMGenerator(const Machine& machine) : 
    machine_(&machine) {
}

/**
 * BEM constructor for the Coprocessor generation.
 *
 * @param machine The machine for which the binary encoding map will be
 *                generated.
 * @param coproInterF Bool for enabling the ROCC format
 */
BEMGenerator::BEMGenerator(const Machine& machine, bool coproInterF)
    : machine_(&machine), rocc_(coproInterF) {}

/**
 * The destructor.
 */
BEMGenerator::~BEMGenerator() {
}


/**
 * Generates the binary encoding map.
 *
 * @return The newly created binary encoding map.
 */
BinaryEncoding*
BEMGenerator::generate() {
    
    BinaryEncoding* bem = new BinaryEncoding();
    
    addSocketCodeTables(*bem);
    addTopLevelFields(*bem);    
    for (int i = 0; i < bem->moveSlotCount(); i++) {
        MoveSlot& slot = bem->moveSlot(i);
         addSubfields(slot);
    }

    return bem;
}


/**
 * Adds the socket code tables to the given binary encoding map.
 *
 * @param bem The binary encoding map.
 */
void
BEMGenerator::addSocketCodeTables(BinaryEncoding& bem) {
    Machine::SocketNavigator socketNav = machine_->socketNavigator();
    for (int i = 0; i < socketNav.count(); i++) {
        Socket* socket = socketNav.item(i);
        if (needsSocketCodeTable(*socket)) {
            SocketCodeTable* suitable = suitableSocketCodeTable(*socket);
            if (suitable != NULL) {
                assignSocketCodeTable(socket, suitable);
            } else {
                SocketCodeTable* table = 
                    new SocketCodeTable(socket->name(), bem);
                addPortCodes(*table, *socket);
                assignSocketCodeTable(socket, table);
            }
        }
    }
}


/**
 * Adds the top-level fields to the given binary encoding map.
 *
 * @param bem The binary encoding to be modified.
 */
void
BEMGenerator::addTopLevelFields(BinaryEncoding& bem) const {
    
    // add immediate slots
    Machine::ImmediateSlotNavigator isNav = 
        machine_->immediateSlotNavigator();
    for (int i = 0; i < isNav.count(); i++) {
        ImmediateSlot* slot = isNav.item(i);
        int width = slot->width();
        if (width > 0) {
            new ImmediateSlotField(slot->name(), width, bem);
        }
    }

    // add move slots
    Machine::BusNavigator busNav = machine_->busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        new MoveSlot(bus->name(), bem);
    }

    // add long immediate destination register fields
    addLongImmDstRegisterFields(bem);

    // add long immediate control field
    Machine::InstructionTemplateNavigator itNav = 
        machine_->instructionTemplateNavigator();

    if (itNav.count() > 1) {
        ImmediateControlField* field = new ImmediateControlField(bem);
        addEncodings(*field);
    }

    unsigned amountOfRCustomOps = 0;
    unsigned amountOfR3RCustomOps = 0;

    Machine::OperationTriggeredFormatNavigator fNav =
        machine_->operationTriggeredFormatNavigator();

    // For CVX-IF, make sure these are iterated in the correct order.
    for (int i = 0; i < fNav.count(); i++) {
        OperationTriggeredFormat* fTemp = fNav.item(i);
        addRiscvFormat(fTemp, bem, amountOfRCustomOps, amountOfR3RCustomOps);
    }
}

/**
 * RISC-V F7,F3 encoding generation
 */

void
BEMGenerator::funcencodeRiscv(
    OperationTriggeredFormat* format, InstructionFormat* instrFormat,
    const unsigned& custom_op, unsigned& amountOfRCustomOps,
    unsigned& rocc_f3) const {
    for (int i = 0; i < format->operationCount(); i++) {
        const std::string op = format->operationAtIndex(i);
        if (MapTools::containsKey(RISCVFields::RISCVRTypeOperations, op)) {
            instrFormat->addOperation(
                op, RISCVFields::RISCVRTypeOperations.at(op));
        } else {
            if (rocc_) {  // For the ROCC interface F3=rocc_f3, F7 in
                          // incrementals
                unsigned int customEncoding = (rocc_f3 << 7) + custom_op;
                customEncoding += (amountOfRCustomOps << 10);
                amountOfRCustomOps++;
                // 10 bit encoding for operation
                assert(amountOfRCustomOps < 1024);
                instrFormat->addOperation(op, customEncoding);
            } else {  // F7:F3 in incrementals
                unsigned int customEncoding = custom_op;
                customEncoding += (amountOfRCustomOps << 7);
                amountOfRCustomOps++;
                // 10 bit encoding for operation
                assert(amountOfRCustomOps < 1024);
                instrFormat->addOperation(op, customEncoding);
            }
        }
    }
}

/**
 * Adds a RISC-V format to the binary encoding map
 *
 * @param format The operation triggered format
 * @param bem The binary encoding
 */

void
BEMGenerator::addRiscvFormat(
    OperationTriggeredFormat* format, BinaryEncoding& bem,
    unsigned& amountOfRCustomOps, unsigned& amountOfR3RCustomOps) const {
    std::string name = format->name();
    InstructionFormat* instrFormat = new InstructionFormat(name, bem);
    const unsigned OPC_CUSTOM_0 = 0b0001011;
    const unsigned OPC_CUSTOM_1 = 0b0101011;
    unsigned OP_ROCC_F3 =
        0b0;  // For ROCC F3 enconding for R type custom instructions

    if (name == RISCVFields::RISCV_R_TYPE_NAME) {
        OperationTriggeredEncoding* rs1 =
            new OperationTriggeredEncoding(std::string("rs1"), *instrFormat);
        OperationTriggeredEncoding* rs2 =
            new OperationTriggeredEncoding(std::string("rs2"), *instrFormat);
        OperationTriggeredEncoding* rd =
            new OperationTriggeredEncoding(std::string("rd"), *instrFormat);
        new OperationTriggeredField(*rs1, 0, 15, 5);
        new OperationTriggeredField(*rs2, 0, 20, 5);
        new OperationTriggeredField(*rd, 0, 7, 5);
        OperationTriggeredEncoding* opcode = new OperationTriggeredEncoding(
            std::string("opcode"), *instrFormat);
        new OperationTriggeredField(*opcode, 0, 0, 7);
        new OperationTriggeredField(*opcode, 1, 12, 3);
        new OperationTriggeredField(*opcode, 2, 25, 7);
        OP_ROCC_F3 = 0b111;
        // Encoding generation
        funcencodeRiscv(
            format, instrFormat, OPC_CUSTOM_0, amountOfR3RCustomOps,
            OP_ROCC_F3);

    } else if (name == RISCVFields::RISCV_I_TYPE_NAME) {
        // TODO: shift operations use immediate bits for funct code in this
        //  format
        OperationTriggeredEncoding* rs1 =
            new OperationTriggeredEncoding(std::string("rs1"), *instrFormat);
        OperationTriggeredEncoding* imm =
            new OperationTriggeredEncoding(std::string("imm"), *instrFormat);
        OperationTriggeredEncoding* rd =
            new OperationTriggeredEncoding(std::string("rd"), *instrFormat);
        new OperationTriggeredField(*rs1, 0, 15, 5);
        new OperationTriggeredField(*imm, 0, 25, 12);
        new OperationTriggeredField(*rd, 0, 7, 5);
        OperationTriggeredEncoding* opcode = new OperationTriggeredEncoding(
            std::string("opcode"), *instrFormat);
        new OperationTriggeredField(*opcode, 0, 0, 7);
        new OperationTriggeredField(*opcode, 1, 12, 3);
        new OperationTriggeredField(*opcode, 2, 25, 7);
        for (int i = 0; i < format->operationCount(); i++) {
            const std::string op = format->operationAtIndex(i);
            if (MapTools::containsKey(RISCVFields::RISCVITypeOperations, op)) {
                instrFormat->addOperation(op, RISCVFields::RISCVITypeOperations.at(op));
            } else {
                assert(false);
            }
        }
    } else if (name == RISCVFields::RISCV_S_TYPE_NAME) {
        OperationTriggeredEncoding* rs1 =
            new OperationTriggeredEncoding(std::string("rs1"), *instrFormat);
        OperationTriggeredEncoding* imm =
            new OperationTriggeredEncoding(std::string("imm"), *instrFormat);
        OperationTriggeredEncoding* rs2 =
            new OperationTriggeredEncoding(std::string("rs2"), *instrFormat);
        new OperationTriggeredField(*rs1, 0, 15, 5);
        new OperationTriggeredField(*rs2, 0, 20, 5);
        new OperationTriggeredField(*imm, 0, 7, 5);
        new OperationTriggeredField(*imm, 1, 25, 7);
        OperationTriggeredEncoding* opcode = new OperationTriggeredEncoding(
            std::string("opcode"), *instrFormat);
        new OperationTriggeredField(*opcode, 0, 0, 7);
        new OperationTriggeredField(*opcode, 1, 12, 3);
        for (int i = 0; i < format->operationCount(); i++) {
            const std::string op = format->operationAtIndex(i);
            if (MapTools::containsKey(RISCVFields::RISCVSTypeOperations, op)) {
                instrFormat->addOperation(op, RISCVFields::RISCVSTypeOperations.at(op));
            } else {
                assert(false);
            }
        }
    } else if (name == RISCVFields::RISCV_B_TYPE_NAME) {
        OperationTriggeredEncoding* rs1 =
            new OperationTriggeredEncoding(std::string("rs1"), *instrFormat);
        OperationTriggeredEncoding* imm =
            new OperationTriggeredEncoding(std::string("imm"), *instrFormat);
        OperationTriggeredEncoding* rs2 =
            new OperationTriggeredEncoding(std::string("rs2"), *instrFormat);
        new OperationTriggeredField(*rs1, 0, 15, 5);
        new OperationTriggeredField(*rs2, 0, 20, 5);
        new OperationTriggeredField(*imm, 0, 8, 4);
        new OperationTriggeredField(*imm, 1, 25, 6);
        new OperationTriggeredField(*imm, 2, 11, 1);
        new OperationTriggeredField(*imm, 3, 31, 1);
        OperationTriggeredEncoding* opcode = new OperationTriggeredEncoding(
            std::string("opcode"), *instrFormat);
        new OperationTriggeredField(*opcode, 0, 0, 7);
        new OperationTriggeredField(*opcode, 1, 12, 3);
        for (int i = 0; i < format->operationCount(); i++) {
            const std::string op = format->operationAtIndex(i);
            if (MapTools::containsKey(RISCVFields::RISCVBTypeOperations, op)) {
                instrFormat->addOperation(op, RISCVFields::RISCVBTypeOperations.at(op));
            } else {
                assert(false);
            }
        }
    } else if (name == RISCVFields::RISCV_U_TYPE_NAME) {
        OperationTriggeredEncoding* rd =
            new OperationTriggeredEncoding(std::string("rd"), *instrFormat);
        new OperationTriggeredField(*rd, 0, 7, 5);
        OperationTriggeredEncoding* opcode = new OperationTriggeredEncoding(
            std::string("opcode"), *instrFormat);
        OperationTriggeredEncoding* imm =
            new OperationTriggeredEncoding(std::string("imm"), *instrFormat);
        new OperationTriggeredField(*imm, 0, 12, 20);
        new OperationTriggeredField(*opcode, 0, 0, 7);
        for (int i = 0; i < format->operationCount(); i++) {
            const std::string op = format->operationAtIndex(i);
            if (MapTools::containsKey(RISCVFields::RISCVUTypeOperations, op)) {
                instrFormat->addOperation(op, RISCVFields::RISCVUTypeOperations.at(op));
            } else {
                assert(false);
            }
        }
    } else if (name == RISCVFields::RISCV_J_TYPE_NAME) {
        OperationTriggeredEncoding* rd =
            new OperationTriggeredEncoding(std::string("rd"), *instrFormat);
        new OperationTriggeredField(*rd, 0, 7, 5);
        OperationTriggeredEncoding* opcode = new OperationTriggeredEncoding(
            std::string("opcode"), *instrFormat);
        OperationTriggeredEncoding* imm =
            new OperationTriggeredEncoding(std::string("imm"), *instrFormat);
        new OperationTriggeredField(*imm, 0, 20, 10);
        new OperationTriggeredField(*imm, 1, 20, 1);
        new OperationTriggeredField(*imm, 2, 15, 8);
        new OperationTriggeredField(*imm, 3, 31, 1);
        new OperationTriggeredField(*opcode, 0, 0, 7);
        for (int i = 0; i < format->operationCount(); i++) {
            const std::string op = format->operationAtIndex(i);
            if (MapTools::containsKey(RISCVFields::RISCVJTypeOperations, op)) {
                instrFormat->addOperation(op, RISCVFields::RISCVJTypeOperations.at(op));
            } else {
                assert(false);
            }
        }
        // This is a custom format for unary operations
        // but it still uses R-format with rs2 = x0
    } else if (name == RISCVFields::RISCV_R1R_TYPE_NAME) {
        OperationTriggeredEncoding* rs1 =
            new OperationTriggeredEncoding(std::string("rs1"), *instrFormat);
        OperationTriggeredEncoding* rd =
            new OperationTriggeredEncoding(std::string("rd"), *instrFormat);
        new OperationTriggeredField(*rs1, 0, 15, 5);
        new OperationTriggeredField(*rd, 0, 7, 5);
        OperationTriggeredEncoding* opcode = new OperationTriggeredEncoding(
            std::string("opcode"), *instrFormat);
        new OperationTriggeredField(*opcode, 0, 0, 7);
        new OperationTriggeredField(*opcode, 1, 12, 3);
        new OperationTriggeredField(*opcode, 2, 25, 7);
        OP_ROCC_F3 = 0b110;

        funcencodeRiscv(
            format, instrFormat, OPC_CUSTOM_0, amountOfR3RCustomOps,
            OP_ROCC_F3);
        /*
        for (int i = 0; i < format->operationCount(); i++) {
            const std::string op = format->operationAtIndex(i);
            unsigned int customEncoding = OPC_CUSTOM_0;
            customEncoding += (amountOfRCustomOps << 7);
            amountOfRCustomOps++;
            // 10 bit encoding for operation
            assert(amountOfRCustomOps < 1024);
            instrFormat->addOperation(op, customEncoding);
        }*/
        // unary without output, stdout for example
    } else if (name == RISCVFields::RISCV_R1_TYPE_NAME) {
        OperationTriggeredEncoding* rs1 =
            new OperationTriggeredEncoding(std::string("rs1"), *instrFormat);
        new OperationTriggeredField(*rs1, 0, 15, 5);
        OperationTriggeredEncoding* opcode = new OperationTriggeredEncoding(
            std::string("opcode"), *instrFormat);
        new OperationTriggeredField(*opcode, 0, 0, 7);
        new OperationTriggeredField(*opcode, 1, 12, 3);
        new OperationTriggeredField(*opcode, 2, 25, 7);

        for (int i = 0; i < format->operationCount(); i++) {
            const std::string op = format->operationAtIndex(i);
            unsigned int customEncoding = OPC_CUSTOM_0;
            // Reserve this for printing
            if (TCEString(op).lower() != "stdout") {
                customEncoding += (amountOfRCustomOps << 7);
                amountOfRCustomOps++;
            }
            // 10 bit encoding for operation
            assert(amountOfRCustomOps < 1024);
            instrFormat->addOperation(op, customEncoding);
        }
    } else if (name == RISCVFields::RISCV_R3R_TYPE_NAME) {
        OperationTriggeredEncoding* rs1 =
            new OperationTriggeredEncoding(std::string("rs1"), *instrFormat);
        OperationTriggeredEncoding* rs2 =
            new OperationTriggeredEncoding(std::string("rs2"), *instrFormat);
        OperationTriggeredEncoding* rs3 =
            new OperationTriggeredEncoding(std::string("rs3"), *instrFormat);
        OperationTriggeredEncoding* rd =
            new OperationTriggeredEncoding(std::string("rd"), *instrFormat);
        new OperationTriggeredField(*rs1, 0, 15, 5);
        new OperationTriggeredField(*rs2, 0, 20, 5);
        new OperationTriggeredField(*rs3, 0, 27, 5);
        new OperationTriggeredField(*rd, 0, 7, 5);
        OperationTriggeredEncoding* opcode = new OperationTriggeredEncoding(
            std::string("opcode"), *instrFormat);
        new OperationTriggeredField(*opcode, 0, 0, 7);
        new OperationTriggeredField(*opcode, 1, 12, 3);
        new OperationTriggeredField(*opcode, 2, 25, 2);

        // Reserve first few encodings for fixed special case
        for (int i = 0; i < format->operationCount(); i++) {
            const std::string op = format->operationAtIndex(i);
            if (MapTools::containsKey(RISCVFields::RISCVRTypeOperations, op)) {
                instrFormat->addOperation(op, RISCVFields::RISCVRTypeOperations.at(op));
            } else {
                unsigned int customEncoding = OPC_CUSTOM_1;
                customEncoding += (amountOfR3RCustomOps << 7);
                amountOfR3RCustomOps++;
                assert(amountOfR3RCustomOps < 32);
                instrFormat->addOperation(op, customEncoding);
            }
        }
    }  else {
        // TODO: Throw some meaniningful exception here
        assert(false);
    }
}

/**
 * Adds the long immediate destination register fields to the given
 * binary encoding map.
 *
 * @param bem The binary encoding map.
 */
void
BEMGenerator::addLongImmDstRegisterFields(BinaryEncoding& bem) const {
    
    // check how many fields is needed
    Machine::InstructionTemplateNavigator itNav = 
        machine_->instructionTemplateNavigator();
    int fieldCount(0);
    for (int i = 0; i < itNav.count(); i++) {
        InstructionTemplate* iTemp = itNav.item(i);
        int thisRequires = 0;
        Machine::ImmediateUnitNavigator iuNav = 
            machine_->immediateUnitNavigator();
        for (int i = 0; i < iuNav.count(); i++) {
            ImmediateUnit* iu = iuNav.item(i);
            if (iu->numberOfRegisters() > 1 && 
                iTemp->isOneOfDestinations(*iu)) {
                thisRequires++;
            }
        }
                
        if (thisRequires > fieldCount) {
            fieldCount = thisRequires;
        }
    }

    // create a vector that contains the required widths of the fields
    Machine::ImmediateUnitNavigator iuNav = 
        machine_->immediateUnitNavigator();
    std::vector<int> fieldWidths(fieldCount, 0);

    for (int i = 0; i < itNav.count(); i++) {
        InstructionTemplate* iTemp = itNav.item(i);

        // collect the destinations units to a set
        std::set<ImmediateUnit*> dstUnits;
        for (int i = 0; i < iuNav.count(); i++) {
            ImmediateUnit* iu = iuNav.item(i);
            if (iTemp->isOneOfDestinations(*iu)) {
                dstUnits.insert(iu);
            }
        }

        // create a set containing the sizes required by all the destinations
        // of the instruction template
        std::multiset<int> requiredSizes;
        for (set<ImmediateUnit*>::const_iterator iter = dstUnits.begin();
             iter != dstUnits.end(); iter++) {
            ImmediateUnit* iu = *iter;
            if (iu->numberOfRegisters() > 1) {
                requiredSizes.insert(
                    MathTools::bitLength(iu->numberOfRegisters() - 1));
            }
        }

        // increase the values in fieldWidhts if required
        int counter(0);
        for (multiset<int>::const_reverse_iterator iter = 
                 requiredSizes.rbegin(); 
             iter != requiredSizes.rend(); iter++) {
            if (fieldWidths[counter] < *iter) {
                fieldWidths[counter] = *iter;
            }
            counter++;
        }
    }

    // create the fields
    typedef std::pair<InstructionTemplate*, ImmediateUnit*> ImmDstMapping;
    std::set<ImmDstMapping> mappedDestinations;
    for (int i = fieldCount-1; i >= 0; i--) {
        unsigned int fieldWidth = fieldWidths[i];
        LImmDstRegisterField* newField = new LImmDstRegisterField(
            fieldWidth, bem);
        for (int i = 0; i < itNav.count(); i++) {
            InstructionTemplate* iTemp = itNav.item(i);
            for (int i = 0; i < iuNav.count(); i++) {
                ImmediateUnit* iu = iuNav.item(i);
                if (iTemp->isOneOfDestinations(*iu) && 
                    iu->numberOfRegisters() > 1 &&
                    static_cast<unsigned int>(
                        MathTools::bitLength(iu->numberOfRegisters() - 1))
                    <= fieldWidth &&
                    !AssocTools::containsKey(
                        mappedDestinations, 
                        ImmDstMapping(iTemp, iu))) {
                    newField->addDestination(iTemp->name(), iu->name());
                    mappedDestinations.insert(ImmDstMapping(iTemp, iu));
                }
            }
        }
    }                      
}


/**
 * Adds subfields to the given move slot.
 *
 * @param slot The move slot.
 */
void
BEMGenerator::addSubfields(MoveSlot& slot) const {
    
    string busName = slot.name();
    Machine::BusNavigator busNav = machine_->busNavigator();
    assert(busNav.hasItem(busName));
    Bus* bus = busNav.item(busName);

    DestinationField* dField = new DestinationField(
        BinaryEncoding::LEFT, slot);
    SourceField* sField = new SourceField(BinaryEncoding::LEFT, slot);
    GuardField* gField = NULL;
        
    addEncodings(*dField); // destination field
    addEncodings(*sField); // source field

    // we need guard field only if we have >1 guard.
    if (bus->guardCount() > 1) {
        gField = new GuardField(slot);
        addEncodings(*gField); // guard field
    }
    
    // adds extra bits to guard field (any field is probably ok) if needed.
    // extra bits added so that long immediate fits to the slot.
    int longImmWidth = maxLongImmSlotWidth(slot);
    if (longImmWidth > slot.width()) {
        // if no guard field, create it so that it can be enlarged.
        if (gField == NULL) {
            gField = new GuardField(slot);
            addEncodings(*gField); // guard field
        }
        gField->setExtraBits((longImmWidth - slot.width()) + 
            gField->extraBits());
    }
}


/**
 * Adds the encoding for instruction templates to the given immediate
 * control field.
 *
 * @param field The immediate control field.
 */
void
BEMGenerator::addEncodings(ImmediateControlField& field) const {
    Machine::InstructionTemplateNavigator itNav =
        machine_->instructionTemplateNavigator();
    for (int i = 0; i < itNav.count(); i++) {
        InstructionTemplate* iTemp = itNav.item(i);
        field.addTemplateEncoding(iTemp->name(), i);
    }
}


/**
 * Adds encodings for the given destination field.
 *
 * @param field The destination field.
 */
void
BEMGenerator::addEncodings(DestinationField& field) const {

    string busName = field.parent()->name();
    Machine::BusNavigator busNav = machine_->busNavigator();
    assert(busNav.hasItem(busName));
    Bus* bus = busNav.item(busName);
    bool createNopField = true;
    for (int i = 0; i < bus->guardCount(); i++) {
	Guard* g = bus->guard(i);
	if (g->isInverted() && dynamic_cast<UnconditionalGuard*>(g) != NULL) {
	    createNopField = false;
	}
    }

    int dstSockets = socketCount(*bus, Socket::INPUT);
    if (dstSockets == 0) {
        return;
    }

    multiset<int> socketCodeWidths = socketCodeWidthsForBus(
        *bus, Socket::INPUT);
    // add socket code width for NOP encoding
    if (createNopField) {
        socketCodeWidths.insert(0);
    }
    multiset<Encoding> encodings;
    calculateEncodings(socketCodeWidths, true, encodings);
    
    set<Socket*> handledSockets;
    multiset<int>::reverse_iterator scIter = socketCodeWidths.rbegin();
    bool nopEncodingSet = false;
    for (multiset<Encoding>::const_iterator encIter = encodings.begin();
         encIter != encodings.end(); encIter++) {
        Encoding enc = *encIter;
        int scWidth = *scIter;
        if (scWidth == 0 && !nopEncodingSet && createNopField) {
            // add NOP encoding
            new NOPEncoding(enc.first, enc.second, field);
            nopEncodingSet = true;
        } else {
            // find socket that has correct socket code width
            for (int i = 0; i < dstSockets; i++) {
                Socket& socket = BEMGenerator::socket(
                    i, *bus, Socket::INPUT);
                SocketCodeTable* scTable = socketCodeTable(socket);
                if (scTable == NULL && scWidth == 0 && 
                    !AssocTools::containsKey(handledSockets, &socket)) {
                    new SocketEncoding(
                        socket.name(), enc.first, enc.second, field);
                    handledSockets.insert(&socket);
                    break;
                } else if (scTable != NULL && scWidth == scTable->width() &&
                           !AssocTools::containsKey(
                               handledSockets, &socket)) {
                    SocketEncoding* socketEnc = new SocketEncoding(
                        socket.name(), enc.first, enc.second, field);
                    socketEnc->setSocketCodes(*scTable);
                    handledSockets.insert(&socket);
                    break;
                }   
            }
        }
        scIter++;
    }      
}


/**
 * Adds encodings for the given source field.
 *
 * @param field The source field.
 */
void
BEMGenerator::addEncodings(SourceField& field) const {

    string busName = field.parent()->name();
    Machine::BusNavigator busNav = machine_->busNavigator();
    assert(busNav.hasItem(busName));
    Bus* bus = busNav.item(busName);
    bool createNopField = true;
    for (int i = 0; i < bus->guardCount(); i++) {
	Guard* g = bus->guard(i);
	if (g->isInverted() && dynamic_cast<UnconditionalGuard*>(g) != NULL) {
	    createNopField = false;
	}
    }

    int srcSockets = socketCount(*bus, Socket::OUTPUT);
    int srcBridges = sourceBridgeCount(*bus);
    bool shortImmSupport = (bus->immediateWidth() > 0);

    multiset<int> socketCodeWidths = socketCodeWidthsForBus(
        *bus, Socket::OUTPUT);
    for (int i = 0; i < srcBridges; i++) {
        socketCodeWidths.insert(0);
    }
    if (shortImmSupport) {
        socketCodeWidths.insert(bus->immediateWidth());
    }
    // one encoding for NOP
    if (createNopField) {
        socketCodeWidths.insert(0);
    }

    multiset<Encoding> encodings;
    calculateEncodings(socketCodeWidths, true, encodings);

    // set the encodings
    set<Socket*> handledSockets;
    int nextBridge = 0;
    bool nopEncodingSet = false;
    bool immEncodingSet = !shortImmSupport;
    multiset<int>::reverse_iterator scIter = socketCodeWidths.rbegin();
    for (multiset<Encoding>::const_iterator encIter = encodings.begin();
         encIter != encodings.end(); encIter++) {
        Encoding enc = *encIter;
        int scWidth = *scIter;
        if (scWidth == 0 && !nopEncodingSet && createNopField) {
            new NOPEncoding(enc.first, enc.second, field);
            nopEncodingSet = true;
        } else if (!immEncodingSet && scWidth == bus->immediateWidth()) {
            new ImmediateEncoding(
                enc.first, enc.second, bus->immediateWidth(), field);
            immEncodingSet = true;
        } else {
            bool socketFound = false;
            // find the socket that has correct socket code width
            for (int i = 0; i < srcSockets; i++) {
                Socket& socket = BEMGenerator::socket(
                    i, *bus, Socket::OUTPUT);
                SocketCodeTable* scTable = socketCodeTable(socket);
                if (scTable == NULL && scWidth == 0 && 
                    !AssocTools::containsKey(handledSockets, &socket)) {
                    new SocketEncoding(
                        socket.name(), enc.first, enc.second, field);
                    handledSockets.insert(&socket);
                    socketFound = true;
                    break;
                } else if (scTable != NULL && scWidth == scTable->width() &&
                           !AssocTools::containsKey(
                               handledSockets, &socket)) {
                    SocketEncoding* socketEnc = new SocketEncoding(
                        socket.name(), enc.first, enc.second, field);
                    socketEnc->setSocketCodes(*scTable);
                    handledSockets.insert(&socket);
                    socketFound = true;
                    break;
                }
            }
            if (!socketFound) {
                assert(scWidth == 0);
                assert(nextBridge < srcBridges);
                Bridge& bridge = BEMGenerator::sourceBridge(
                    nextBridge, *bus);
                new BridgeEncoding(
                    bridge.name(), enc.first, enc.second, field);
                nextBridge++;
            }
        }
        scIter++;
    }
}


/**
 * Adds guard encodings to the given guard field.
 *
 * @param field The guard field.
 */
void
BEMGenerator::addEncodings(GuardField& field) const {

    string busName = field.parent()->name();
    Machine::BusNavigator busNav = machine_->busNavigator();
    assert(busNav.hasItem(busName));
    Bus* bus = busNav.item(busName);

    int guards = bus->guardCount();
    
    for (int i = 0; i < guards; i++) {
        Guard* guard = bus->guard(i);
        PortGuard* portGuard = dynamic_cast<PortGuard*>(guard);
        RegisterGuard* regGuard = dynamic_cast<RegisterGuard*>(guard);
        UnconditionalGuard* ucGuard = 
            dynamic_cast<UnconditionalGuard*>(guard);
        if (portGuard != NULL) {
            FUPort* port = portGuard->port();
            new FUGuardEncoding(
                port->parentUnit()->name(), port->name(),
                portGuard->isInverted(), i, field);
        } else if (regGuard != NULL) {
            new GPRGuardEncoding(
                regGuard->registerFile()->name(), regGuard->registerIndex(),
                regGuard->isInverted(), i, field);
        } else if (ucGuard != NULL) {
            new UnconditionalGuardEncoding(ucGuard->isInverted(), i, field);
        } else {
            assert(false);
        }
    }
}


/**
 * Returns the socket code table that is assigned to the given socket.
 *
 * @param socket The socket.
 * @return The socket code table or NULL if no socket code table is assigned
 *         to the given socket.
 */
SocketCodeTable*
BEMGenerator::socketCodeTable(const Socket& socket) const {
    if (MapTools::containsKey(scTableMap_, &socket)) {
        return MapTools::valueForKey<SocketCodeTable*>(scTableMap_, &socket);
    } else {
        return NULL;
    }
}


/**
 * Finds a suitable socket code table from the socket code table map for
 * the given socket.
 *
 * Returns NULL if there is no suitable socket code table.
 *
 * @param socket The socket.
 * @return The socket code table or NULL.
 */
SocketCodeTable*
BEMGenerator::suitableSocketCodeTable(const Socket& socket) const {
    for (SCTableMap::const_iterator iter = scTableMap_.begin(); 
         iter != scTableMap_.end(); iter++) {
        const Socket* toCheck = (*iter).first;
        if (haveEqualConnections(socket, *toCheck)) {
            return (*iter).second;
        }
    }
    return NULL;
}


/**
 * Assigns the given socket code table for the given socket.
 *
 * @param socket The socket.
 * @param table The socket code table.
 */
void
BEMGenerator::assignSocketCodeTable(
    const Socket* socket,
    SocketCodeTable* table) {

    assert(!MapTools::containsKey(scTableMap_, socket));
    scTableMap_.insert(pair<const Socket*, SocketCodeTable*>(socket, table));
}


/**
 * Returns the number of sockets of the given direction connected to the
 * given bus.
 *
 * @param bus The bus.
 * @param direction The direction
 * @return The number of sockets.
 */
int
BEMGenerator::socketCount(const Bus& bus, Socket::Direction direction) {

    typedef std::set<Socket*> SocketSet;
    SocketSet sockets;

    for (int i = 0; i < bus.segmentCount(); i++) {
        Segment* segment = bus.segment(i);
        for (int i = 0; i < segment->connectionCount(); i++) {
            Socket* socket = segment->connection(i);
            if (socket->direction() == direction) {
                sockets.insert(socket);
            }
        }
    }

    return sockets.size();
}


/**
 * By the given index, returns a socket that is attached to the
 * given bus and has the given direction.
 *
 * @param index The index.
 * @param bus The bus.
 * @param direction Direction of the sockets being returned.
 * @return The socket.
 */
Socket&
BEMGenerator::socket(
    int index,
    const Bus& bus,
    Socket::Direction direction) {

    typedef std::vector<Socket*> SocketTable;

    assert(index >= 0);
    SocketTable connectedSockets;

    for (int i = 0; i < bus.segmentCount(); i++) {
        Segment* segment = bus.segment(i);
        for (int i = 0; i < segment->connectionCount(); i++) {
            Socket* socket = segment->connection(i);
            if (socket->direction() == direction) {
                if (!ContainerTools::containsValue(
                        connectedSockets, socket)) {
                    connectedSockets.push_back(socket);
                }
            }
        }
    }

    assert(connectedSockets.size() > static_cast<size_t>(index));
    return *connectedSockets[index];
}


/**
 * Tells whether the given socket needs a socket code table.
 *
 * @return True if the socket needs a socket code table, otherwise false.
 */
bool
BEMGenerator::needsSocketCodeTable(const Socket& socket) {

    if (socket.portCount() > 1) {
        return true;

    } else if (socket.portCount() == 1) {

        // socket code table is needed if the port is opcode setting with
        // more than one possible opcode or if the port is a data port of
        // register file with more than one register
        Port* port = socket.port(0);
        FUPort* fuPort = dynamic_cast<FUPort*>(port);
        RFPort* rfPort = dynamic_cast<RFPort*>(port);
        if (fuPort != NULL && fuPort->isOpcodeSetting() && 
            fuPort->parentUnit()->operationCount() > 1) {
            return true;
        } else if (rfPort != NULL && 
                   rfPort->parentUnit()->numberOfRegisters() > 1) {
            return true;
        } else {
            return false;
        }

    } else {
        return false;
    }
}


/**
 * Adds the port codes to the given socket code table.
 *
 * @param table The socket code table.
 * @param socket The socket that will refer to the table.
 */
void
BEMGenerator::addPortCodes(
    SocketCodeTable& table,
    const Socket& socket) const {

    // create a set of register index widths for each port code
    multiset<int> indexWidths;
    for (int i = 0; i < socket.portCount(); i++) {
        Port* port = socket.port(i);
        Unit* parentUnit = port->parentUnit();
        BaseRegisterFile* rfParent = dynamic_cast<BaseRegisterFile*>(
            parentUnit);
        FunctionUnit* fuParent = dynamic_cast<FunctionUnit*>(parentUnit); 

        if (fuParent != NULL) {
            int encodingsNeeded(0);
            if (dynamic_cast<BaseFUPort*>(port)->isOpcodeSetting() &&
                socket.direction() == Socket::INPUT) {
                encodingsNeeded = fuParent->operationCount();
            } else {
                encodingsNeeded = 1;
            }
            for (int i = 0; i < encodingsNeeded; i++) {
                indexWidths.insert(0);
            }
        } else {
            assert(rfParent != NULL);
            unsigned int indexWidth = requiredIndexWidth(*rfParent);
            indexWidths.insert(indexWidth);
        }
    }

    // calculate port IDs
    multiset<Encoding> encodings;
    calculateEncodings(indexWidths, true, encodings);
        
    // set the encodings
    if (encodings.size() == 0) {
        assert(socket.portCount() == 1);
        Port* port = socket.port(0);
        ImmediateUnit* iu = dynamic_cast<ImmediateUnit*>(port->parentUnit());
        RegisterFile* rf = dynamic_cast<RegisterFile*>(port->parentUnit());
        assert(iu != NULL || rf != NULL);
        // iu must be first because iu is inherited from rf
        if (iu != NULL) {
            new IUPortCode(iu->name(), requiredIndexWidth(*iu), table);
        } else {
            new RFPortCode(rf->name(), requiredIndexWidth(*rf), table);
        }
    } else {
        set<Port*> handledPorts;
        multiset<int>::const_reverse_iterator indexWidthIter = 
            indexWidths.rbegin();
        for (multiset<Encoding>::const_iterator encIter = encodings.begin();
             encIter != encodings.end();) {
            int indexWidth = *indexWidthIter;
            Encoding enc = *encIter;
            for (int portIndex = 0; portIndex < socket.portCount();
                 portIndex++) {
                Port* port = socket.port(portIndex);
                if (AssocTools::containsKey(handledPorts, port)) {
                    continue;
                }
                RegisterFile* rfParent = dynamic_cast<RegisterFile*>(
                    port->parentUnit());
                ImmediateUnit* iuParent = dynamic_cast<ImmediateUnit*>(
                    port->parentUnit());
                FunctionUnit* fuParent = dynamic_cast<FunctionUnit*>(
                    port->parentUnit());
                // iu must be first because iu is inherited from rf
                if (iuParent != NULL) {
                    int reqIndexWidth = requiredIndexWidth(*iuParent);
                    if (reqIndexWidth == indexWidth) {
                        new IUPortCode(
                            iuParent->name(), enc.first, enc.second,
                            reqIndexWidth, table);
                        indexWidthIter++;
                        encIter++;
                        handledPorts.insert(port);
                        break;
                    }
                } else if (rfParent != NULL) {
                    int reqIndexWidth = requiredIndexWidth(*rfParent);
                    if (reqIndexWidth == indexWidth) {
                        new RFPortCode(
                            rfParent->name(), enc.first, enc.second,
                            reqIndexWidth, table);
                        indexWidthIter++;
                        encIter++;
                        handledPorts.insert(port);
                        break;
                    }
                } else {
                    if (indexWidth != 0) {
                        continue;
                    }
                    assert(fuParent != NULL);
                    BaseFUPort* fuPort = dynamic_cast<BaseFUPort*>(port);
                    assert(fuPort != NULL);
                    if (fuPort->isOpcodeSetting()) {
                        // map<operation name, operation index in FU>
                        map<string,int> opcodeSet;
                        assert(*indexWidthIter == 0);
                        // operation indeces are numbered according to the
                        // alphabetical order of opertations
                        for (int opIndex = 0;
                             opIndex < fuParent->operationCount();
                             opIndex++) {
                            opcodeSet.insert(
                                make_pair(
                                    fuParent->operation(opIndex)->name(),
                                    opIndex));
                        }
                        
                        for (map<string,int>::iterator 
                                 iter = opcodeSet.begin();
                             iter != opcodeSet.end(); iter++) {
                            Encoding enc = *encIter;
                            HWOperation* operation = 
                                fuParent->operation(iter->second);
                            assert(operation->name() == iter->first);
                            new FUPortCode(
                                fuParent->name(), fuPort->name(),
                                operation->name(), enc.first, enc.second,
                                table);
                            indexWidthIter++;
                            encIter++;
                        }
                    } else {
                        new FUPortCode(
                            fuParent->name(), fuPort->name(), enc.first,
                            enc.second, table);
                        indexWidthIter++;
                        encIter++;
                    }
                    handledPorts.insert(port);
                    break;
                }
            }
        }
    }
}


/**
 * Returns a multiset containing the socket code widths for sockets of
 * the given direction that are connected to the given bus.
 *
 * @param bus The bus.
 * @param socket The direction.
 * @return The multiset.
 */
std::multiset<int>
BEMGenerator::socketCodeWidthsForBus(
    const TTAMachine::Bus& bus,
    Socket::Direction socketDir) const {

    std::multiset<int> socketCodeWidths;
    int socketCount = BEMGenerator::socketCount(bus, socketDir);
    for (int i = 0; i < socketCount; i++) {
        Socket& socket = BEMGenerator::socket(i, bus, socketDir);
        SocketCodeTable* scTable = socketCodeTable(socket);
        if (scTable == NULL) {
            socketCodeWidths.insert(0);
        } else {
            socketCodeWidths.insert(scTable->width());
        }
    }

    return socketCodeWidths;
}


/**
 * Checks whether the given sockets have equal port connections.
 *
 * @param socket1 The first socket.
 * @param socket2 The second socket.
 * @return True if the sockets have equal port connections, otherwise false.
 */
bool
BEMGenerator::haveEqualConnections(
    const Socket& socket1,
    const Socket& socket2) {

    std::set<Port*> socket1Ports;
    std::set<Port*> socket2Ports;

    for (int i = 0; i < socket1.portCount(); i++) {
        socket1Ports.insert(socket1.port(i));
    }

    for (int i = 0; i < socket2.portCount(); i++) {
        socket2Ports.insert(socket2.port(i));
    }
    
    if (socket1Ports == socket2Ports) {
        return true;
    } else {
        return false;
    }
}
            

/**
 * Tells how many source bridges the given bus has.
 *
 * @param bus The bus.
 * @return The number of source bridges.
 */
int
BEMGenerator::sourceBridgeCount(const Bus& bus) {

    Machine* mach = bus.machine();
    assert(mach != NULL);
    Machine::BridgeNavigator bridgeNav = mach->bridgeNavigator();

    int count(0);

    for (int i = 0; i < bridgeNav.count(); i++) {
        Bridge* bridge = bridgeNav.item(i);
        if (bridge->destinationBus() == &bus) {
            count++;
        }
    }

    assert(count <= 2);
    return count;
}


/**
 * By the given index returns a source bridge for the given bus.
 *
 * @param index The index (0 or 1).
 * @param bus The bus.
 */
Bridge&
BEMGenerator::sourceBridge(int index, const Bus& bus) {
    
    Machine* mach = bus.machine();
    assert(mach != NULL);
    Machine::BridgeNavigator bridgeNav = mach->bridgeNavigator();

    int count(0);

    for (int i = 0; i < bridgeNav.count(); i++) {
        Bridge* bridge = bridgeNav.item(i);
        if (bridge->destinationBus() == &bus) {
            if (count == index) {
                return *bridge;
            } else {
                count++;
            }
        }
    }

    assert(false);
    // dummy return
    return *bridgeNav.item(0);
}


/**
 * Tells whether the given bus has an unconditional guard.
 *
 * @param bus The bus.
 * @return True if the bus has an unconditional guard, otherwise false.
 */
bool
BEMGenerator::hasUnconditionalGuard(const Bus& bus) {
    for (int i = 0; i < bus.guardCount(); i++) {
        Guard* guard = bus.guard(i);
        if (dynamic_cast<UnconditionalGuard*>(guard) != NULL) {
            return true;
        }
    }
    return false;
}


/**
 * Returns the number of bits needed to identify a register in the given 
 * register file.
 *
 * @param regFile The register file.
 * @return The number of bits needed for register index.
 */
int
BEMGenerator::requiredIndexWidth(const BaseRegisterFile& regFile) {
    if (regFile.numberOfRegisters() <= 1) {
        return 0;
    } else {
        return MathTools::bitLength(regFile.numberOfRegisters() - 1);
    }
}


/**
 * Calculates unambiguous encodings when the encodings have opposite
 * fields of the given widths.
 *
 * Minimizes both the total width of the field and the width of the
 * encodings. Currently supports only left aligment of encodings.
 *
 * @param oppositeFieldWidths Widths of the opposite fields.
 * @param alignment Tells whether the encodings are aligned to left (true)
 *                  or right (false).
 * @param encoding The encodings are added here.
 */
void
BEMGenerator::calculateEncodings(
    const std::multiset<int>& oppositeFieldWidths,
    bool alignment,
    std::multiset<Encoding>& encodings) {

    if (oppositeFieldWidths.size() < 1) {
        return;
    }

    assert(alignment);

    unsigned int remainder = 0;
    unsigned int encodingsLeft = oppositeFieldWidths.size();
    unsigned int prevEncoding = 0;
    unsigned int nextEncoding = 0;
    unsigned int prevOppositeFieldWidth = 0;

    for (multiset<int>::reverse_iterator iter = oppositeFieldWidths.rbegin();
         iter != oppositeFieldWidths.rend(); iter++) {
        unsigned int oppositeFieldWidth = *iter;
        if (iter == oppositeFieldWidths.rbegin()) {
            nextEncoding = 0;
            encodings.insert(Encoding(nextEncoding, 0));
            remainder = 1;
        } else {
            nextEncoding = prevEncoding + 1;
            if (MathTools::bitLength(nextEncoding) > 
                MathTools::bitLength(prevEncoding)) {
                addExtraBits(encodings, 1);
                unsigned int setEncodingCount = 
                    oppositeFieldWidths.size() - encodingsLeft;
                remainder = (remainder << 1) + setEncodingCount;
            }

            if (oppositeFieldWidth == prevOppositeFieldWidth) {
                encodings.insert(Encoding(nextEncoding, 0));
            } else {
                assert(oppositeFieldWidth < prevOppositeFieldWidth);
                unsigned int freeBits = 
                    prevOppositeFieldWidth - oppositeFieldWidth;
                // calculate the number of bits the encoding has to be
                // expanded
                unsigned int expansion = 0;
                while (remainder << expansion < encodingsLeft) {
                    if (expansion < freeBits) {
                        expansion++;
                    } else {
                        break;
                    }
                }
                nextEncoding = nextEncoding << expansion;
                encodings.insert(Encoding(nextEncoding, 0));
                remainder = remainder << expansion;
            }
        }
        prevOppositeFieldWidth = oppositeFieldWidth;
        encodingsLeft--;
        remainder--;
        prevEncoding = nextEncoding;
    }

    assert(encodings.size() == oppositeFieldWidths.size());
}


/**
 * Adds the given number of extra bits to the encodings in the given set.
 *
 * @param encodings The encodings.
 * @param bitLength The number of bits.
 */
void
BEMGenerator::addExtraBits(
    std::multiset<Encoding>& encodings,
    int bitCount) {

    multiset<Encoding> newSet;
    for (multiset<Encoding>::iterator iter = encodings.begin();
         iter != encodings.end(); iter++) {
        unsigned int encoding = iter->first;
        unsigned int extraBits = iter->second;
        extraBits += bitCount;
        newSet.insert(Encoding(encoding, extraBits));
    }

    encodings.clear();
    encodings.insert(newSet.begin(), newSet.end());
}


/** 
 * Returns the width of the longest long immediate template being encoded in a
 * slot given as parameter.
 * 
 * @param slot Move slot where to check if it's used to store long immediates.
 * @return Width of the longest long immediate width stored in a move slot.
 */
unsigned int
BEMGenerator::maxLongImmSlotWidth(const MoveSlot& slot) const {
    Machine::InstructionTemplateNavigator itNav = 
        machine_->instructionTemplateNavigator();
    int maxWidth = 0;

    for (int i = 0; i < itNav.count(); i++) {
        InstructionTemplate* iTemp = itNav.item(i);
        for (int i = 0; i < iTemp->slotCount(); ++i) {
            if (slot.name() == iTemp->slot(i)->slot() &&
                maxWidth < iTemp->slot(i)->width()) {

                maxWidth = iTemp->slot(i)->width();
            } 
        }
    }

    return maxWidth;
}

