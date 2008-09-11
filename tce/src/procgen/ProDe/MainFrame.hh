/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file MainFrame.hh
 *
 * Declaration of the MainFrame class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
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
