/**
 * @file WarningDialog.cc
 *
 * Implementation of the WarningDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "WarningDialog.hh"

using std::string;

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param message Warning message to show.
 */
WarningDialog::WarningDialog(wxWindow* parent, const wxString message):
    MessageDialog(parent, _T("Warning"), message, wxOK|wxICON_EXCLAMATION) {

}


/**
 * Destructor.
 */
WarningDialog::~WarningDialog() {
}
