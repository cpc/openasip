/**
 * @file BEMGeneratorCLITextGenerator.hh
 *
 * Declaration of BEMGeneratorCLITextGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BEM_GENERATOR_CLI_TEXT_GENERATOR_HH
#define TTA_BEM_GENERATOR_CLI_TEXT_GENERATOR_HH

#include "TextGenerator.hh"

/**
 * Text generator used for texts of BEM generator command line user 
 * interface.
 */
class BEMGeneratorCLITextGenerator : public Texts::TextGenerator {
public:
    BEMGeneratorCLITextGenerator();
    virtual ~BEMGeneratorCLITextGenerator();

    /// Ids for the stored texts.
    enum {
        TXT_CLI_TITLE = Texts::LAST__,
        TXT_CLI_VERSION
    };
};

#endif
