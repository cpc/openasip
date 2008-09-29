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
 * @file UserManualCmd.cc
 *
 * Definition of UserManualCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 */

#include <wx/mimetype.h>
#include <string>

#include "UserManualCmd.hh"
#include "WxConversion.hh"
#include "FileSystem.hh"
#include "ErrorDialog.hh"
#include "WarningDialog.hh"
#include "WidgetTools.hh"

using std::string;

bool UserManualCmd::mimeTypesManagerInitialized_ = false;

const std::string UserManualCmd::COMMAND_NAME = "User Manual";
const std::string UserManualCmd::COMMAND_ICON = "help.png";
const std::string UserManualCmd::COMMAND_SHORT_NAME = "Help";
const int UserManualCmd::COMMAND_ID = 200000;

/**
 * The Constructor.
 */
UserManualCmd::UserManualCmd():
    GUICommand(COMMAND_NAME, NULL) {
}


/**
 * The Destructor.
 */
UserManualCmd::~UserManualCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
UserManualCmd::Do() {


    if (!mimeTypesManagerInitialized_) {
        wxTheMimeTypesManager->Initialize();
        mimeTypesManagerInitialized_ = true;
    }

    wxString manual = WxConversion::toWxString(Environment::pdfManual());
    wxString extension = manual.AfterLast(_T('.'));
    wxFileType* ft =
        wxTheMimeTypesManager->GetFileTypeFromExtension(extension);

    if (ft == NULL) {
        return askFromUser();
    }

    wxString cmd = ft->GetOpenCommand(manual);
    delete ft;
    ft = NULL;

    if (cmd.IsEmpty()) {
        return askFromUser();
    } else {
        wxExecute(cmd);
    }
    
    return true;
}


/**
 * Returns id of this command.
 */
int
UserManualCmd::id() const {
    return COMMAND_ID;
}


/**
 * Creates and returns a new instance of this command.
 */
UserManualCmd*
UserManualCmd::create() const {
    return new UserManualCmd();
}


/**
 * Returns short version of the command name.
 */
string
UserManualCmd::shortName() const {
    return COMMAND_SHORT_NAME;
}


/**
 * Returns path to the command's icon file.
 */
string
UserManualCmd::icon() const {
    return COMMAND_ICON;
}


/**
 * This command is always executable.
 *
 * @return Always true.
 */
bool
UserManualCmd::isEnabled() {
    return true;
}


/**
 * Fall back  function which is called if a default pdf viewer is not found.
 *
 * @return True, if the user chose a pdf viewer.
 */
bool
UserManualCmd::askFromUser() {

    wxString manual = WxConversion::toWxString(Environment::pdfManual());
    wxString extension = manual.AfterLast(_T('.'));
    wxString message = _T("No default PDF viewer found.");
    message.Append(_T("Please select  an executable program to view PDF "));
    message.Append(_T("files with."));
    assert(parentWindow() != NULL);
    ErrorDialog errorDialog(parentWindow(), message);
    errorDialog.ShowModal();

    wxFileDialog execDialog(
        parentWindow(), _T("Choose a pdf viewer executable"), _T(""), _T(""),
        _T("*"), wxOPEN);

    if (execDialog.ShowModal() == wxID_OK) {
        wxString command = execDialog.GetPath();
        command.Append(_T(" "));
        command.Append(manual);
        wxExecute(command);
        return true;
    }

    return false;
}
