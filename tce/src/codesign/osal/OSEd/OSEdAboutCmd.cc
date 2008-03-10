/**
 * @file OSEdAboutCmd.cc
 *
 * Definition of OSEdAboutCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "OSEdAboutCmd.hh"
#include "OSEdConstants.hh"
#include "InformationDialog.hh"
#include "WxConversion.hh"
#include "OSEdTextGenerator.hh"
#include "OSEdAboutDialog.hh"

using boost::format;
using std::string;

/**
 * Constructor.
 */
OSEdAboutCmd::OSEdAboutCmd() : 
    GUICommand(OSEdConstants::CMD_NAME_ABOUT, NULL) {
}

/**
 * Destructor.
 */
OSEdAboutCmd::~OSEdAboutCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdAboutCmd::id() const {
    return OSEdConstants::CMD_ABOUT;
}

/**
 * Creates a new command.
 *
 * @return The created command.
 */
GUICommand*
OSEdAboutCmd::create() const {
    return new OSEdAboutCmd();
}

/**
 * Executes the command.
 *
 * @return True.
 */
bool
OSEdAboutCmd::Do() {
    OSEdAboutDialog about(parentWindow());
    about.ShowModal();
    return true;
}

/**
 * Returns true, if command is enabled.
 *
 * @return Always true.
 */
bool
OSEdAboutCmd::isEnabled() {
    return true;
}

/**
 * Returns the icon path.
 *
 * @return Empty string (no icons used).
 */
string
OSEdAboutCmd::icon() const {
    return "";
}
