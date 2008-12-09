/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file OSEdMainFrame.hh
 *
 * Declaration of class OSEdMainFrame.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_MAIN_FRAME_HH
#define TTA_OSED_MAIN_FRAME_HH

#include <wx/wx.h>
#include <wx/splitter.h>

class CommandRegistry;
class OSEdTreeView;
class OSEdInformer;

/**
 * Main window of the application.
 *
 * Contains a splitted window for showing data and a menu bar for executing
 * actions.
 */
class OSEdMainFrame : public wxFrame {
public:
    OSEdMainFrame(
        const wxString& title,
        const wxPoint& pos,
        const wxSize& size);
    
    virtual ~OSEdMainFrame();
    
    OSEdTreeView* treeView() const;
    CommandRegistry* registry() const;
    OSEdInformer* informer() const;
    wxStatusBar* statusBar() const;

    void updateMenuBar();
    void onCommandEvent(wxCommandEvent& event);

private:
    /// Copying not allowed.
    OSEdMainFrame(const OSEdMainFrame&);
    /// Assignment not allowed.
    OSEdMainFrame& operator=(const OSEdMainFrame&);

    /// Command registry.
    CommandRegistry* registry_;
    /// Tree view.
    OSEdTreeView* treeView_;
    /// Informs listener classes for events that occurs.
    OSEdInformer* informer_;
    /// Status bar of the main window.
    wxStatusBar* statusBar_;
    
    DECLARE_EVENT_TABLE()
};

#endif
