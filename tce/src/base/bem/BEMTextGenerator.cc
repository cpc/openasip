/**
 * @file BEMTextGenerator.cc
 *
 * Implementation of BEMTextGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "BEMTextGenerator.hh"

/**
 * The constructor.
 */
BEMTextGenerator::BEMTextGenerator() : TextGenerator() {
    addText(TXT_ILLEGAL_IMM_ENC, 
            "Illegal immediate encoding in the source field of move slot "
            "'%1%'.");
}


/**
 * The destructor.
 */
BEMTextGenerator::~BEMTextGenerator() {
}
