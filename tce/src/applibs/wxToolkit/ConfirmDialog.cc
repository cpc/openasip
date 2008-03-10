/**
 * @file ConfirmDialog.cc
 *
 * Implementation of the ConfirmDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ConfirmDialog.hh"

using std::string;

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param message Confirm message to show.
 */
ConfirmDialog::ConfirmDialog(wxWindow* parent, const wxString message):
    wxMessageDialog(parent,message,_T("Confirm"),
                    wxYES_NO | wxCANCEL | wxICON_QUESTION) {

}


/**
 * Destructor.
 */
ConfirmDialog::~ConfirmDialog() {
}
