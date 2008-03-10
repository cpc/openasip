/**
 * @file OSEdQuitCmd.cc
 *
 * Definition of OSEdQuitCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include "OSEdQuitCmd.hh"
#include "OSEdConstants.hh"
#include "OSEdMainFrame.hh"
#include "OSEd.hh"

using std::string;

/**
 * Constructor.
 */
OSEdQuitCmd::OSEdQuitCmd() : GUICommand(OSEdConstants::CMD_NAME_QUIT, NULL) {
}

/**
 * Destructor.
 */
OSEdQuitCmd::~OSEdQuitCmd() {
}

/**
 * Returns the id of the command.
 */
int
OSEdQuitCmd::id() const {
    return OSEdConstants::CMD_QUIT;
}

/**
 * Creates a new command.
 *
 * @return Created command.
 */
GUICommand*
OSEdQuitCmd::create() const {
    return new OSEdQuitCmd();
}

/**
 * Executes the command.
 *
 * @return False.
 */
bool
OSEdQuitCmd::Do() {
    OSEdMainFrame* parent = wxGetApp().mainFrame();
    parent->Close(true);
    return false;
}

/**
 * Returns true if command is enabled.
 *
 * @return True.
 */
bool
OSEdQuitCmd::isEnabled() {
    return true;
}

/**
 * Returns the icon file.
 *
 * @return Empty string (no icons used).
 */
string
OSEdQuitCmd::icon() const {
    return "";
}
