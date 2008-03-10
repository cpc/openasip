/**
 * @file WxConversion.hh
 *
 * Declaration of WxConversion class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_WX_CONVERSION_HH
#define TTA_WX_CONVERSION_HH

#include <wx/wx.h>
#include <string>

/**
 * Contains functions for converting between wxString and std::string.
 */
class WxConversion {
public:
    static std::string toString(const wxString& source);
    static wxString toWxString(const std::string& source);
    static wxString toWxString(const int& source);
    static wxString toWxString(const unsigned int& source);
    static wxString toWxString(const float& source);
    static wxString toWxString(const double& source);
    static wxString toWxString(const char& source);
};

#include "WxConversion.icc"

#endif
