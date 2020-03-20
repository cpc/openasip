/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file EditOptionsCmd.cc
 *
 * Implementation of EditOptionsCmd class.
 *
 * @author Veli-Pekka Jääskeläinen (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "EditOptionsCmd.hh"
#include "ProDeOptionsDialog.hh"
#include "ProDeConstants.hh"
#include "ProDeOptions.hh"
#include "ProDe.hh"
#include "MainFrame.hh"

using std::string;

/**
 * The Constructor.
 */
EditOptionsCmd::EditOptionsCmd():
    EditorCommand(ProDeConstants::CMD_NAME_EDIT_OPTIONS) {

}


/**
 * The Destructor.
 */
EditOptionsCmd::~EditOptionsCmd() {
}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
EditOptionsCmd::Do() {

    ProDeOptionsDialog dialog(
        parentWindow(), *wxGetApp().options(), *wxGetApp().commandRegistry());
    if (dialog.ShowModal() == wxID_OK) {
        if (wxGetApp().options()->toolbarVisibility()) {
            wxGetApp().mainFrame()->createToolbar();
            wxGetApp().mainFrame()->createMenubar();
        }
    }
    return true;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
EditOptionsCmd::id() const {
    return ProDeConstants::COMMAND_EDIT_OPTIONS;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
EditOptionsCmd*
EditOptionsCmd::create() const {
    return new EditOptionsCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
EditOptionsCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_EDIT_OPTIONS;
}


/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
EditOptionsCmd::icon() const {
    return ProDeConstants::CMD_ICON_EDIT_OPTIONS;
}


/**
 * This command is always executable.
 *
 * @return Always true.
 */
bool
EditOptionsCmd::isEnabled() {
    return true;
}
