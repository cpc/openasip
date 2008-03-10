/**
 * @file OSEdAboutDialog.hh
 *
 * Declaration of OSEdAboutDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_ABOUT_DIALOG_HH
#define TTA_OSED_ABOUT_DIALOG_HH

#include <wx/wx.h>

/**
 * About dialog of the application.
 */
class OSEdAboutDialog : public wxDialog {
public:
    explicit OSEdAboutDialog(wxWindow* parent);
    virtual ~OSEdAboutDialog();

private:
    /// Copying not allowed.
    OSEdAboutDialog(const OSEdAboutDialog&);
    /// Assignment not allowed.
    OSEdAboutDialog& operator=(const OSEdAboutDialog&);
    
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void setTexts();

    /**
     * Widget ids.
     */
    enum {
        ID_ABOUT_TEXT = 1000010,
        ID_LINE
    };
};

#endif
