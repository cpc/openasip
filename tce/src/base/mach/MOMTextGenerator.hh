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
