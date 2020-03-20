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
 * @file SOPCInterface.cc
 *
 * Implementation of SOPCInterface class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <cassert>
#include "SOPCInterface.hh"
#include "Conversion.hh"
using std::vector;
using std::map;
using std::endl;

const TCEString SOPCInterface::SOPC_ADD_INTERFACE =
    "add_interface";
const TCEString SOPCInterface::SOPC_SET_INT_PROPERTY = 
    "set_interface_property";
const TCEString SOPCInterface::SOPC_ADD_INT_PORT =
    "add_interface_port";
const TCEString SOPCInterface::SOPC_ASSOCIATED_CLOCK =
    "ASSOCIATED_CLOCK";

const TCEString SOPCInterface::SOPC_CLOCK_INT_NAME = "clock_reset";
const TCEString SOPCInterface::SOPC_CLOCK_INT_DECLR = "clock end";

const TCEString SOPCInterface::SOPC_EXPORT_INT_NAME = "conduit_interface";
const TCEString SOPCInterface::SOPC_EXPORT_INT_DECLR = "conduit end";
const TCEString SOPCInterface::SOPC_EXPORT_NAME = "export";

const TCEString SOPCInterface::SOPC_MASTER_INT_NAME = "avalon_master";
const TCEString SOPCInterface::SOPC_MASTER_INT_DECLR = "avalon start";

const TCEString SOPCInterface::SOPC_IRQ_RECV_INT_NAME = "interrupt_recv";
const TCEString SOPCInterface::SOPC_IRQ_RECV_INT_DECLR = "interrupt start";

const TCEString SOPCInterface::SOPC_INPUT = "Input";
const TCEString SOPCInterface::SOPC_OUTPUT = "Output";
const TCEString SOPCInterface::SOPC_BIDIR = "Bidir";

SOPCInterface::SOPCInterface(TCEString name, TCEString declaration):
    name_(name), declaration_(declaration) {
}

SOPCInterface::~SOPCInterface() {
}

void SOPCInterface::setProperty(
    const TCEString& propertyName,
    const TCEString& propertyValue) {

    properties_[propertyName] = propertyValue;
}


void SOPCInterface::setProperty(
    const TCEString& propertyName,
    int propertyValue) {

    TCEString convertedValue = Conversion::toString(propertyValue);
    setProperty(propertyName, convertedValue);
}

void SOPCInterface::setPort(
    const TCEString& hdlName,
    const TCEString& interfaceName,
    HDB::Direction direction,
    int width) {

    SOPCPort port = {hdlName, interfaceName, direction, width};
    ports_.push_back(port);
}

TCEString
SOPCInterface::name() const {

    return name_;
}

bool
SOPCInterface::hasPorts() const {

    return !ports_.empty();
}

void
SOPCInterface::writeInterface(std::ostream& stream) const {

    stream
        << "# " << name_ << endl
        << SOPC_ADD_INTERFACE << " " << name_ << " " << declaration_ << endl;
    writeProperties(stream);
    writePorts(stream);
    stream << endl;
}

const SOPCInterface::PropertyMap*
SOPCInterface::properties() const {

    return &properties_;
}

const SOPCInterface::PortList*
SOPCInterface::ports() const {
    
    return &ports_;
}


void
SOPCInterface::writeProperties(std::ostream& stream) const {

    PropertyMap::const_iterator iter = properties_.begin();
    while (iter != properties_.end()) {
        stream 
            << SOPC_SET_INT_PROPERTY << " " << name_ << " " << iter->first
            << " " << iter->second << endl;
        iter++;
    }
}


void
SOPCInterface::writePorts(std::ostream& stream) const {

    for (unsigned int i = 0; i < ports_.size(); i++) {
        stream
            << SOPC_ADD_INT_PORT << " " << name_ << " "
            << ports_.at(i).hdlName << " " << ports_.at(i).interfaceName
            << " ";
        if (ports_.at(i).direction == HDB::IN) {
            stream << SOPC_INPUT << " ";
        } else if (ports_.at(i).direction == HDB::OUT) {
            stream << SOPC_OUTPUT << " ";
        } else if (ports_.at(i).direction == HDB::BIDIR) {
            stream << SOPC_BIDIR << " ";
        } else {
            assert(false && "Invalid port direction");
        }
        stream << ports_.at(i).width << endl;
    }
}
