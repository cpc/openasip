/**
 * @file ADFSerializerTextGenerator.cc
 *
 * Implementation of ADFSerializerTextGenerator class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "ADFSerializerTextGenerator.hh"

/**
 * Constructor.
 */
ADFSerializerTextGenerator::ADFSerializerTextGenerator() :
    TextGenerator() {

    addText(TXT_MULTIPLE_DESTINATIONS_IN_TEMPLATE_SLOT,
            "The slot '%1%' of instruction template '%2%' cannot write to "
            "two destination units. The same slot exists in the "
            "declarations of immediate units '%3%' and '%4%'.");
    addText(TXT_SAME_TEMPLATE_SLOT,
            "Template slot with bus '%1%' is declared two times in "
            "the declaration of immediate unit '%2%', template '%3%'.");
    addText(TXT_IT_EMPTY_AND_NON_EMPTY,
            "Instruction template '%1%' is declared empty in non-empty in "
            "different immediate unit declarations.");
    addText(TXT_EMPTY_IT_NOT_DECLARED,
            "The machine contains an empty instruction template that is not "
            "declared in the declaration of immediate unit '%1%'.");
}


/**
 * Destructor.
 */
ADFSerializerTextGenerator::~ADFSerializerTextGenerator() {
}


