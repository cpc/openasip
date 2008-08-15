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
 * @file MOMTextGenerator.hh
 *
 * Declaration of MOMTextGenerator class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 */

#ifndef TTA_MOM_TEXT_GENERATOR_HH
#define TTA_MOM_TEXT_GENERATOR_HH

#include "TextGenerator.hh"

/**
 * Stores the error messages given in exceptions thrown from MOM.
 */
class MOMTextGenerator : public Texts::TextGenerator {
public:
    MOMTextGenerator();
    virtual ~MOMTextGenerator();

    /// Ids for the stored texts.
    enum {
        TXT_GUARD_REF_LOAD_ERR = Texts::LAST__,
        TXT_GUARD_REF_LOAD_ERR_FU,
        TXT_GUARD_REF_LOAD_ERR_RF,
        TXT_GUARD_REF_LOAD_ERR_PORT,
        TXT_GUARD_REF_LOAD_ERR_REGISTER,
        TXT_EQUAL_PORT_GUARDS,
        TXT_EQUAL_REGISTER_GUARDS,
        TXT_EQUAL_UNCONDITIONAL_GUARDS,
        TXT_NO_LAST_SEGMENT,
        TXT_NO_SOURCE_SEGMENT,
        TXT_SEGMENT_WITH_SAME_NAME,
        TXT_SEGMENT_REF_LOAD_ERR,
        TXT_SEGMENT_REF_LOAD_ERR_SOCKET,
        TXT_SOCKET_REF_LOAD_ERR,
        TXT_SOCKET_REF_LOAD_ERR_BUS,
        TXT_SOCKET_REF_LOAD_ERR_SEGMENT,
        TXT_UNKNOWN_SOCKET_DIR_AND_SEGMENT_CONN,
        TXT_SET_DIR_SOCKET_NOT_REGISTERED,
        TXT_OPCODE_SETTING_PORT_EXISTS,
        TXT_OPCODE_SETTING_MUST_BE_TRIGGERING,
        TXT_BRIDGE_EXISTS_BY_SAME_NAME,
        TXT_BRIDGE_UNKNOWN_SRC_OR_DST,
        TXT_IT_EXISTS_BY_NAME,
        TXT_IT_REF_LOAD_ERR_SLOT,
        TXT_IT_REF_LOAD_ERR_IU,
        TXT_AS_EXISTS_BY_NAME,
        TXT_PORT_EXISTS_BY_NAME,
        TXT_PORT_REF_LOAD_ERR_SOCKET,
        TXT_FU_REF_LOAD_ERR_AS,
        TXT_OPERATION_REF_LOAD_ERR_PORT,
        TXT_OPERATION_EXISTS_BY_SAME_NAME,
        TXT_OPERAND_BOUND_TO_PORT,
        TXT_OPERAND_ALREADY_BOUND,
        TXT_PORT_READ_AND_WRITTEN_BY_PIPELINE,
        TXT_PIPELINE_START_TOO_LATE,
        TXT_PIPELINE_NOT_CANONICAL,
        TXT_SAME_NAME,
        TXT_INVALID_NAME,
        TXT_BUS_AND_IMM_SLOT_WITH_SAME_NAME,
        TXT_IMM_SLOT_EXISTS_BY_SAME_NAME,
        TXT_INVALID_GUARD_LATENCY
    };
};

#endif
