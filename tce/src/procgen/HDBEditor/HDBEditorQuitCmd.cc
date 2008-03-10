/**
 * @file HDBEditorQuitCmd.cc
 *
 * Implementation of HDBEditorQuitCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "HDBEditorQuitCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"

/**
 * The Constructor.
 */
HDBEditorQuitCmd::HDBEditorQuitCmd():
    GUICommand(HDBEditorConstants::COMMAND_NAME_QUIT, NULL) {
}


/**
 * The Destructor.
 */
HDBEditorQuitCmd::~HDBEditorQuitCmd() {
}


/**
 * Executes the command.
 */
bool
HDBEditorQuitCmd::Do() {

    wxGetApp().mainFrame().Close();

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
HDBEditorQuitCmd::icon() const {
    return "quit.png";
}


/**
 * Returns ID of this command.
 */
int
HDBEditorQuitCmd::id() const {
    return HDBEditorConstants::COMMAND_QUIT;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
HDBEditorQuitCmd*
HDBEditorQuitCmd::create() const {
    return new HDBEditorQuitCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
HDBEditorQuitCmd::isEnabled() {
    return true;
}
