/*
    Copyright (c) 2002-2011 Tampere University.

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

const TCEString IPXactResetInterface::DEFAULT_INSTANCE_NAME = "rst_n";
const TCEString IPXactResetInterface::TTA_RESET_PORT = "rstx";
const TCEString IPXactResetInterface::SPIRIT_RESET_PORT = "RESETn";

const TCEString IPXactResetInterface::VENDOR = "TUT";
const TCEString IPXactResetInterface::LIBRARY ="ip.hwp.interface";
const TCEString IPXactResetInterface::NAME ="reset.busdef";
const TCEString IPXactResetInterface::BUS_VERSION = "1.0";
const TCEString IPXactResetInterface::ABS_VENDOR =
    IPXactResetInterface::VENDOR;
const TCEString IPXactResetInterface::ABS_LIBRARY = 
    IPXactResetInterface::LIBRARY;
const TCEString IPXactResetInterface::ABS_NAME = "reset.absDef";
const TCEString IPXactResetInterface::ABS_VERSION =
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
    for (size_t i = 0; i < toplevel.portCount(); i++) {
        if (toplevel.port(i).name() == TTA_RESET_PORT) {
            addSignalMapping(TTA_RESET_PORT, SPIRIT_RESET_PORT);
            return true;
        }
    }
    TCEString msg = "Reset port " + TTA_RESET_PORT + " not found!";
    InvalidData exc(__FILE__, __LINE__, "IPXactResetInterface", msg);
    throw exc;

    return false;
}


