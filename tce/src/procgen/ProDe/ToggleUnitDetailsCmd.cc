/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file ToggleUnitDetailsCmd.cc
 *
 * Definition of ToggleUnitDetails class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/docview.h>

#include "ToggleUnitDetailsCmd.hh"
#include "MDFView.hh"
#include "MachineCanvas.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"
#include "MachineCanvasOptions.hh"

using std::string;

/**
 * The Constructor.
 */
ToggleUnitDetailsCmd::ToggleUnitDetailsCmd() :
    EditorCommand(ProDeConstants::CMD_NAME_TOGGLE_UNIT_DETAILS) {

}


/**
 * The Destructor.
 */
ToggleUnitDetailsCmd::~ToggleUnitDetailsCmd() {
}

/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ToggleUnitDetailsCmd::Do() {
    MDFView* mView = dynamic_cast<MDFView*>(view());
    MachineCanvas* canvas = mView->canvas();
    OptionValue& option = canvas->options().optionValue(
        MachineCanvasOptions::SHOW_UNIT_INFO_STRING);
    option.setBoolValue(!option.isFlagOn());
    canvas->updateMachine();
    return true;
}


/**
 * Returns state of the check-item related to this command.
 *
 * @return true, if the command check item is checked.
 */
bool
ToggleUnitDetailsCmd::isChecked() const {
    MDFView* mView = dynamic_cast<MDFView*>(view());
    if (mView == NULL) {
        return false;
    }
    MachineCanvas* canvas = mView->canvas();
    if (canvas == NULL) {
        return false;
    }
    OptionValue& option = canvas->options().optionValue(
        MachineCanvasOptions::SHOW_UNIT_INFO_STRING);
    return option.isFlagOn();
}



/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
ToggleUnitDetailsCmd::id() const {
    return ProDeConstants::COMMAND_TOGGLE_UNIT_DETAILS;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
ToggleUnitDetailsCmd*
ToggleUnitDetailsCmd::create() const {
    return new ToggleUnitDetailsCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
ToggleUnitDetailsCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_TOGGLE_UNIT_DETAILS;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
ToggleUnitDetailsCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
