/**
 * @file PrintCmd.cc
 *
 * Definition of PrintCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include <wx/docview.h>
#include <string>

#include "config.h"
#include "PrintCmd.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"
#include "ErrorDialog.hh"

using std::string;


/**
 * The Constructor.
 */
PrintCmd::PrintCmd():
    EditorCommand(ProDeConstants::CMD_NAME_PRINT) {

}


/**
 * The Destructor.
 */
PrintCmd::~PrintCmd() {
}


/**
 * Executes the command.
 *
 * If the wxWidgets printing framework was not compiled in the wxWidgets
 * library, printing is not possible. An error message will be displayed
 * instead of printing.
 *
 * @return Always false. The command is not undoable.
 */
bool
PrintCmd::Do() {

#if WX_PRINTING_DISABLED

    // wxWidgets was compiled without printing framework, display an
    // error message.

    wxString message =
        wxString(_T("Printing framework is disabled in the wxWidgets\n"));

    message.Append(_T("library. Print previewing is not possible."));

    ErrorDialog error(parentWindow(), message);
    error.ShowModal();
    return false;

#else

    // wxWidgets was compiled with the printing framework, create printout
    // of the active view and print it.

    // find the active view
    wxView* view = wxGetApp().docManager()->GetCurrentView();
    if (view == NULL) {
        return false;
    }

    // create a printout of the active view
    wxPrintout* printout = view->OnCreatePrintout();

    if (printout != NULL) {
        wxPrinter printer;
        printer.Print(view->GetFrame(), printout, true);

        delete printout;
    }

    return false;

#endif // WX_PRINTING_DISABLED

}


/**
 * Return id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
PrintCmd::id() const {
    return ProDeConstants::COMMAND_PRINT;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
PrintCmd*
PrintCmd::create() const {
    return new PrintCmd();
}


/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
PrintCmd::icon() const {
    return ProDeConstants::CMD_ICON_PRINT;
}


/**
 * Returns true when command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if the command is executable.
 */
bool
PrintCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
        return true;
    }
    return false;
}
