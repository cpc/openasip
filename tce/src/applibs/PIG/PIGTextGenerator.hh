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
 * @file PIGTextGenerator.hh
 *
 * Declaration PIGTextGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PIG_TEXT_GENERATOR_HH
#define TTA_PIG_TEXT_GENERATOR_HH

#include "TextGenerator.hh"

/**
 * Text generator for the texts used in the program image generator.
 */
class PIGTextGenerator : public Texts::TextGenerator {
public:
    PIGTextGenerator();
    virtual ~PIGTextGenerator();

    enum {
        TXT_TEMPLATE_ENCODING_MISSING = Texts::LAST__,
        TXT_EMPTY_ITEMP_MISSING,
        TXT_BEM_DEFINES_SLOT_FOR_NONEXISTING_BUS,
        TXT_BEM_DEFINES_IMM_SLOT_THAT_IS_NEVER_USED,
        TXT_ALWAYS_TRUE_GUARD_ENCODING_MISSING,
        TXT_FU_GUARD_ENCODING_MISSING,
        TXT_GPR_GUARD_ENCODING_MISSING,
        TXT_NOP_ENCODING_MISSING,
        TXT_SOCKET_ENCODING_MISSING,
        TXT_IU_PORT_CODE_MISSING,
        TXT_RF_PORT_CODE_MISSING,
        TXT_FU_OC_PORT_CODE_MISSING,
        TXT_OSAL_OC_MISSING,
        TXT_FU_PORT_CODE_MISSING,
        TXT_IMMEDIATE_ENCODING_MISSING,
        TXT_FILE_NOT_FOUND
    };
};

#endif
