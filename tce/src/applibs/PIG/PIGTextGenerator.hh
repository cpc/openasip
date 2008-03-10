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
