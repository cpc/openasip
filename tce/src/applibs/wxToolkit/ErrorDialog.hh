/**
 * @file ErrorDialog.hh
 *
 * Declaration of ErrorDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ERROR_DIALOG_HH
#define TTA_ERROR_DIALOG_HH

#include <wx/wx.h>
#include "MessageDialog.hh"

/**
 * Simple error dialog for displaying short error messages.
 */
class ErrorDialog: public MessageDialog {
public:
    ErrorDialog(wxWindow* parent, wxString message);
    virtual ~ErrorDialog();
};

#endif
