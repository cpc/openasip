/**
 * @file AboutDialog.hh
 *
 * Declaration of AboutDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_ABOUT_DIALOG_HH
#define TTA_ABOUT_DIALOG_HH

#include <wx/wx.h>
#include <wx/statline.h>

/**
 * Dialog with editor logo, name, version and copyright information.
 */
class AboutDialog : public wxDialog {
public:
    AboutDialog(wxWindow* parent);
    virtual ~AboutDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void setTexts();
};
#endif
