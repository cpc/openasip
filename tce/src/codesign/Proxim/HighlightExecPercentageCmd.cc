/**
 * @file HighlightExecPercentageCmd.cc
 *
 * Implementation of HighlightExecPercentageCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "HighlightExecPercentageCmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "DisasmExecPercentageAttrProvider.hh"
#include "ProximToolbox.hh"
#include "ProximDisassemblyWindow.hh"
#include "TracedSimulatorFrontend.hh"

/**
 * The Constructor.
 */
HighlightExecPercentageCmd::HighlightExecPercentageCmd():
    GUICommand("Highlight Execution Percentage", NULL) {

    }

/**
 * The Destructor.
 */
HighlightExecPercentageCmd::~HighlightExecPercentageCmd() {
}


/**
 * Executes the command.
 */
bool
HighlightExecPercentageCmd::Do() {
    ProximDisassemblyWindow* window = ProximToolbox::disassemblyWindow();
    assert(window != NULL);
    DisasmExecPercentageAttrProvider* attrProvider = 
        new DisasmExecPercentageAttrProvider(*ProximToolbox::frontend());
    window->setMoveAttrProvider(attrProvider);
    window->Refresh();
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
HighlightExecPercentageCmd::icon() const {
    return "clear_console.png";
}


/**
 * Returns ID of this command.
 */
int
HighlightExecPercentageCmd::id() const {
    return ProximConstants::COMMAND_HIGHLIGHT_EXEC_PERCENTAGE;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
HighlightExecPercentageCmd*
HighlightExecPercentageCmd::create() const {
    return new HighlightExecPercentageCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the console window exists.
 */
bool
HighlightExecPercentageCmd::isEnabled() {
    if (ProximToolbox::frontend()->isProgramLoaded()) {
	return true;
    } else {
	return false;
    }
}
