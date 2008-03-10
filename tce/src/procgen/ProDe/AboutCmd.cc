/**
 * @file AboutCmd.cc
 *
 * Definition of AboutCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
 */

#include "AboutCmd.hh"
#include "ProDeConstants.hh"
#include "AboutDialog.hh"

using std::string;

/**
 * The Constructor.
 */
AboutCmd::AboutCmd():
    EditorCommand(ProDeConstants::CMD_NAME_ABOUT) {

}


/**
 * The Destructor.
 */
AboutCmd::~AboutCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
AboutCmd::Do() {
    AboutDialog about(parentWindow());
    about.ShowModal();
    return true;
}


/**
 * Returns id of this command.
 */
int
AboutCmd::id() const {
    return ProDeConstants::COMMAND_ABOUT;
}


/**
 * Creates and returns a new instance of this command.
 */
AboutCmd*
AboutCmd::create() const {
    return new AboutCmd();
}


/**
 * This command is always executable.
 *
 * @return Always true.
 */
bool
AboutCmd::isEnabled() {
    return true;
}


/**
 * Returns path to the command's icon file.
 */
string
AboutCmd::icon() const {
    return ProDeConstants::CMD_ICON_ABOUT;
}

