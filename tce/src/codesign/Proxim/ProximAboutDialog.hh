/**
 * @file ProximAboutDialog.hh
 * 
 * Declaration of ProximAboutDialog class.
 * 
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_ABOUT_DIALOG_HH
#define TTA_PROXIM_ABOUT_DIALOG_HH

#include <wx/wx.h>
#include <wx/statline.h>

/**
 * About dialog for Proxim.
 */
class ProximAboutDialog : public wxDialog {
public:
    ProximAboutDialog(wxWindow* parent);
    virtual ~ProximAboutDialog();
   
private: 
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// Widget IDs.
    enum {
	ID_TEXT_PROGRAM_NAME = 10000,
	ID_TEXT_PROGRAM_VERSION,
    ID_TEXT_PROGRAM_COPYRIGHT,
	ID_LINE
    };
};
#endif
