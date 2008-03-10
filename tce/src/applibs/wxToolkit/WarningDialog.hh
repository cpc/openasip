/**
 * @file WarningDialog.hh
 *
 * Declaration of WarningDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_WARNING_DIALOG_HH
#define TTA_WARNING_DIALOG_HH

#include <wx/wx.h>
#include "MessageDialog.hh"

/**
 * Simple warning dialog for displaying short warning messages.
 */
class WarningDialog: public MessageDialog {
public:
    WarningDialog(wxWindow* parent, const wxString message);
    virtual ~WarningDialog();
};

#endif
