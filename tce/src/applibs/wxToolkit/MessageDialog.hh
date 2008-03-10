/**
 * @file MessageDialog.hh
 *
 * Declaration of MessageDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi) 2005.
 * @note rating: red
 */

#ifndef TTA_MESSAGE_DIALOG_HH
#define TTA_MESSAGE_DIALOG_HH

#include <wx/wx.h>

/**
 * MessageDialog is base class for all dialogs diaplaying a simple text
 * message. MessageDialog extends wxMessageDialog functionality by
 * word wrapping long lines to multiple lines, and suppressing lines
 * of very long messages.
 */
class MessageDialog: public wxMessageDialog {
public:
    MessageDialog(
	wxWindow* parent,
	const wxString& title,
	const wxString& message,
	long style);

    virtual ~MessageDialog();

private:
    wxString wordWrap(const wxString& string, unsigned int lineWidth);
    wxString suppressLines(const wxString& string, int maxLines);
};
#endif
