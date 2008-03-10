/**
 * @file ProximNewRegisterWindowCmd.cc
 *
 * Implementation of ProximNewRegisterWindowCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximNewRegisterWindowCmd.hh"
#include "ProximConstants.hh"
#include "ProximRegisterWindow.hh"
#include "ProximToolbox.hh"

/**
 * The Constructor.
 */
ProximNewRegisterWindowCmd::ProximNewRegisterWindowCmd():
    GUICommand(ProximConstants::COMMAND_NAME_NEW_REGISTER_WINDOW, NULL) {

    }

/**
 * The Destructor.
 */
ProximNewRegisterWindowCmd::~ProximNewRegisterWindowCmd() {
}


/**
 * Executes the command.
 */
bool
ProximNewRegisterWindowCmd::Do() {

    ProximRegisterWindow* regWindow = new ProximRegisterWindow(
        ProximToolbox::mainFrame(), -1);

    ProximToolbox::addFramedWindow(
        regWindow, _T("Registers"), false, wxSize(270, 200));

    regWindow->GetParent()->SetSize(270, 300);

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximNewRegisterWindowCmd::icon() const {
    return "window_new.png";
}


/**
 * Returns ID of this command.
 */
int
ProximNewRegisterWindowCmd::id() const {
    return ProximConstants::COMMAND_NEW_REGISTER_WINDOW;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximNewRegisterWindowCmd*
ProximNewRegisterWindowCmd::create() const {
    return new ProximNewRegisterWindowCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximNewRegisterWindowCmd::isEnabled() {
    return true;
}
