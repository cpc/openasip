/**
 * @file UserManualCmd.cc
 *
 * Definition of UserManualCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
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
