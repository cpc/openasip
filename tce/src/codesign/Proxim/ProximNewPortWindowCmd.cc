/**
 * @file ProximNewPortWindowCmd.cc
 *
 * Implementation of ProximNewPortWindowCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximNewPortWindowCmd.hh"
#include "ProximConstants.hh"
#include "ProximPortWindow.hh"
#include "ProximToolbox.hh"

/**
 * The Constructor.
 */
ProximNewPortWindowCmd::ProximNewPortWindowCmd():
    GUICommand(ProximConstants::COMMAND_NAME_NEW_PORT_WINDOW, NULL) {

    }

/**
 * The Destructor.
 */
ProximNewPortWindowCmd::~ProximNewPortWindowCmd() {
}


/**
 * Executes the command.
 */
bool
ProximNewPortWindowCmd::Do() {

    ProximPortWindow* portWindow =
        new ProximPortWindow(ProximToolbox::mainFrame(), -1);

    ProximToolbox::addFramedWindow(
        portWindow, _T("Ports"), false, wxSize(270, 200));

    portWindow->GetParent()->SetSize(270, 300);

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximNewPortWindowCmd::icon() const {
    return "window_new.png";
}


/**
 * Returns ID of this command.
 */
int
ProximNewPortWindowCmd::id() const {
    return ProximConstants::COMMAND_NEW_PORT_WINDOW;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximNewPortWindowCmd*
ProximNewPortWindowCmd::create() const {
    return new ProximNewPortWindowCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximNewPortWindowCmd::isEnabled() {
    return true;
}
