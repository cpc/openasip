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
 * @file MOMTextGenerator.cc
 *
 * Implementation of MOMTextGenerator class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 */

#include "MOMTextGenerator.hh"

/**
 * Constructor.
 */
MOMTextGenerator::MOMTextGenerator() {
    addText(TXT_GUARD_REF_LOAD_ERR,
            "Cannot resolve references to other components from guard of "
            "bus '%1%' because the bus is not registered to a machine.");
    addText(TXT_GUARD_REF_LOAD_ERR_FU,
            "Cannot resolve the reference from guard of bus '%1%' to "
            "function unit '%2%'.");
    addText(TXT_GUARD_REF_LOAD_ERR_RF,
            "Cannot resolve the reference from guard of bus '%1%' to "
            "register file '%2%'.");
    addText(TXT_GUARD_REF_LOAD_ERR_PORT,
            "Cannot resolve the reference from guard of bus '%1%' to port "
            "'%2%' of function unit '%3%'.");
    addText(TXT_GUARD_REF_LOAD_ERR_REGISTER,
            "Cannot resolve the reference from guard of bus '%1%' to "
            "register '%2%' of register file '%3%' because the register "
            "file has only '%4%' registers.");
    addText(TXT_EQUAL_PORT_GUARDS,
            "Cannot create two equal port guards referencing to port '%1%' "
            "of function unit '%2%' to bus '%3%'.");
    addText(TXT_EQUAL_REGISTER_GUARDS,
            "Cannot create two equal register guards referencing to "
            "register '%1%' of register file '%2%' to bus '%3%'.");
    addText(TXT_EQUAL_UNCONDITIONAL_GUARDS,
            "Cannot create two equal unconditional guards to bus '%1%'.");
    addText(TXT_NO_LAST_SEGMENT,
            "Bus '%1%' has no segment that doesn't write any segment.");
    addText(TXT_NO_SOURCE_SEGMENT,
            "Bus '%1%' has no segment that writes to '%2%'.");
    addText(TXT_SEGMENT_WITH_SAME_NAME,
            "Cannot create two segments called '%1%' to bus '%2%'.");
    addText(TXT_SEGMENT_REF_LOAD_ERR,
            "Cannot resolve references to other components from segment "
            "'%1%' of bus '%2%' because the bus is not registered to a "
            "machine.");
    addText(TXT_SEGMENT_REF_LOAD_ERR_SOCKET,
            "Cannot resolve the reference from segment '%1%' of bus '%2%' "
            "to socket '%3%'.");
    addText(TXT_SOCKET_REF_LOAD_ERR,
            "Cannot resolve references to other components from socket "
            "'%1%' because the socket is not registered to a machine.");
    addText(TXT_SOCKET_REF_LOAD_ERR_BUS,
            "Cannot resolve the reference to bus '%1%' from socket '%2%'.");
    addText(TXT_SOCKET_REF_LOAD_ERR_SEGMENT,
            "Cannot resolve the reference from socket '%1%' to segment "
            "'%2%' of bus '%3%'.");
    addText(TXT_UNKNOWN_SOCKET_DIR_AND_SEGMENT_CONN,
            "Direction of socket cannot be 'UNKNOWN' because the socket is "
            "connected to segments.");
    addText(TXT_SET_DIR_SOCKET_NOT_REGISTERED,
            "Direction of socket '%1%' cannot be set because it is not "
            "registered to a machine.");
    addText(TXT_OPCODE_SETTING_PORT_EXISTS,
            "Cannot create two operation code setting ports to function "
            "unit '%1%'.");
    addText(TXT_OPCODE_SETTING_MUST_BE_TRIGGERING,
            "Port '%1%' of function unit '%2%' should be triggering because "
            "it sets operation code.");
    addText(TXT_BRIDGE_EXISTS_BY_SAME_NAME,
            "Cannot create bridge '%1%' because there is another one by the "
            "same name.");
    addText(TXT_BRIDGE_UNKNOWN_SRC_OR_DST,
            "Bridge '%1%' has unknown source or destination bus.");
    addText(TXT_IT_EXISTS_BY_NAME,
            "Cannot create instruction template '%1%' because there is "
            "another by the same name.");
    addText(TXT_IT_REF_LOAD_ERR_SLOT,
            "Cannot resolve the reference to slot '%1%' from instruction "
            "template '%2%'.");
    addText(TXT_IT_REF_LOAD_ERR_IU,
            "Cannot resolve the reference to immediate unit '%2%' from "
            "instruction template '%2%'.");
    addText(TXT_AS_EXISTS_BY_NAME,
            "Cannot create address space '%1%' because there is another one "
            "by the same name.");
    addText(TXT_PORT_EXISTS_BY_NAME,
            "Cannot create port '%1%' of unit '%2%' because there is "
            "another one by the same name.");
    addText(TXT_PORT_REF_LOAD_ERR_SOCKET,
            "Cannot resolve the reference to socket '%1%' from port '%2%' "
            "of unit '%3%'.");
    addText(TXT_FU_REF_LOAD_ERR_AS,
            "Cannot resolve the reference to address space '%1%' from "
            "function unit '%2%'.");
    addText(TXT_OPERATION_REF_LOAD_ERR_PORT,
            "Cannot resolve the reference to port '%1%' from operation "
            "'%2%' of function unit '%3%'.");
    addText(TXT_OPERATION_EXISTS_BY_SAME_NAME,
            "Cannot create operation '%1%' because another one exists by "
            "the same name in function unit '%2%'.");
    addText(TXT_OPERAND_BOUND_TO_PORT,
            "At least two operands of operation '%1%' are bound to port "
            "'%2%' of function unit '%3%'.");
    addText(TXT_OPERAND_ALREADY_BOUND,
            "Cannot bound operand '%1%' to port '%2%' in operation '%3%' "
            "of function unit '%4%' because the operand is bound to port "
            "'%5%' already.");
    addText(TXT_PORT_READ_AND_WRITTEN_BY_PIPELINE,
            "Operand '%1%' is both read and written by pipeline of "
            "operation '%2%' in function unit '%3%'. Operand can be only "
            "read or written.");
    addText(TXT_PIPELINE_START_TOO_LATE,
            "Pipeline of operation '%1%' of function unit '%2%' starts at "
            "cycle '%3%'. Pipeline must always start at cycle 0 or 1.");
    addText(TXT_PIPELINE_NOT_CANONICAL,
            "Pipeline of operation '%1%' of function unit '%2%' is not "
            "canonical.");
    addText(TXT_SAME_NAME, "Two components of same type exists by name "
            "'%1%'.");
    addText(TXT_INVALID_NAME,
            "'%1%' is not a valid name of a machine part.");
    addText(TXT_BUS_AND_IMM_SLOT_WITH_SAME_NAME,
            "There is a bus and immediate slot with the same name '%1%' in "
            "the machine.");
    addText(TXT_IMM_SLOT_EXISTS_BY_SAME_NAME,
            "Cannot create immediate slot '%1%' because there is another "
            "one by the same name.");
    addText(TXT_INVALID_GUARD_LATENCY,
            "Total guard latency cannot be zero. Global guard latency must "
            "be at least one if local guard latency is zero.");
}


/**
 * Destructor.
 */
MOMTextGenerator::~MOMTextGenerator() {
}

