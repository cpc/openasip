/**
 * @file ConfirmDialog.hh
 *
 * Declaration of ConfirmDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONFIRM_DIALOG_HH
#define TTA_CONFIRM_DIALOG_HH

#include <wx/wx.h>

/**
 * Simple confirm dialog for displaying short confirmation messages.
 */
class ConfirmDialog: public wxMessageDialog {
public:
    ConfirmDialog(wxWindow* parent, const wxString message);
    virtual ~ConfirmDialog();
};

#endif
