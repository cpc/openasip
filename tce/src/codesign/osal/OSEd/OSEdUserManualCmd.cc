/**
 * @file OSEdUserManualCmd.cc
 *
 * Definition of OSEdUserManualCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "OSEdUserManualCmd.hh"
#include "OSEdConstants.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "HelpBrowser.hh"
#include "OSEdTextGenerator.hh"
#include "WxConversion.hh"

using std::string;
using boost::format;

/**
 * Constructor.
 */
OSEdUserManualCmd::OSEdUserManualCmd() :
    GUICommand(OSEdConstants::CMD_NAME_USER_MANUAL, NULL) {
}

/**
 * Destructor.
 */
OSEdUserManualCmd::~OSEdUserManualCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdUserManualCmd::id() const {
    return OSEdConstants::CMD_USER_MANUAL;
}

/**
 * Creates new command.
 *
 * @return New command.
 */
GUICommand*
OSEdUserManualCmd::create() const {
    return new OSEdUserManualCmd();
}

/**
 * Executes the command.
 *
 * @return True if execution is successful.
 */
bool
OSEdUserManualCmd::Do() {

    string manualFile = userManual();
		
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    format fmt = texts.text(OSEdTextGenerator::TXT_USER_MANUAL_TITLE);

    HelpBrowser* browser = new HelpBrowser(
        WxConversion::toWxString(fmt.str()), 
        WxConversion::toWxString(manualFile), wxDefaultPosition,
        wxSize(800, 600));

    browser->Show(true);
    return true;
}

/**
 * Returns true if command is enabled.
 *
 * Command is enabled when user manual exists.
 *
 * @return True if command is enabled.
 */
bool
OSEdUserManualCmd::isEnabled() {
    string manualFile = userManual();
    return FileSystem::fileExists(manualFile);
}

/**
 * Returns the icon path.
 *
 * @return Empty string (icons not used).
 */
string
OSEdUserManualCmd::icon() const {
    return "";
}

/**
 * Returns the user manual location.
 *
 * @return User manual location.
 */
std::string
OSEdUserManualCmd::userManual() const {
    return Environment::manDirPath("OSALGUI") +
        FileSystem::DIRECTORY_SEPARATOR + "OSALGUI" +
        FileSystem::DIRECTORY_SEPARATOR + OSEdConstants::USER_MANUAL_NAME;
}
