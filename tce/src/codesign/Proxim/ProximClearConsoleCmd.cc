/**
 * @file ProximClearConsoleCmd.cc
 *
 * Implementation of ProximClearConsoleCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximClearConsoleCmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ConsoleWindow.hh"


/**
 * The Constructor.
 */
ProximClearConsoleCmd::ProximClearConsoleCmd():
    GUICommand("Clear Console", NULL) {

    }

/**
 * The Destructor.
 */
ProximClearConsoleCmd::~ProximClearConsoleCmd() {
}


/**
 * Executes the command.
 */
bool
ProximClearConsoleCmd::Do() {
    wxWindow* window = wxGetApp().GetTopWindow()->FindWindow(
        ProximConstants::ID_CONSOLE_WINDOW);
    assert(window != NULL);
    ConsoleWindow* console = dynamic_cast<ConsoleWindow*>(window);
    assert(console != NULL);
    console->clear();
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximClearConsoleCmd::icon() const {
    return "clear_console.png";
}


/**
 * Returns ID of this command.
 */
int
ProximClearConsoleCmd::id() const {
    return ProximConstants::COMMAND_CLEAR_CONSOLE;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximClearConsoleCmd*
ProximClearConsoleCmd::create() const {
    return new ProximClearConsoleCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the console window exists.
 */
bool
ProximClearConsoleCmd::isEnabled() {

    wxWindow* window = wxGetApp().GetTopWindow()->FindWindow(
        ProximConstants::ID_CONSOLE_WINDOW);

    if (window == NULL) {
	return false;
    } else {
	return true;
    }
}
