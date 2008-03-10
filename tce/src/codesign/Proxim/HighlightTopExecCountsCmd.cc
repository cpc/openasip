/**
 * @file HighlightTopExecCountsCmd.cc
 *
 * Implementation of HighlightTopExecCountsCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "HighlightTopExecCountsCmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "DisasmTopCountAttrProvider.hh"
#include "ProximToolbox.hh"
#include "ProximDisassemblyWindow.hh"
#include "TracedSimulatorFrontend.hh"

/**
 * The Constructor.
 */
HighlightTopExecCountsCmd::HighlightTopExecCountsCmd():
    GUICommand("Highlight Top Execution Counts", NULL) {

    }

/**
 * The Destructor.
 */
HighlightTopExecCountsCmd::~HighlightTopExecCountsCmd() {
}


/**
 * Executes the command.
 */
bool
HighlightTopExecCountsCmd::Do() {
    ProximDisassemblyWindow* window = ProximToolbox::disassemblyWindow();
    assert(window != NULL);
    DisasmTopCountAttrProvider* attrProvider = 
        new DisasmTopCountAttrProvider(*ProximToolbox::frontend(), 10);

    if (ProximToolbox::frontend()->isProgramLoaded()) {
        attrProvider->update();
    }
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
HighlightTopExecCountsCmd::icon() const {
    return "clear_console.png";
}


/**
 * Returns ID of this command.
 */
int
HighlightTopExecCountsCmd::id() const {
    return ProximConstants::COMMAND_HIGHLIGHT_TOP_EXEC_COUNTS;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
HighlightTopExecCountsCmd*
HighlightTopExecCountsCmd::create() const {
    return new HighlightTopExecCountsCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the console window exists.
 */
bool
HighlightTopExecCountsCmd::isEnabled() {
    if (ProximToolbox::frontend()->isProgramLoaded()) {
	return true;
    } else {
	return false;
    }
}
