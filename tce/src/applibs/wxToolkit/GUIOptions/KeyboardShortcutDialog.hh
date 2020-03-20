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
 * @file KeyboardShortcutDialog.hh
 * 
 * Declaration of the KeyboardShortcutDialog class.
 * 
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_KEYBOARD_SHORTCUT_DIALOG_HH
#define TTA_KEYBOARD_SHORTCUT_DIALOG_HH

#include <wx/wx.h>
#include <wx/statline.h>

class KeyboardShortcut;

/**
 * Dialog for defining keyboard shortcuts.
 */
class KeyboardShortcutDialog : public wxDialog {
public:
    KeyboardShortcutDialog(wxWindow* parent, KeyboardShortcut* shortcut);
    virtual ~KeyboardShortcutDialog();
   
private: 
    virtual bool TransferDataToWindow();
    wxSizer* createContents(wxWindow* parent, bool callFit, bool set_sizer);
    void onCharEvent(wxKeyEvent& event);
   
    /// modified shortcut
    KeyboardShortcut* shortcut_;
    /// shortcut text field widget
    wxStaticText* shortcutField_;
   
    /// enumerated IDs for dialog widgets
    enum {
        ID_SHORTCUT = 10000
    };

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};
#endif
