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
 * @file IPXactResetInterface.cc
 *
 * Implementation of IPXactResetInterface class.
 *
 * @author Otto Esko 2011 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#include "IPXactResetInterface.hh"
#include "Exception.hh"
using std::string;

const std::string IPXactResetInterface::DEFAULT_INSTANCE_NAME = "rst_n";
const std::string IPXactResetInterface::TTA_RESET_PORT = "rstx";
const std::string IPXactResetInterface::SPIRIT_RESET_PORT = "RESETn";

const std::string IPXactResetInterface::VENDOR = "spiritconsortium.org";
const std::string IPXactResetInterface::LIBRARY ="busdef.reset";
const std::string IPXactResetInterface::NAME ="reset";
const std::string IPXactResetInterface::BUS_VERSION = "1.0";
const std::string IPXactResetInterface::ABS_VENDOR =
    IPXactResetInterface::VENDOR;
const std::string IPXactResetInterface::ABS_LIBRARY = 
    IPXactResetInterface::LIBRARY;
const std::string IPXactResetInterface::ABS_NAME = "reset_rtl";
const std::string IPXactResetInterface::ABS_VERSION =
    IPXactResetInterface::BUS_VERSION;

const IPXactModel::BusMode IPXactResetInterface::DEFAULT_BUS_MODE =
    IPXactModel::SLAVE;

IPXactResetInterface::IPXactResetInterface(): IPXactInterface() {

    setInstanceName(DEFAULT_INSTANCE_NAME);
    setBusType(VENDOR, LIBRARY, NAME, BUS_VERSION);
    setBusAbsType(ABS_VENDOR, ABS_LIBRARY, ABS_NAME, ABS_VERSION);
    setBusMode(DEFAULT_BUS_MODE);
}

IPXactResetInterface::~IPXactResetInterface() {
}

bool
IPXactResetInterface::mapPortsToInterface(const ProGe::NetlistBlock& toplevel) {
    
    for (int i = 0; i < toplevel.portCount(); i++) {
        if (toplevel.port(i).name() == TTA_RESET_PORT) {
            addSignalMapping(TTA_RESET_PORT, SPIRIT_RESET_PORT);
            return true;
        }
    }
    string msg = "Reset port " + TTA_RESET_PORT + " not found!";
    InvalidData exc(__FILE__, __LINE__, "IPXactResetInterface", msg);
    throw exc;

    return false;
}


