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
 * @file MachineTestReportTextGenerator.cc
 *
 * Implementation of MachineTestReportTextGenerator class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 */

#include "MachineTestReportTextGenerator.hh"


/**
 * Constructor.
 */
MachineTestReportTextGenerator::MachineTestReportTextGenerator() {
    addText(TXT_SOCKET_SEGMENT_CONN_EXISTS,
            "Connection of socket '%1%' and segment '%2%' of bus "
            "'%3%' already exists.");
    addText(TXT_SOCKET_SEGMENT_CONN_ILLEGAL_REG,
            "Socket '%1%' and bus '%2%' are not registered to the same "
            "machine.");
    addText(TXT_SOCKET_SEGMENT_CONN_BOTH_DIRS_ARE_ILLEGAL,
            "Direction of socket '%1%' can be neither input nor output "
            "because connection to port is '%2%' of unit '%3%' is illegal "
            "if the direction is input and connection to port '%4%' of unit "
            "'%5%' is illegal in the case that direction is output.");
    addText(TXT_SOCKET_PORT_CONN_ILLEGAL_REG,
            "Socket '%1%' and unit '%2%' are not registrered to the same "
            "machine.");
    addText(TXT_SOCKET_PORT_CONN_EXISTS,
            "Connection of socket '%1%' and port '%2%' of unit '%3%' "
            "already exists.");
    addText(TXT_SOCKET_PORT_CONN_RF_PORT_CONN_EXISTS,
            "Connection of socket '%1%' and port '%2%' of register file "
            "'%3%' is illegal because two ports of a register file cannot "
            "be connected to the same socket.");
    addText(TXT_SOCKET_PORT_CONN_WRONG_SOCKET_DIR,
            "Connection of socket '%1%' and port '%2%' of unit '%3%' is "
            "illegal because of the direction of the socket.");
    addText(TXT_SOCKET_PORT_CONN_MAX_CONN,
            "Connection of socket '%1%' and port '%2%' of unit '%3%' is "
            "illegal because the port has maximum number of connections.");
    addText(TXT_BRIDGE_ILLEGAL_REG,
            "Buses '%1%' and '%2%' are not registered to the same machine.");
    addText(TXT_BRIDGE_LOOP,
            "Bridging buses '%1%' and '%2%' would create a loop into the "
            "bus chain.");
    addText(TXT_BRIDGE_EXISTS,
            "There is already a bridge between buses '%1%' and '%2%'.");
    addText(TXT_BRIDGE_BRANCHED_BUS,
            "Cannot bridge buses '%1%' and '%2%' because bus '%3%' would be "
            "branched.");
    addText(TXT_SOCKET_DIR_UNKNOWN,
            "Direction of socket '%1%' can never be set to UNKNOWN.");
    addText(TXT_SOCKET_NO_CONN,
            "Direction of socket '%1%' cannot be set because it has no "
            "segments connected.");
    addText(TXT_SOCKET_SAME_DIR_CONN,
            "Direction of socket '%1%' cannot be '%2%' because it is "
            "connected to port '%3%' of unit '%4%' which is already "
            "connected to another socket of the same direction.");
    addText(TXT_SOCKET_FORBIDDEN_DIR,
            "Direction of socket '%1%' cannot be '%2%' because it is "
            "connected to port '%3%' of unit '%4%' which doesn't allow "
            "sockets of that direction to be connected.");
}


/**
 * Destructor.
 */
MachineTestReportTextGenerator::~MachineTestReportTextGenerator() {
}
