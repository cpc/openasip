/**
 * @file ProDeExportCmd.cc
 *
 * Definition of ProDeExportCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
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
    wxString fileTypes = _T("Encapsulated Postscript (.eps)|*.eps|");
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

    if (extension == ".eps" || extension == ".epsi") {
        if (!canvas->saveEPS(filename, title, creator)) {
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
