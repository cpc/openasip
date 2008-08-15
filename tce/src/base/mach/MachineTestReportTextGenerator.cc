/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file MachineTestReportTextGenerator.cc
 *
 * Implementation of MachineTestReportTextGenerator class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
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
