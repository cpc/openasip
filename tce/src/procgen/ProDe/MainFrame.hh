/**
 * @file MainFrame.hh
 *
 * Declaration of the MainFrame class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MAIN_FRAME_HH
#define TTA_MAIN_FRAME_HH

#include <wx/docmdi.h>
#include <wx/menu.h>

class CommandRegistry;

/**
 * Main window of the editor.
 *
 * Contains a default menubar, toolbar, statusbar and the child frames
 * for all open documents. Derived from the wxDocMDIParentFrame class,
 * which handles the child frames automatically. Handles the Options
 * and the Help menu events and the events for toggling visibility of
 * the menubar and the toolbar.
 */
class MainFrame : public wxDocMDIParentFrame {
public:
    MainFrame(
        wxDocManager* manager, wxFrame* parent, const wxString& title,
        const wxPoint& position, const wxSize& size, long style);
    void onKeyEvent(wxKeyEvent& event);
    void createToolbar();
    void createMenubar();
    void onToggleToolbar();
    void onToggleStatusbar();
    void updateUI();

private:
    wxMenu* createOptionsMenu();
    wxMenu* createHelpMenu();
    void onCommandEvent(wxCommandEvent& event);
    void onUpdateUI(wxUpdateUIEvent& event);

    wxString menuAccelerator(int id);

    /// Command registry.
    CommandRegistry* commandRegistry_;

    /// Toolbar.
    wxToolBar* toolbar_;

    /// GUI element IDs.
    enum {
        ID_ADD_SUBMENU = 12000,
        ID_ADD_FROM_HDB_SUBMENU,
        ID_ZOOM_SUBMENU,
        ID_VIEW_TOOLBAR,
        ID_VIEW_STATUSBAR
    };

    /// Event table of the MainFrame.
    DECLARE_EVENT_TABLE()
};
#endif
