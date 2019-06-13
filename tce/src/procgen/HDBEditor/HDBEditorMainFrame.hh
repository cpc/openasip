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
 * @file HDBEditorMainFrame.hh
 *
 * Declaration of the HDBEditorMainFrame class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
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
