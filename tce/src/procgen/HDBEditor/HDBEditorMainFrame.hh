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
 * @file HDBEditorMainFrame.hh
 *
 * Declaration of the HDBEditorMainFrame class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_EDITOR_MAIN_FRAME_HH
#define TTA_HDB_EDITOR_MAIN_FRAME_HH

class CommandRegistry;
class HDBBrowserWindow;

#include <wx/frame.h>
#include <string>

namespace HDB {
    class HDBManager;
    class CachedHDBManager;
}

/**
 * Main window of the editor. 
 */
class HDBEditorMainFrame : public wxFrame {
public:
    HDBEditorMainFrame(
        const wxString& title, const wxPoint& position, const wxSize& size);
    virtual ~HDBEditorMainFrame();

    bool setHDB(const std::string& hdbFile);
    bool createHDB(const std::string& filePath);

    HDB::HDBManager* hdbManager();
    HDBBrowserWindow* browser() const;

    void update();

private:
    void onCommandEvent(wxCommandEvent& event);
    void onUpdateUI(wxUpdateUIEvent& event);

    /// Command registry.
    CommandRegistry* commandRegistry_;

    HDB::CachedHDBManager* hdb_;
    HDBBrowserWindow* browser_;

    /// Event table of the MainFrame.
    DECLARE_EVENT_TABLE()
};
#endif
