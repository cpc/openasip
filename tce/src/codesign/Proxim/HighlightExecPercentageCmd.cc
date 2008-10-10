/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file HighlightExecPercentageCmd.cc
 *
 * Implementation of HighlightExecPercentageCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
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
