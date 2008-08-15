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
 * @file PIGTextGenerator.cc
 *
 * Implementation of PIGTextGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "PIGTextGenerator.hh"

/**
 * The constructor.
 */
PIGTextGenerator::PIGTextGenerator() {
    addText(TXT_TEMPLATE_ENCODING_MISSING, 
            "Encoding for instruction template '%1%' is missing from BEM.");
    addText(TXT_EMPTY_ITEMP_MISSING,
            "Empty instruction template missing in machine.");
    addText(TXT_BEM_DEFINES_SLOT_FOR_NONEXISTING_BUS,
            "BEM defines a move slot for bus '%1%' which does not exist "
            "in the machine.");
    addText(TXT_BEM_DEFINES_IMM_SLOT_THAT_IS_NEVER_USED,
            "BEM defines an immediate slot that is not used by any "
            "instruction template of the machine.");
    addText(TXT_ALWAYS_TRUE_GUARD_ENCODING_MISSING,
            "BEM does not define an encoding for always-true guard "
            "expression for bus '%1%'.");
    addText(TXT_FU_GUARD_ENCODING_MISSING,
            "BEM does not define an encoding for '%1%' FU guard expression "
            "of port '%2%' of function unit '%3%'.");
    addText(TXT_GPR_GUARD_ENCODING_MISSING,
            "BEM does not define an encoding for '%1%' GPR guard expression "
            "of register '%2%' of register file '%3%'.");
    addText(TXT_NOP_ENCODING_MISSING,
            "BEM does not define an encoding for NOP on bus '%1%'.");
    addText(TXT_SOCKET_ENCODING_MISSING,
            "BEM does not define an encoding for socket '%1%' at the %2% "
            "of bus '%3%'.");
    addText(TXT_IU_PORT_CODE_MISSING,
            "BEM does not define an encoding for port of immediate unit "
            "'%1%' which is connected to socket '%2%', in move slot '%3%'.");
    addText(TXT_RF_PORT_CODE_MISSING,
            "BEM does not define an encoding for port of register file "
            "'%1%' which is connected to socket '%2%', in move slot '%3%'.");
    addText(TXT_FU_OC_PORT_CODE_MISSING,
            "BEM does not define an encoding for opcode setting port of "
            "function unit '%1%' for operation '%2%'.");
    addText(TXT_OSAL_OC_MISSING,
            "Osal operation for operation code name '%1%' in function unit "
            "'%2%' was not found.");
    addText(TXT_FU_PORT_CODE_MISSING,
            "BEM does not define an encoding for port '%1%' of function "
            "unit '%2%' which is connected to socket '%3%'.");
    addText(TXT_IMMEDIATE_ENCODING_MISSING,
            "BEM does not define an encoding for immediate in the source "
            "field of bus '%1%'.");
    addText(TXT_FILE_NOT_FOUND,
            "File '%1%' not found.");
}


/**
 * The destructor.
 */
PIGTextGenerator::~PIGTextGenerator() {
}
