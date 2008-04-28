/**
 * @file TCEString.hh
 *
 * Declaration of TCEString class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef TCE_STRING_HH
#define TCE_STRING_HH

#include <string>

/**
 * A simple wrapper around std::string to make forward declarations possible.
 * 
 * Also used if TCE needs a special string class some nice day.
 */
class TCEString : public std::string {
public:
    TCEString();
    TCEString(const char* text);
    TCEString(const std::string& text);
private:
};


#endif
