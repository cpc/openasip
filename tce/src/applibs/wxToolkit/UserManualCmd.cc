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
 * @file UserManualCmd.cc
 *
 * Definition of UserManualCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 */

#include <wx/mimetype.h>
#include <wx/arrstr.h>
#include <string>

#include "UserManualCmd.hh"
#include "WxConversion.hh"
#include "FileSystem.hh"
#include "ErrorDialog.hh"
#include "WarningDialog.hh"
#include "WidgetTools.hh"
#include "StringTools.hh"

#if wxCHECK_VERSION(3, 0, 0)
    #define wxOPEN wxFD_OPEN
#endif

using std::string;

bool UserManualCmd::mimeTypesManagerInitialized_ = false;

const std::string UserManualCmd::COMMAND_NAME = "User Manual";
const std::string UserManualCmd::COMMAND_ICON = "help.png";
const std::string UserManualCmd::COMMAND_SHORT_NAME = "Help";
const int UserManualCmd::COMMAND_ID = 30000;

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

    std::string cmdStr;
    wxString cmd;
    if (ft != NULL) {
        cmd = ft->GetOpenCommand(manual);
        delete ft;
        ft = NULL;
        cmdStr = std::string(cmd.ToAscii());
    }

    // Stupid hack for now. For fedora some reason this cmd would end up
    // being 'gimp' which is not very nice PDF reader ;)
    // on ubuntu this could be gv which cannot view PDF's.
    if (!(cmdStr.find("evince") != std::string::npos || 
          cmdStr.find("kpdf") != std::string::npos || 
          cmdStr.find("kghostview") != std::string::npos || 
          cmdStr.find("acroread") != std::string::npos || 
          cmdStr.find("okular") != std::string::npos)) {
        cmdStr = "";
    }
    if (cmdStr == "") {
        // if couldn't get viewer from mime types test if some usual
        // default viewer is found
        std::vector<std::string> viewers;
        viewers.push_back("/usr/bin/evince");
        viewers.push_back("/usr/bin/kpdf");
        viewers.push_back("/usr/bin/kghostview");
        viewers.push_back("/usr/bin/acroread");
        viewers.push_back("/usr/bin/okular");
        for (unsigned int i = 0; i < viewers.size(); ++i) {
            if (FileSystem::fileIsExecutable(viewers.at(i))) {
                wxExecute(
                    wxString::FromAscii(
                        (viewers.at(i) + " ").c_str()) + manual);
                return true;
            }
        }

        // if none of above viewers were found try to find from PATH env
        // variable
        viewers.clear();
        std::string DS = FileSystem::DIRECTORY_SEPARATOR;
        viewers.push_back("evince");
        viewers.push_back("kpdf");
        viewers.push_back("kghostview");
        viewers.push_back("acroread");
        viewers.push_back("okular");

        std::vector<std::string> paths;
        std::string pathsEnv = Environment::environmentVariable("PATH");
        StringTools::chopString(pathsEnv, ":", paths);

        for (unsigned int i = 0; i < paths.size(); ++i) {
            for (unsigned int j = 0; j < viewers.size(); ++j) {
                std::string viewer = paths.at(i) + DS + viewers.at(j);
                if (FileSystem::fileIsExecutable(viewer)) {
                    wxExecute(
                        wxString::FromAscii(
                            (viewers.at(i) + " ").c_str()) + manual);
                    return true;
                }
            }
        }
        // did not find any viewer from path or default locations

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
