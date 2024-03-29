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
 * @file KeyboardShortcutDialog.cc
 * 
 * Definition of KeyboardShortcutDialog class.
 * 
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>

#include "KeyboardShortcutDialog.hh"
#include "KeyboardShortcut.hh"
#include "WxConversion.hh"
#include "Conversion.hh"
#include "Application.hh"

using std::string;


BEGIN_EVENT_TABLE(KeyboardShortcutDialog, wxDialog)
    EVT_CHAR(KeyboardShortcutDialog::onCharEvent)
    EVT_KEY_DOWN(KeyboardShortcutDialog::onCharEvent)
END_EVENT_TABLE()


/**
 * The constructor.
 *
 * @param parent Parent window of the dialog.
 * @param shortcut Shortcut to modify.
 */
KeyboardShortcutDialog::KeyboardShortcutDialog(
    wxWindow* parent, KeyboardShortcut* shortcut):
    wxDialog(parent, -1, _T("Edit keyboard shortcut"), wxDefaultPosition),
    shortcut_(shortcut) {
    
    assert(shortcut != NULL);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxPanel* panel = new wxPanel(this, -1);
    createContents(panel, true, true);
    sizer->Add(panel);
    SetSizer(sizer);
    sizer->Fit(this);
    FindWindow(wxID_OK)->Enable(false);
    panel->SetFocus();
}
    

/**
 * The Destructor.
 */
KeyboardShortcutDialog::~KeyboardShortcutDialog() {
}


/**
 * Transfers data from dialog attributes to dialog widgets.
 * 
 * @return True, if the data was succesfully transfered, false othwerwise.
 */
bool
KeyboardShortcutDialog::TransferDataToWindow() {
    
    string keyName = "";
    
    // set the name of the key
    if (shortcut_->key() > 32 && shortcut_->key() < 127) {
        // character key
        keyName = Conversion::toString(shortcut_->key());
    } else if (shortcut_->key() == 127) {
        // delete key
        keyName = "DEL";
    } else if (shortcut_->fKey() != 0) {
        // function key
        keyName = "F"+Conversion::toString(shortcut_->fKey());
    }
    
    wxString key = WxConversion::toWxString(keyName);
    
    // prepend key modifiers
    if (shortcut_->alt()) {
        key.Prepend(_T("ALT - "));
    }
    if (shortcut_->ctrl()) {
        key.Prepend(_T("CTRL - "));
    }
    
    shortcutField_->SetLabel(key);

    return true;
}



/**
 * Sets a new shortcut for the command.
 * 
 * @param event Keyevent of the new shortcut.
 */
void
KeyboardShortcutDialog::onCharEvent(wxKeyEvent& event) {
    
    int keycode = event.GetKeyCode();

    PRINT_VAR(keycode);
    // check that key code is valid
    if (!((keycode >= int('0') && keycode <= int('9')) ||
          (keycode >= int('A') && keycode <= int('Z')) ||
          (keycode == 127) ||
          (keycode >= WXK_F1 && keycode <= WXK_F12))) {
        
        return;
    }
    
    // Check that character key shortcut has at least control
    // or alt modifier.
    if (keycode < 256 &&
        !(event.AltDown() || event.ControlDown())) {
        return;
    }
    
    shortcut_->setAlt(event.AltDown());
    shortcut_->setCtrl(event.ControlDown());
    
    if (keycode < 256) {
        // character key shortcut
        shortcut_->setFKey(0);
        shortcut_->setKey(keycode);
    } else if(keycode >= WXK_F1 && keycode <= WXK_F12) {
        // function key shortcut
        shortcut_->setKey(0);
        shortcut_->setFKey(keycode - WXK_F1 + 1);
    } else {
        // invalid shortcut
        assert(false);
    }
    
    FindWindow(wxID_OK)->Enable(true);
    TransferDataToWindow();
}


/**
 * Creates the dialog contents.
 * 
 * This function was initially generated by wxDesigner.
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
KeyboardShortcutDialog::createContents(			       
    wxWindow *parent, bool call_fit, bool set_sizer) {
    
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    wxStaticText *item1 = new wxStaticText( parent, -1, 
        wxT("Redefine the shortcut by pressing\n"
        "keys for the new key combination."),
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 10 );
    wxStaticBox *item3 = new wxStaticBox( parent, -1, wxT("Shortcut:") );
    wxStaticBoxSizer *item2 = new wxStaticBoxSizer( item3, wxHORIZONTAL );

    shortcutField_ =
	 new wxStaticText(parent, ID_SHORTCUT, wxT(""), wxDefaultPosition,
			  wxDefaultSize, 0);
    item2->Add(shortcutField_, 0, wxALIGN_CENTER|wxALL, 5 );
    item0->Add( item2, 0, wxGROW|wxALL, 5 );
    wxStaticLine *item5 =
	new wxStaticLine(parent, -1, wxDefaultPosition, wxSize(20,-1),
			 wxLI_HORIZONTAL );
    item0->Add( item5, 0, wxGROW|wxALL, 5 );
    wxBoxSizer *item6 = new wxBoxSizer( wxHORIZONTAL );
    wxButton *item7 =
	new wxButton(parent, wxID_OK, wxT("OK"), wxDefaultPosition,
		     wxDefaultSize, 0);
    item6->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );
    wxButton *item8 =
	new wxButton(parent, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition,
		     wxDefaultSize, 0);
    item6->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );
    item0->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer) {
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0 );
        if (call_fit)
	{
            item0->Fit( parent );
            item0->SetSizeHints( parent );
        }
    }
    
    return item0;
}
