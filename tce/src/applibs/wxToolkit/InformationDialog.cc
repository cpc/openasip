/**
 * @file InformationDialog.cc
 *
 * Implementation of the InformationDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "InformationDialog.hh"

using std::string;

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param message Information message to show.
 */
InformationDialog::InformationDialog(wxWindow* parent,const wxString message):
    MessageDialog(parent, _T("Information"), message,
    wxOK | wxICON_INFORMATION) {

}


/**
 * Destructor.
 */
InformationDialog::~InformationDialog() {
}
