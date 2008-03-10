/**
 * @file ProximAboutCmd.cc
 *
 * Implementation of ProximAboutCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximAboutCmd.hh"
#include "ProximConstants.hh"
#include "ProximAboutDialog.hh"


/**
 * The Constructor.
 */
ProximAboutCmd::ProximAboutCmd():
    GUICommand(ProximConstants::COMMAND_NAME_ABOUT, NULL) {

}

/**
 * The Destructor.
 */
ProximAboutCmd::~ProximAboutCmd() {
}


/**
 * Executes the command.
 */
bool
ProximAboutCmd::Do() {
    assert(parentWindow() != NULL);
    ProximAboutDialog dialog(parentWindow());
    dialog.ShowModal();
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximAboutCmd::icon() const {
    return "about.png";
}


/**
 * Returns ID of this command.
 */
int
ProximAboutCmd::id() const {
    return ProximConstants::COMMAND_ABOUT;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximAboutCmd*
ProximAboutCmd::create() const {
    return new ProximAboutCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximAboutCmd::isEnabled() {
    return true;
}
