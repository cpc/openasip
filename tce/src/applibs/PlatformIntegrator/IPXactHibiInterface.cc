/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file IPXactHibiInterface.cc
 *
 * Implementation of IPXactHibiInterface class.
 *
 * @author Otto Esko 2011 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#include <string>
#include <vector>
#include "IPXactHibiInterface.hh"
#include "StringTools.hh"
#include "NetlistPort.hh"
using std::pair;
using std::string;
using ProGe::NetlistPort;

const std::string IPXactHibiInterface::DEFAULT_INSTANCE_NAME = "hibi_p";
const std::string IPXactHibiInterface::VENDOR = "TUT";
const std::string IPXactHibiInterface::LIBRARY ="ip.hwp.communication.hibi";
const std::string IPXactHibiInterface::NAME ="hibi_ip_r4.busdef";
const std::string IPXactHibiInterface::BUS_VERSION = "3.0";
const std::string IPXactHibiInterface::ABS_VENDOR =
    IPXactHibiInterface::VENDOR;
const std::string IPXactHibiInterface::ABS_LIBRARY = 
    IPXactHibiInterface::LIBRARY;
const std::string IPXactHibiInterface::ABS_NAME = "hibi_ip_r4.absdef";
const std::string IPXactHibiInterface::ABS_VERSION =
    IPXactHibiInterface::BUS_VERSION;

const IPXactModel::BusMode IPXactHibiInterface::DEFAULT_BUS_MODE =
    IPXactModel::MASTER;

IPXactHibiInterface::IPXactHibiInterface():
    IPXactInterface(), interfaceSearch_() {

    setInstanceName(DEFAULT_INSTANCE_NAME);
    setBusType(VENDOR, LIBRARY, NAME, BUS_VERSION);
    setBusAbsType(ABS_VENDOR, ABS_LIBRARY, ABS_NAME, ABS_VERSION);

    setBusMode(DEFAULT_BUS_MODE);

    interfaceSearch_.push_back(
        pair<string,string>("hibi_comm_out", "COMM_FROM_IP"));
    interfaceSearch_.push_back(
        pair<string,string>("hibi_data_out", "DATA_FROM_IP"));
    interfaceSearch_.push_back(
        pair<string,string>("hibi_av_out", "AV_FROM_IP"));
    interfaceSearch_. push_back(
        pair<string,string>("hibi_we_out", "WE_FROM_IP"));
    interfaceSearch_.push_back(
        pair<string,string>("hibi_re_out", "RE_FROM_IP"));
    interfaceSearch_.push_back(
        pair<string,string>("hibi_comm_in", "COMM_TO_IP"));
    interfaceSearch_.push_back(
        pair<string,string>("hibi_data_in", "DATA_TO_IP"));
    interfaceSearch_.push_back(
        pair<string,string>("hibi_av_in", "AV_TO_IP"));
    interfaceSearch_. push_back(
        pair<string,string>("hibi_full_in", "FULL_TO_IP"));
    interfaceSearch_.push_back(
        pair<string,string>("hibi_empty_in", "EMPTY_TO_IP"));
}

IPXactHibiInterface::~IPXactHibiInterface() {
}

bool
IPXactHibiInterface::mapPortsToInterface(
    const ProGe::NetlistBlock& toplevel) {
    
    bool instanceNameSet = false;
    for (unsigned int i = 0; i < interfaceSearch_.size(); i++) {
        for (int j = 0; j < toplevel.portCount(); j++) {           
            string interfacePort = interfaceSearch_.at(i).first;
            NetlistPort& port = toplevel.port(j);
            if (port.name().find(interfacePort) != string::npos) {
                string abstractPort = interfaceSearch_.at(i).second;
                addSignalMapping(port.name(), abstractPort);
                if (!instanceNameSet) {
                    createInstanceName(port.name(), interfacePort);
                    instanceNameSet = true;
                }
                break;
            }
        }
        if (interfaceMapping().size() == 0) {
            // does not contain this interface
            return false;
        }
    }
    bool foundAllPorts = false;
    if (interfaceSearch_.size() == interfaceMapping().size()) {
        foundAllPorts = true;
    }
    return foundAllPorts;
}

void
IPXactHibiInterface::createInstanceName(
    const std::string& fullName,
    const std::string& portName) {

    string fu = "";
    string::size_type pos = fullName.find(portName);
    if (pos == string::npos || pos == 0) {
        fu = fullName;
    } else {
        fu = StringTools::trim(fullName.substr(0, pos));
    }
    string instance = fu + DEFAULT_INSTANCE_NAME;
    setInstanceName(instance);
}
