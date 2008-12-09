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
 * @file PIGTextGenerator.cc
 *
 * Implementation of PIGTextGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
