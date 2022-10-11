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
 * @file QuitCmd.cc
 *
 * Definition of QuitCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
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
