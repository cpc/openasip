/**
 * @file InformationDialog.hh
 *
 * Declaration of InformationDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INFORMATION_DIALOG_HH
#define TTA_INFORMATION_DIALOG_HH

#include <wx/wx.h>
#include "MessageDialog.hh"

/**
 * Simple information dialog for displaying short informative messages.
 */
class InformationDialog: public MessageDialog {
public:
    InformationDialog(wxWindow* parent, const wxString message);
    virtual ~InformationDialog();
};

#endif
