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
 * @file KeyboardShortcutDialog.hh
 * 
 * Declaration of the KeyboardShortcutDialog class.
 * 
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
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
    /// original shortcut
    KeyboardShortcut* oldShortcut_;
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
