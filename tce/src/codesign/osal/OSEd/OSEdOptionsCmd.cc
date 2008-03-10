/**
 * @file OSEdOptionsCmd.cc 
 *
 * Definition of OSEdOptionsCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include "OSEdOptionsCmd.hh"
#include "OSEdConstants.hh"
#include "OSEdOptionsDialog.hh"

using std::string;

/**
 * Constructor.
 */
OSEdOptionsCmd::OSEdOptionsCmd() : 
	GUICommand(OSEdConstants::CMD_NAME_OPTIONS, NULL) {
}

/**
 * Destructor.
 */
OSEdOptionsCmd::~OSEdOptionsCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdOptionsCmd::id() const {
	return OSEdConstants::CMD_OPTIONS;
}

/**
 * Creates a new command.
 *
 * @return New command.
 */
GUICommand*
OSEdOptionsCmd::create() const {
	return new OSEdOptionsCmd();
}

/**
 * Executes the command.
 *
 * @return True if execution is successful.
 */
bool
OSEdOptionsCmd::Do() {
	OSEdOptionsDialog dialog(parentWindow());
	dialog.ShowModal();
	return true;
}

/**
 * Return true if command is enabled.
 *
 * @return Always true.
 */
bool
OSEdOptionsCmd::isEnabled() {
	return true;
}

/**
 * Return icon path.
 *
 * @return Empty string (icons not used).
 */
string
OSEdOptionsCmd::icon() const {
	return "";
}
