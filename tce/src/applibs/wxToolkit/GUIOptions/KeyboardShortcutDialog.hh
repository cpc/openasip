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
