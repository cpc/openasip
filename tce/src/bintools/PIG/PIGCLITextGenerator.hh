/**
 * @file PIGCLITextGenerator.hh
 *
 * Declaration of PIGCLITextGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PIG_CLI_TEXT_GENERATOR_HH
#define TTA_PIG_CLI_TEXT_GENERATOR_HH

#include "TextGenerator.hh"

/**
 * Text generator used for the texts of PIG command line user interface.
 */
class PIGCLITextGenerator : public Texts::TextGenerator {
public:
    PIGCLITextGenerator();
    virtual ~PIGCLITextGenerator();

    /// Ids for the stored texts.
    enum {
        TXT_CLI_TITLE = Texts::LAST__, 
        TXT_CLI_VERSION,
        TXT_CLI_USAGE,
        TXT_GENERATING_BEM,
        TXT_ADF_REQUIRED,
        TXT_ILLEGAL_ARGS
    };
};

#endif
