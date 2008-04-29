/**
 * @file TCEString.cc
 *
 * Definition of TCEString class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */
#include <string>
#include "TCEString.hh"


/**
 * Default constructor
 */
TCEString::TCEString() : std::string() {
}

/**
 * Constructor
 * 
 * @param text text to set
 */
TCEString::TCEString(const char* text) : std::string(text) {
}

/**
 * Constructor.
 * 
 * @param text text to set
 */
TCEString::TCEString(const std::string& text) : std::string(text) {
}
