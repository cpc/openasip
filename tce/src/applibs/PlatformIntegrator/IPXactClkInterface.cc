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
 * @file IPXactClkInterface.cc
 *
 * Implementation of IPXactClkInterface class.
 *
 * @author Otto Esko 2011 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#include "IPXactClkInterface.hh"
#include "Exception.hh"
using std::string;

const std::string IPXactClkInterface::DEFAULT_INSTANCE_NAME = "clk_in";
const std::string IPXactClkInterface::TTA_CLK_PORT = "clk";
const std::string IPXactClkInterface::SPIRIT_CLK_PORT = "CLK";

const std::string IPXactClkInterface::VENDOR = "spiritconsortium.org";
const std::string IPXactClkInterface::LIBRARY ="busdef.clock";
const std::string IPXactClkInterface::NAME ="clock";
const std::string IPXactClkInterface::BUS_VERSION = "1.0";
const std::string IPXactClkInterface::ABS_VENDOR =
    IPXactClkInterface::VENDOR;
const std::string IPXactClkInterface::ABS_LIBRARY = 
    IPXactClkInterface::LIBRARY;
const std::string IPXactClkInterface::ABS_NAME = "clock_rtl";
const std::string IPXactClkInterface::ABS_VERSION =
    IPXactClkInterface::BUS_VERSION;

const IPXactModel::BusMode IPXactClkInterface::DEFAULT_BUS_MODE =
    IPXactModel::SLAVE;

IPXactClkInterface::IPXactClkInterface(): IPXactInterface() {

    setInstanceName(DEFAULT_INSTANCE_NAME);
    setBusType(VENDOR, LIBRARY, NAME, BUS_VERSION);
    setBusAbsType(ABS_VENDOR, ABS_LIBRARY, ABS_NAME, ABS_VERSION);
    setBusMode(DEFAULT_BUS_MODE);
}

IPXactClkInterface::~IPXactClkInterface() {
}

bool
IPXactClkInterface::mapPortsToInterface(const ProGe::NetlistBlock& toplevel) {
    
    for (int i = 0; i < toplevel.portCount(); i++) {
        if (toplevel.port(i).name() == TTA_CLK_PORT) {
            addSignalMapping(TTA_CLK_PORT, SPIRIT_CLK_PORT);
            return true;
        }
    }
    string msg = "Clock port " + TTA_CLK_PORT + " not found!";
    InvalidData exc(__FILE__, __LINE__, "IPXactClkInterface", msg);
    throw exc;
    return false;
}


