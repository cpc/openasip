/**
 * @file BEMTextGenerator.hh
 *
 * Declaration of BEMTextGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BEM_TEXT_GENERATOR_HH
#define TTA_BEM_TEXT_GENERATOR_HH

#include "TextGenerator.hh"

/**
 * Stores the error messages given in exceptions thrown from BEM object 
 * model.
 */
class BEMTextGenerator : public Texts::TextGenerator {
public:
    BEMTextGenerator();
    virtual ~BEMTextGenerator();

    /// Ids for the stored texts.
    enum {
        TXT_ILLEGAL_IMM_ENC = Texts::LAST__
    };
};

#endif
