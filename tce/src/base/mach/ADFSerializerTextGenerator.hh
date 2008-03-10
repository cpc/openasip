/**
 * @file ADFSerializerTextGenerator.hh
 *
 * Declaration of ADFSerializerTextGenerator class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MDF_SERIALIZER_TEXT_GENERATOR_HH
#define TTA_MDF_SERIALIZER_TEXT_GENERATOR_HH

#include "TextGenerator.hh"

/**
 * Stores the error messages given in exceptions thrown from ADFSerializer.
 */
class ADFSerializerTextGenerator : public Texts::TextGenerator {
public:
    ADFSerializerTextGenerator();
    virtual ~ADFSerializerTextGenerator();

    /// Ids for the stored texts.
    enum {
        TXT_MULTIPLE_DESTINATIONS_IN_TEMPLATE_SLOT = Texts::LAST__,
        TXT_SAME_TEMPLATE_SLOT,
        TXT_IT_EMPTY_AND_NON_EMPTY,
        TXT_EMPTY_IT_NOT_DECLARED
    };
};

#endif
