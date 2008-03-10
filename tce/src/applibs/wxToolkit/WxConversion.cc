/**
 * @file WxConversion.cc
 *
 * Implementation on the WxConversion class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "WxConversion.hh"
#include "Conversion.hh"

using std::string;

/**
 * Converts an int to a wxString.
 *
 * @param source int to convert.
 * @return The int as a wxString.
 */
wxString
WxConversion::toWxString(const int& source) {
    return wxString((Conversion::toString(source)).c_str(), *wxConvCurrent);
}

/**
 * Converts an unsigned int to a wxString.
 *
 * @param source unsigned int to convert.
 * @return The unsigned int as a wxString.
 */
wxString
WxConversion::toWxString(const unsigned int& source) {
    return wxString((Conversion::toString(source)).c_str(), *wxConvCurrent);
}

/**
 * Converts char to a wxString.
 *
 * @param source char to convert.
 * @return The char as a wxString.
 */
wxString
WxConversion::toWxString(const char& source) {
    return wxString((Conversion::toString(source)).c_str(), *wxConvCurrent);
}


/**
 * Converts a floating point value to wxString.
 *
 * @param source float to convert.
 * @return The floating point value as a wxString.
 */
wxString
WxConversion::toWxString(const float& source) {
    return wxString((Conversion::toString(source)).c_str(), *wxConvCurrent);
}

/**
 * Converts a double precission floating point value to wxString.
 *
 * @param source double to convert.
 * @return The double value as a wxString.
 */
wxString
WxConversion::toWxString(const double& source) {
    return wxString((Conversion::toString(source)).c_str(), *wxConvCurrent);
}
