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
 * @file ProDeExportCmd.cc
 *
 * Definition of ProDeExportCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/docview.h>

#include "MachineCanvas.hh"
#include "ProDeExportCmd.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"
#include "MDFView.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"
#include "FileSystem.hh"

#if wxCHECK_VERSION(3, 0, 0)
    #define wxSAVE wxFD_SAVE
    #define wxOVERWRITE_PROMPT wxFD_OVERWRITE_PROMPT
#endif

using std::string;

/**
 * The Constructor.
 */
ProDeExportCmd::ProDeExportCmd():
    EditorCommand(ProDeConstants::CMD_NAME_EXPORT) {

}


/**
 * The Destructor.
 */
ProDeExportCmd::~ProDeExportCmd() {}


/**
 * Executes the command.
 *
 * @return Always false. Returning true here would cause problems with the
 *         wxCommandProcessor, and the command is never undoable so it's
 *         safe to return false even if the command was succesfully executed.
 */
bool
ProDeExportCmd::Do() {

    wxView* view = wxGetApp().docManager()->GetCurrentView();
    MachineCanvas* canvas = dynamic_cast<MDFView*>(view)->canvas();

    assert(view != NULL && canvas != NULL);

    wxString message = _T("Export processor figure.");
    wxString defaultDir = _T(".");
    wxString defaultFile= _T("");
#if wxCHECK_VERSION(3, 0, 0)
    wxString fileTypes = _T("Scalable Vector Graphics (.svg)|*.svg|");
#else
    wxString fileTypes = _T("Encapsulated Postscript (.eps)|*.eps;*.epsi|");
#endif
    fileTypes.Append(_T("Portable Network Graphics (.png)|*.png"));

    wxFileDialog dialog(
        parentWindow(), message, defaultDir, defaultFile, fileTypes,
        wxSAVE | wxOVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_CANCEL) {
        return false;
    }

    std::string filename = WxConversion::toString(dialog.GetPath());
    std::string extension = FileSystem::fileExtension(filename);
    std::string creator = "TTA Processor Designer";
    std::string title =
        WxConversion::toString(view->GetDocument()->GetTitle());

#if wxCHECK_VERSION(3, 0, 0)
    if (extension == ".svg") {
        if (!canvas->saveSVG(filename)) {
#else
    if (extension == ".eps" || extension == ".epsi") {
        if (!canvas->saveEPS(filename, title, creator)) {
#endif
            wxString message = _T("Error saving file '");
            message.Append(dialog.GetPath());
            message.Append(_T("'."));
            ErrorDialog errorDialog(parentWindow(), message);
            errorDialog.ShowModal();
        }
    } else if (extension == ".png") {
        if (!canvas->savePNG(filename)) {
            wxString message = _T("Error saving file '");
            message.Append(dialog.GetPath());
            message.Append(_T("'."));
            ErrorDialog errorDialog(parentWindow(), message);
            errorDialog.ShowModal();
        }
    } else {
        wxString message = _T("File type with extension '");
        message.Append(WxConversion::toWxString(extension));
        message.Append(_T("' is not supported."));
        ErrorDialog dialog(parentWindow(), message);
        dialog.ShowModal();
        return false;
    }

    return false;
}

/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
ProDeExportCmd::id() const {
    return ProDeConstants::COMMAND_EXPORT;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
ProDeExportCmd*
ProDeExportCmd::create() const {
    return new ProDeExportCmd();
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
ProDeExportCmd::isEnabled() {
    wxView* view = wxGetApp().docManager()->GetCurrentView();
    if (view != NULL) {
	return true;
    }
    return false;
}
