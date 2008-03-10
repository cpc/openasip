/**
 * @file HDBEditorAboutCmd.cc
 *
 * Implementation of HDBEditorAboutCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "HDBEditorAboutCmd.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditorAboutDialog.hh"


/**
 * The Constructor.
 */
HDBEditorAboutCmd::HDBEditorAboutCmd():
    GUICommand(HDBEditorConstants::COMMAND_NAME_ABOUT, NULL) {

}

/**
 * The Destructor.
 */
HDBEditorAboutCmd::~HDBEditorAboutCmd() {
}


/**
 * Executes the command.
 */
bool
HDBEditorAboutCmd::Do() {
    assert(parentWindow() != NULL);
    HDBEditorAboutDialog dialog(parentWindow());
    dialog.ShowModal();
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
HDBEditorAboutCmd::icon() const {
    return "about.png";
}


/**
 * Returns ID of this command.
 */
int
HDBEditorAboutCmd::id() const {
    return HDBEditorConstants::COMMAND_ABOUT;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
HDBEditorAboutCmd*
HDBEditorAboutCmd::create() const {
    return new HDBEditorAboutCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
HDBEditorAboutCmd::isEnabled() {
    return true;
}
