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
 * @file PIGTextGenerator.hh
 *
 * Declaration PIGTextGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
