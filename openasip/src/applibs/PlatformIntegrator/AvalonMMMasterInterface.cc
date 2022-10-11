/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file AvalonMMMasterInterface.hh
 *
 * Implementation of AvalonMMMasterInterface class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include "AvalonMMMasterInterface.hh"

const TCEString AvalonMMMasterInterface::AVALON_MM_ADDRESS = "address";
const TCEString AvalonMMMasterInterface::AVALON_MM_WDATA = "writedata";
const TCEString AvalonMMMasterInterface::AVALON_MM_RDATA = "readdata";
const TCEString AvalonMMMasterInterface::AVALON_MM_READ = "read";
const TCEString AvalonMMMasterInterface::AVALON_MM_WRITE = "write";
const TCEString AvalonMMMasterInterface::AVALON_MM_BYTE_ENABLE = 
    "byteenable";
const TCEString AvalonMMMasterInterface::AVALON_MM_WAIT_REQ = "waitrequest";
const TCEString AvalonMMMasterInterface::AVALON_MM_IRQ_RECV = "irq";

const int AvalonMMMasterInterface::ADDRESS_DEF_WIDTH = 32;
const int AvalonMMMasterInterface::WDATA_DEF_WIDTH = 32;
const int AvalonMMMasterInterface::RDATA_DEF_WIDTH = 32;
const int AvalonMMMasterInterface::READ_DEF_WIDTH = 1;
const int AvalonMMMasterInterface::WRITE_DEF_WIDTH = 1;
const int AvalonMMMasterInterface::BYTE_ENABLE_DEF_WIDTH = WDATA_DEF_WIDTH/8;
const int AvalonMMMasterInterface::WAIT_REQ_DEF_WIDTH = 1;
const int AvalonMMMasterInterface::IRQ_RECV_DEF_WIDTH = 32;

AvalonMMMasterInterface::AvalonMMMasterInterface(
    const TCEString& name,
    const TCEString& declaration,
    const TCEString& avalonPrefix,
    const SOPCInterface& clock):
    SOPCInterface(name, declaration), avalonPrefix_(avalonPrefix),
    clock_(&clock) {

    SOPCPort addr = {"", AVALON_MM_ADDRESS, ProGe::OUT, ADDRESS_DEF_WIDTH};
    masterPorts_.push_back(addr);
    SOPCPort wdata = {"", AVALON_MM_WDATA, ProGe::OUT, WDATA_DEF_WIDTH};
    masterPorts_.push_back(wdata);
    SOPCPort rdata = {"", AVALON_MM_RDATA, ProGe::IN, RDATA_DEF_WIDTH};
    masterPorts_.push_back(rdata);
    SOPCPort read = {"", AVALON_MM_READ, ProGe::OUT, READ_DEF_WIDTH};
    masterPorts_.push_back(read);
    SOPCPort write = {"", AVALON_MM_WRITE, ProGe::OUT, WRITE_DEF_WIDTH};
    masterPorts_.push_back(write);
    SOPCPort bEnable =
        {"", AVALON_MM_BYTE_ENABLE, ProGe::OUT, BYTE_ENABLE_DEF_WIDTH};
    masterPorts_.push_back(bEnable);
    SOPCPort waitReq =
        {"", AVALON_MM_WAIT_REQ, ProGe::IN, WAIT_REQ_DEF_WIDTH};
    masterPorts_.push_back(waitReq);
    SOPCPort irqRecv = {"", AVALON_MM_IRQ_RECV, ProGe::IN, IRQ_RECV_DEF_WIDTH};
    masterPorts_.push_back(irqRecv);

    // add default properties
    setProperty("linewrapBursts", "false");
    setProperty("adaptsTo", "\"\"");
    setProperty("doStreamReads","false");
    setProperty("doStreamWrites","false");
    setProperty("burstOnBurstBoundariesOnly", "false");
    setProperty(SOPC_ASSOCIATED_CLOCK, clock_->name());
}
    
AvalonMMMasterInterface::~AvalonMMMasterInterface() {
    
    for (unsigned int i = 0; i < childInterfaces_.size(); i++) {
        if (childInterfaces_.at(i) != NULL) {
            delete childInterfaces_.at(i);
        }
    }
}

bool
AvalonMMMasterInterface::isValidPort(const ProGe::NetlistPort& port) const {
    
    return findInterfacePort(port.name(), port.direction()) != NULL;
}

void
AvalonMMMasterInterface::addPort(const ProGe::NetlistPort& port) {

    const SOPCPort* avalonPort =
        findInterfacePort(port.name(), port.direction());
    if (avalonPort->interfaceName == AVALON_MM_IRQ_RECV) {
        addIrqInterface(port, *avalonPort);
    } else {
        int width = avalonPort->width;
        if (port.realWidthAvailable()) {
            width = port.realWidth();
        }
        setPort(port.name(), avalonPort->interfaceName,
                avalonPort->direction, width);
    }
}


void
AvalonMMMasterInterface::writeInterface(std::ostream& stream) const {

    SOPCInterface::writeInterface(stream);
    for (unsigned int i = 0; i < childInterfaces_.size(); i++) {
        childInterfaces_.at(i)->writeInterface(stream);
    }
}

const SOPCInterface::SOPCPort*
AvalonMMMasterInterface::findInterfacePort(
    TCEString hdlName,
    ProGe::Direction direction) const {

    const SOPCPort* found = NULL;
    for (unsigned int i = 0; i < masterPorts_.size(); i++) {
        TCEString search = avalonPrefix_ + masterPorts_.at(i).interfaceName;
        if (hdlName.find(search) != TCEString::npos) {
            if (direction == masterPorts_.at(i).direction) {
                found = &masterPorts_.at(i);
                break;
            }
        }
    }
    return found;
}

void
AvalonMMMasterInterface::addIrqInterface(
    const ProGe::NetlistPort& port,
    const SOPCPort& avalonPort) {

    TCEString irqName = name() + SOPC_IRQ_RECV_INT_NAME;
    TCEString declr = SOPC_IRQ_RECV_INT_DECLR;
    SOPCInterface* irq = new SOPCInterface(irqName, declr);
    irq->setProperty("associatedAddressablePoint", name());
    irq->setProperty("irqScheme", "INDIVIDUAL_REQUESTS");
    irq->setProperty(SOPC_ASSOCIATED_CLOCK, clock_->name());
  
    int width = avalonPort.width;
    if (port.realWidthAvailable()) {
        width = port.realWidth();
    }
    irq->setPort(port.name(), avalonPort.interfaceName,
                 avalonPort.direction, width);

    childInterfaces_.push_back(irq);
}
