/**
 * @file HelpBrowser.hh
 *
 * Declaration of the HelpBrowser class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_HELP_BROWSER
#define TTA_HELP_BROWSER

#include <wx/frame.h>

class wxHtmlWindow;

/**
 * Simple window for displaying html-pages.
 */
class HelpBrowser : public wxFrame {
public:
    HelpBrowser(wxString title, wxString file,
                wxPoint position, wxSize size);

    ~HelpBrowser();

private:
    void onClose(wxCloseEvent& event);
    void onMenuClose(wxCommandEvent& event);
    void onBack(wxCommandEvent& event);
    void onForward(wxCommandEvent& event);

    wxHtmlWindow* html_;

    /// IDs form menu items.
    enum {
        ID_BACK = 10000,
        ID_FORWARD
    };

    DECLARE_EVENT_TABLE()
};

#endif
