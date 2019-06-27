/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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
    void OnExit(wxCommandEvent& WXUNUSED(event));

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
