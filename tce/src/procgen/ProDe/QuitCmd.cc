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
 * @file QuitCmd.cc
 *
 * Definition of QuitCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <wx/docmdi.h>

#include "QuitCmd.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"
#include "ProDeOptions.hh"
#include "ConfirmDialog.hh"
#include "CommandRegistry.hh"
#include "WxConversion.hh"
#include "MainFrame.hh"

using std::string;

/**
 * The Constructor.
 */
QuitCmd::QuitCmd():
    EditorCommand(ProDeConstants::CMD_NAME_QUIT) {

}


/**
 * The Destructor.
 */
QuitCmd::~QuitCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
QuitCmd::Do() {

    MainFrame* parent = wxGetApp().mainFrame();
    ProDeOptions* options = wxGetApp().options();
    if (options->isModified()) {
        wxString question = _T("Options are modified. ");
        if (options->hasFileName()) {
            question.Append(_T("Do you want to save changes to '"));
            question.Append(WxConversion::toWxString(
                                options->fileName()));
            question.Append(_T("'?"));
        } else {
            question.Append(_T("Do you want to save changes?"));
        }
        ConfirmDialog confirm(parentWindow(), question);
        int buttonPressed = confirm.ShowModal();
        if (buttonPressed == wxID_YES) {
            GUICommand* command = wxGetApp().commandRegistry()
                ->createCommand(ProDeConstants::COMMAND_SAVE_OPTIONS);
            if (!command->Do()) {
                return false;
            }
        } else if (buttonPressed == wxID_CANCEL) {
            return false;
        }
        // if 'No' button was pressed just exit the program
    }

    wxCommandEvent dummy(wxID_EXIT, 0);
    parent->OnExit(dummy);
    return false;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
QuitCmd::id() const {
    return ProDeConstants::COMMAND_QUIT;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
QuitCmd*
QuitCmd::create() const {
    return new QuitCmd();
}


/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
QuitCmd::icon() const {
    return ProDeConstants::CMD_ICON_QUIT;
}


/**
 * This command is always executable.
 *
 * @return Always true.
 */
bool
QuitCmd::isEnabled() {
    return true;
}
