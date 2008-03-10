/**
 * @file ClearDisassemblyHighlightCmd.cc
 *
 * Implementation of ClearDisassemblyHighlightCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ClearDisassemblyHighlightCmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ProximToolbox.hh"
#include "ProximDisassemblyWindow.hh"
#include "TracedSimulatorFrontend.hh"

/**
 * The Constructor.
 */
ClearDisassemblyHighlightCmd::ClearDisassemblyHighlightCmd():
    GUICommand("Clear Disassembly highlight", NULL) {

    }

/**
 * The Destructor.
 */
ClearDisassemblyHighlightCmd::~ClearDisassemblyHighlightCmd() {
}


/**
 * Executes the command.
 */
bool
ClearDisassemblyHighlightCmd::Do() {
    ProximDisassemblyWindow* window = ProximToolbox::disassemblyWindow();
    assert(window != NULL);
    window->setMoveAttrProvider(NULL);
    window->Refresh();
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ClearDisassemblyHighlightCmd::icon() const {
    return "clear_console.png";
}


/**
 * Returns ID of this command.
 */
int
ClearDisassemblyHighlightCmd::id() const {
    return ProximConstants::COMMAND_CLEAR_DISASSEMBLY_HIGHLIGHT;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ClearDisassemblyHighlightCmd*
ClearDisassemblyHighlightCmd::create() const {
    return new ClearDisassemblyHighlightCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the console window exists.
 */
bool
ClearDisassemblyHighlightCmd::isEnabled() {
    if (ProximToolbox::frontend()->isProgramLoaded()) {
	return true;
    } else {
	return false;
    }
}
