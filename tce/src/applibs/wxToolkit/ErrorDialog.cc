/**
 * @file ErrorDialog.cc
 *
 * Implementation of the ErrorDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ErrorDialog.hh"

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param message Error message to show.
 */
ErrorDialog::ErrorDialog(wxWindow* parent, wxString message):
    MessageDialog(parent, _T("Error"), message,
    wxOK | wxICON_ERROR) {

}

/**
 * Destructor.
 */
ErrorDialog::~ErrorDialog() {
}
