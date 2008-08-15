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
 * @file OptionsDialog.cc
 * 
 * Implementation of OptionsDialog class.
 * 
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/valgen.h>
#include <wx/spinctrl.h>
#include <boost/format.hpp>

#include "OptionsDialog.hh"
#include "GUIOptions.hh"
#include "WxConversion.hh"
#include "CommandRegistry.hh"
#include "GUICommand.hh"
#include "ErrorDialog.hh"
#include "Conversion.hh"
#include "ToolbarButton.hh"
#include "KeyboardShortcutDialog.hh"
#include "WidgetTools.hh"
#include "GUITextGenerator.hh"

using boost::format;
using std::string;
using std::vector;

const wxString OptionsDialog::COMMAND_COLUMN_TITLE = _T("Command");
const wxString OptionsDialog::SHORTCUT_COLUMN_TITLE = _T("Shortcut");
const wxString OptionsDialog::TOOLBAR_BUTTONS_COLUMN_TITLE = _T("Toolbar");
const wxString OptionsDialog::AVAILABLE_COMMANDS_COLUMN_TITLE = _T("Commands");
const wxString OptionsDialog::CONTENTS_ICONS = _T("Icons");
const wxString OptionsDialog::CONTENTS_BOTH = _T("Both");
const wxString OptionsDialog::CONTENTS_TEXT = _T("Text");
const wxString OptionsDialog::EDIT_BUTTON_LABEL = _T("Edit...");
const wxString OptionsDialog::DELETE_BUTTON_LABEL = _T("Delete");

BEGIN_EVENT_TABLE(OptionsDialog, wxDialog)
    EVT_BUTTON(ID_DELETE_KB_SC, OptionsDialog::onDeleteShortcut)
    EVT_BUTTON(ID_EDIT_KB_SC, OptionsDialog::onEditShortcut)
    EVT_BUTTON(ID_TOOL_INSERT, OptionsDialog::onInsertTool)
    EVT_BUTTON(ID_TOOL_REMOVE, OptionsDialog::onRemoveTool)
    EVT_BUTTON(ID_TOOL_UP, OptionsDialog::onMoveTool)
    EVT_BUTTON(ID_TOOL_DOWN, OptionsDialog::onMoveTool)
    EVT_BUTTON(wxID_OK, OptionsDialog::onOK)
    EVT_BUTTON(ID_HELP, OptionsDialog::onHelp)

    EVT_LIST_ITEM_FOCUSED(ID_KB_SC_LIST, OptionsDialog::onShortcutSelection)
    EVT_LIST_DELETE_ITEM(ID_KB_SC_LIST, OptionsDialog::onShortcutSelection)
    EVT_LIST_ITEM_SELECTED(ID_KB_SC_LIST, OptionsDialog::onShortcutSelection)
    EVT_LIST_ITEM_DESELECTED(ID_KB_SC_LIST, OptionsDialog::onShortcutSelection)
    EVT_LIST_ITEM_ACTIVATED(ID_KB_SC_LIST, OptionsDialog::onActivateShortcut)
    EVT_MENU(ID_EDIT_KB_SC, OptionsDialog::onEditShortcut)
    EVT_MENU(ID_DELETE_KB_SC, OptionsDialog::onDeleteShortcut)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_KB_SC_LIST, OptionsDialog::onShortcutRightClick)

    EVT_LIST_ITEM_FOCUSED(ID_TOOLBAR_LIST, OptionsDialog::onToolbarSelection)
    EVT_LIST_DELETE_ITEM(ID_TOOLBAR_LIST, OptionsDialog::onToolbarSelection)
    EVT_LIST_ITEM_SELECTED(ID_TOOLBAR_LIST, OptionsDialog::onToolbarSelection)
    EVT_LIST_ITEM_DESELECTED(ID_TOOLBAR_LIST, OptionsDialog::onToolbarSelection)

    EVT_LIST_ITEM_FOCUSED(ID_COMMAND_LIST, OptionsDialog::onCommandSelection)
    EVT_LIST_DELETE_ITEM(ID_COMMAND_LIST, OptionsDialog::onCommandSelection)
    EVT_LIST_ITEM_SELECTED(ID_COMMAND_LIST, OptionsDialog::onCommandSelection)
    EVT_LIST_ITEM_DESELECTED(ID_COMMAND_LIST, OptionsDialog::onCommandSelection)

    // too long lines to keep doxygen quiet
END_EVENT_TABLE()

/**
 * The Constructor.
 * 
 * @param parent Parent window of the dialog.
 * @param options Options to be modified.
 * @param commandRegistry Command registry containing available commands.
 */
OptionsDialog::OptionsDialog(
    wxWindow* parent,
    GUIOptions& options,
    CommandRegistry& commandRegistry):
    wxDialog(parent, -1, _T(""), wxDefaultPosition, wxSize(500, 300),
             (wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)),
    parent_(parent), options_(options), shortcutList_(NULL),
    toolbarList_(NULL), commandList_(NULL), commandRegistry_(commandRegistry) {

    initialize();

    readCommands();
    readOptions();
    SetMinSize(wxSize(420, 300));

    notebook_ = dynamic_cast<wxNotebook*>(FindWindow(ID_NOTEBOOK));
}


/**
 * The Destructor.
 */
OptionsDialog::~OptionsDialog() {
    // delete shortcut list
    vector<Shortcut*>::iterator i = shortcuts_.begin();
    for (; i != shortcuts_.end(); i++) {
	delete (*i);
    }
    shortcuts_.clear();
}


/**
 * Initializes the dialog widgets.
 */
void
OptionsDialog::initialize() {

    createContents(this, true, true);

    toolbarList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_TOOLBAR_LIST));
    commandList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_COMMAND_LIST));
    shortcutList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_KB_SC_LIST));

    // Create keyboard shortcut list columns.
    wxListCtrl* shortcutList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_KB_SC_LIST));
    shortcutList->InsertColumn(
        0, COMMAND_COLUMN_TITLE, wxLIST_FORMAT_LEFT, 190);
    shortcutList_->InsertColumn(
        1, SHORTCUT_COLUMN_TITLE, wxLIST_FORMAT_LEFT, 150);
    
    // Create toolbar buttons list column.
    wxListCtrl* toolbarList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_TOOLBAR_LIST));
    toolbarList->InsertColumn(
        0, TOOLBAR_BUTTONS_COLUMN_TITLE, wxLIST_FORMAT_LEFT, 180);

    // Create available buttons list column.
    wxListCtrl* commandList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_COMMAND_LIST));
    commandList->InsertColumn(
        0, AVAILABLE_COMMANDS_COLUMN_TITLE, wxLIST_FORMAT_LEFT, 180);

    FindWindow(ID_TOOL_DOWN)->Disable();
    FindWindow(ID_TOOL_UP)->Disable();
    FindWindow(ID_TOOL_INSERT)->Disable();
    FindWindow(ID_TOOL_REMOVE)->Disable();
    FindWindow(ID_EDIT_KB_SC)->Disable();
    FindWindow(ID_DELETE_KB_SC)->Disable();

    wxChoice* contentsChoicer =
        dynamic_cast<wxChoice*>(FindWindow(ID_TOOLBAR_CONTENTS));

    // choicer texts
    contentsChoicer->Append(CONTENTS_ICONS);
    contentsChoicer->Append(CONTENTS_BOTH);
    contentsChoicer->Append(CONTENTS_TEXT);
}


/**
 * Reads the options in options_ to dialog attributes.
 */
void
OptionsDialog::readOptions() {
    
    // read and set the toolbar layout
    wxChoice* contentsChoice = 
        dynamic_cast<wxChoice*>(FindWindow(ID_TOOLBAR_CONTENTS));

    if (options_.toolbarLayout() == GUIOptions::TEXT) {
        contentsChoice->SetStringSelection(CONTENTS_TEXT);
    } else if (options_.toolbarLayout() == GUIOptions::ICON) {
        contentsChoice->SetStringSelection(CONTENTS_ICONS);
    } else if (options_.toolbarLayout() == GUIOptions::BOTH) {
        contentsChoice->SetStringSelection(CONTENTS_BOTH);
    } else {
        assert(false);
    }    

    // read keyboard shortcuts
    KeyboardShortcut* shortcut = options_.firstShortcut();
    while (shortcut != NULL) {

        // find the shortcut list item for the command
        Shortcut* listItem = NULL;
        vector<Shortcut*>::iterator i = shortcuts_.begin();
        for (; i != shortcuts_.end(); i++) {
            if ((*i)->name == shortcut->action()) {
                listItem = (*i);
            }
        }
        if (listItem == NULL) {
            wxString message = _T("Unknown command in options:\n");
            message.Append(WxConversion::toWxString(shortcut->action()));
            ErrorDialog dialog(this, message);
            dialog.ShowModal();
        } else {
            listItem->shortcut = new KeyboardShortcut(*shortcut);
        }
        shortcut = options_.nextShortcut();
    }    
    
    bool found = true;
    int slot = 0;

    // read toolbar buttons and separators
    while (found) {
	
	found = false;

	// check if a toolbar button exists for the slot
	ToolbarButton* tool = options_.firstToolbarButton();
	while (tool != NULL) {
	    if (tool->slot() == slot) {
                // button found for the slot, add it
                toolbar_.push_back(tool->action());
                found = true;
	    }
	    tool = options_.nextToolbarButton();
	}	

	// check if a separator exists for the slot
	int separator = options_.firstSeparator();
	while (separator != -1) {
	    if (separator == slot) {
                // separator found for the slot, add it
                toolbar_.push_back(GUIOptions::TOOLBAR_SEPARATOR);
                found = true;
	    }
	    separator = options_.nextSeparator();
	}	
	slot++;
    }
}


/**
 * Writes the options from dialog attributes to the current options object.
 */
void
OptionsDialog::writeOptions() {
    
    // set the toolbar layout
    wxChoice* contentsChoice = 
        dynamic_cast<wxChoice*>(FindWindow(ID_TOOLBAR_CONTENTS));

    if (contentsChoice->GetStringSelection() == CONTENTS_TEXT) {
        options_.setToolbarLayout(GUIOptions::TEXT);
    } else if (contentsChoice->GetStringSelection() == CONTENTS_ICONS) { 
        options_.setToolbarLayout(GUIOptions::ICON);
    } else if (contentsChoice->GetStringSelection() == CONTENTS_BOTH) {
        options_.setToolbarLayout(GUIOptions::BOTH);
    } else {
        assert(false);
    }
    
    // delete all old shortcuts
    while (options_.firstShortcut() != NULL) {
        options_.deleteKeyboardShortcut(options_.firstShortcut());
    }

    // delete all old toolbar buttons
    while (options_.firstToolbarButton() != NULL) {
        options_.deleteToolbarButton(options_.firstToolbarButton());
    }

    // delete all old toolbar separators
    while (options_.firstSeparator() != -1) {
        options_.deleteSeparator(options_.firstSeparator());
    }

    // add all keyboard shortcuts and toolbar buttons
    vector<Shortcut*>::iterator i = shortcuts_.begin();
    for (; i != shortcuts_.end(); i++) {
        if ((*i)->shortcut != NULL) {	    
            options_.addKeyboardShortcut((*i)->shortcut);
        }
    }
    
    // add all toolbar buttons and separators
    int slot = 0;
    vector<string>::iterator iter = toolbar_.begin();
    for (; iter != toolbar_.end(); iter++) {
        if ((*iter) == GUIOptions::TOOLBAR_SEPARATOR) {
            options_.addSeparator(slot);
        } else {
            ToolbarButton* button = new ToolbarButton(slot, (*iter));
            options_.addToolbarButton(button);
        }
        slot++;
    }
}


/**
 * Adds available commands to the keyboard shortcut list and toolbar tool
 * list.
 * 
 * Reads commands fromt the command registry.
 */
void
OptionsDialog::readCommands() {
    GUICommand* command = commandRegistry_.firstCommand();
    while (command != NULL) {
	Shortcut* shortcut = new Shortcut;
	shortcut->id = command->id();
	shortcut->name = command->name();
	shortcut->shortcut = NULL;
	shortcuts_.push_back(shortcut);
	command = commandRegistry_.nextCommand();
    }    
}


/**
 * Transfers data from dialog attributes to controls in the dialog.
 * 
 * @return False if the transfer failed.
 */
bool
OptionsDialog::TransferDataToWindow() {
    
    shortcutList_->DeleteAllItems();
    toolbarList_->DeleteAllItems();
    commandList_->DeleteAllItems();
    
    // Update keyboard shortcut list.
    vector<Shortcut*>::iterator i = shortcuts_.begin();
    for (; i != shortcuts_.end(); i++) {
	shortcutList_->InsertItem(shortcutList_->GetItemCount(),
	    WxConversion::toWxString((*i)->name));
	// add shortcut to the second column if one exists
	if ((*i)->shortcut != NULL) {
    
	    string keyName = "";
    
	    // set the name of the key
	    if ((*i)->shortcut->key() > 32 && (*i)->shortcut->key() < 127) {
		// character key
		keyName = Conversion::toString((*i)->shortcut->key());
	    } else if ((*i)->shortcut->key() == 127) {
		// delete key
		keyName = "DEL";
	    } else if ((*i)->shortcut->fKey() != 0) {
		// function key
		keyName = "F"+Conversion::toString((*i)->shortcut->fKey());
	    }
	    
	    wxString key = WxConversion::toWxString(keyName);
	    
	    if ((*i)->shortcut->alt()) {
		key.Prepend(_T("ALT - "));
	    }
	    if ((*i)->shortcut->ctrl()) {
		key.Prepend(_T("CTRL - "));
	    }
	    shortcutList_->SetItem(shortcutList_->GetItemCount()-1,
				       1, key);
	}
    }
    
    // Update toolbar button lists.
    vector<string>::iterator iter = toolbar_.begin();
    for (; iter != toolbar_.end(); iter++) {
	toolbarList_->InsertItem(toolbarList_->GetItemCount(),
				 WxConversion::toWxString((*iter)));
    }

    // update commands list
    commandList_->InsertItem(
        0, WxConversion::toWxString(GUIOptions::TOOLBAR_SEPARATOR));

    bool inToolbar;
    i = shortcuts_.begin();
    for (; i != shortcuts_.end(); i++) {
	
	inToolbar = false;
	
	// check if command is already in the toolbar
	iter = toolbar_.begin();
	for (; iter != toolbar_.end(); iter++) {
	    if ((*i)->name == (*iter)) {
		inToolbar = true;
	    }
	}

	// add command to the list if it wasn't in the toolbar
	if (!inToolbar) {
	    commandList_->InsertItem(commandList_->GetItemCount(),
		WxConversion::toWxString((*i)->name));
	}
    }
    
    return true;
}


/**
 * Removes keyboard shortcut for the selected command.
 */
void
OptionsDialog::onDeleteShortcut(wxCommandEvent&) {
    vector<Shortcut*>::iterator iter = shortcuts_.begin();
    for (int i = 0; i < shortcutList_->GetItemCount(); i++) {
        if (shortcutList_->GetItemState(i, wxLIST_STATE_SELECTED) != 0) {
            // delete selected shortcut
            delete (*iter)->shortcut;
            (*iter)->shortcut = NULL;
            TransferDataToWindow();
            return;
        }
        iter++;
    }
    assert(false);
}


/**
 * Handles left mouse button double click on the shortcut list.
 */
void
OptionsDialog::onActivateShortcut(wxListEvent&) {
    wxCommandEvent dummy;
    onEditShortcut(dummy);
}


/**
 * Returns pointer to the shortcut selected in the shortcut list.
 *
 * @return NULL if no shortcut is selected.
 */
OptionsDialog::Shortcut*
OptionsDialog::selectedShortcut() {
    vector<Shortcut*>::iterator iter = shortcuts_.begin();
    for (int i = 0; i < shortcutList_->GetItemCount(); i++) {
        if (shortcutList_->GetItemState(i, wxLIST_STATE_SELECTED) != 0) {
            return (*iter);
        }
        iter++;
    }
    return NULL;
}

/**
 * Prompts for a new keyboard shortcut for the selected command with
 * ShortcutDialog.
 */
void
OptionsDialog::onEditShortcut(wxCommandEvent&) {

    bool newShortcut = false;    

    Shortcut* selected = selectedShortcut();
    if (selected == NULL) {
        return;
    }

    // edit selected shortcut    
    KeyboardShortcut* shortcut = selected->shortcut;
    if (shortcut == NULL) {
        newShortcut = true;
        shortcut = new KeyboardShortcut(
            selected->name, 0, false, false, char(0));
    }

    KeyboardShortcutDialog dialog(this, shortcut);

    if (dialog.ShowModal() == wxID_OK) {

        vector<Shortcut*>::iterator di = shortcuts_.begin();
        for (; di != shortcuts_.end(); di++) {
            if ((*di)->name == selected->name) {
                (*di)->shortcut = shortcut;    
            } else if (((*di)->shortcut != NULL) &&
                       ((*di)->shortcut->equals(*shortcut))) {
                
                // Shortcut with the same key-combination exists, delete it.
                delete (*di)->shortcut;
                (*di)->shortcut = NULL;
            }
        }
    
        TransferDataToWindow();
    } else if (newShortcut) {
        // Shortcut adding cancelled.
        delete shortcut;
    }
    return;
}


/**
 * Adds a toolbar button for the command selected in the command list.
 */
void
OptionsDialog::onInsertTool(wxCommandEvent&) {
    // search for the selected command and add it to the toolbar
    for (int i = 0; i < commandList_->GetItemCount(); i++) {	
        if (commandList_->GetItemState(i, wxLIST_STATE_SELECTED) != 0) {
            string selected =
                WxConversion::toString(commandList_->GetItemText(i));
            toolbar_.push_back(selected);
            
        }
    }    
    TransferDataToWindow();
}


/**
 * Removes the selected toolbar button from the toolbar button list.
 */
void
OptionsDialog::onRemoveTool(wxCommandEvent&) {
    vector<string>::iterator iter = toolbar_.begin();
    for (int i = 0; i < toolbarList_->GetItemCount(); i++) {
        // search selected list item
        if (toolbarList_->GetItemState(i, wxLIST_STATE_SELECTED) != 0) {
            toolbar_.erase(iter);
            break;
        }
        assert(iter != toolbar_.end());
        iter++;
    }
    TransferDataToWindow();
}


/**
 * Moves selected tool up or down in the toolbar button list.
 */
void
OptionsDialog::onMoveTool(wxCommandEvent& event) {
    
    int selected = -1;
    // search the selected command in the toolbar button list
    for (int i = 0; i < toolbarList_->GetItemCount(); i++) {
        if (toolbarList_->GetItemState(i, wxLIST_STATE_SELECTED) != 0) {
            selected = i;
        }
    }
    
    assert(selected != -1);
    
    // move tool up
    if (event.GetId() == ID_TOOL_UP) {
        if (selected == 0) {
            // tool already at the top
            return;
        }
        string above = toolbar_[selected-1];
        toolbar_[selected-1] = toolbar_[selected];
        toolbar_[selected] = above;
    }
    
    // move tool down
    if (event.GetId() == ID_TOOL_DOWN) {
        if (selected == int(toolbar_.size())-1) {
            // toolbar already at the bottom
            return;
        }
        string below = toolbar_[selected+1];
        toolbar_[selected+1] = toolbar_[selected];
        toolbar_[selected] = below;
    }
    
    TransferDataToWindow();
    
    long setItem = -1;
    if (event.GetId() == ID_TOOL_UP) {
        setItem = selected - 1;
    } else if (event.GetId() == ID_TOOL_DOWN) {
        setItem = selected + 1;
    }
    
    toolbarList_->SetItemState(setItem, wxLIST_STATE_SELECTED,
                               wxLIST_STATE_SELECTED);
    toolbarList_->EnsureVisible(setItem);
}


/**
 * Disables and enables Edit and Delete buttons under the keyboard shortcut.
 * 
 * If a shortcut is selected, buttons are enabled. If no shortcut is selected
 * the buttons are disabled.
 * 
 * @param event ListEvent, which may have changed the selection.
 */
void
OptionsDialog::onShortcutSelection(wxListEvent& event) {
    if (shortcutList_->GetSelectedItemCount() != 1) {
        FindWindow(ID_DELETE_KB_SC)->Disable();
        FindWindow(ID_EDIT_KB_SC)->Disable();
        return;
    }
    FindWindow(ID_DELETE_KB_SC)->Enable();
    FindWindow(ID_EDIT_KB_SC)->Enable();
    event.Skip();
}   


/**
 * Opens a pop-up menu when right mouse button was pressed.
 *
 * @param event Information about right mouse click event.
 */
void
OptionsDialog::onShortcutRightClick(wxListEvent& event) {

    shortcutList_->SetItemState(event.GetIndex(), wxLIST_STATE_SELECTED,
                                wxLIST_STATE_SELECTED);
    
    wxMenu* contextMenu = new wxMenu();	
    
    contextMenu->Append(ID_EDIT_KB_SC, EDIT_BUTTON_LABEL);
    contextMenu->Append(ID_DELETE_KB_SC, DELETE_BUTTON_LABEL);
    shortcutList_->PopupMenu(contextMenu, event.GetPoint());
}


/**
 * Disables and enables Up, Down and Remove buttons in the 'Toolbar' page.
 * 
 * If a toolbar item is selected, buttons are enabled. If no item is
 * selected the buttons are disabled.
 * 
 * @param event ListEvent, which may have changed the selection.
 */
void
OptionsDialog::onToolbarSelection(wxListEvent& event) {
    if (toolbarList_->GetSelectedItemCount() != 1) {
        FindWindow(ID_TOOL_UP)->Disable();
        FindWindow(ID_TOOL_DOWN)->Disable();
        FindWindow(ID_TOOL_REMOVE)->Disable();
        return;
    }
    FindWindow(ID_TOOL_UP)->Enable();
    FindWindow(ID_TOOL_DOWN)->Enable();
    FindWindow(ID_TOOL_REMOVE)->Enable();
    event.Skip();
}   


/**
 * Disables and enables 'Insert' button in the 'Toolbar' page
 * 
 * If a command is selected, button is enabled. If no command is selected
 * the button is disabled.
 * 
 * @param event ListEvent, which may have changed the selection.
 */
void
OptionsDialog::onCommandSelection(wxListEvent& event) {
    if (commandList_->GetSelectedItemCount() != 1) {
        FindWindow(ID_TOOL_INSERT)->Disable();
        return;
    }
    FindWindow(ID_TOOL_INSERT)->Enable();
    event.Skip();
}   




/**
  * Validates input in the controls, and updates the options.
  */
void
OptionsDialog::onOK(wxCommandEvent&) {
    
    if (!Validate()) {
        return;
    }
    
    if (!TransferDataFromWindow()) {
        return;
    }
    
    writeOptions();
    
    EndModal(wxID_OK);
}


/**
 * Opens the help for the dialog from the user manual.
 */ 
void
OptionsDialog::onHelp(wxCommandEvent&) {
}


/**
 * Adds a new tab to the options dialog.
 *
 * This function can be used to add tabs for modifying application specific
 * options.
 *
 * @param page Tab to add.
 * @param title Title of the tab.
 */
void
OptionsDialog::addPage(wxPanel* page, const wxString& title) {
    page->Reparent(notebook_);
    notebook_->InsertPage(0, page, title, true);
}

/**
 * Creates the dialog contents.
 * 
 * This function was initially generated by wxDesigner.
 * 
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
OptionsDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 0 );

    wxNotebook *item2 = new wxNotebook( parent, ID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, 0 );
#if !wxCHECK_VERSION(2,5,2)
    wxNotebookSizer *item1 = new wxNotebookSizer( item2 );
#else
    wxWindow *item1 = item2;
#endif

    wxPanel *item4 = new wxPanel( item2, -1 );
    OptionsDialog::createKBShortcutPage( item4, true, true );
    item2->AddPage( item4, wxT("Keyboard Shortcuts") );

    wxPanel *item5 = new wxPanel( item2, -1 );
    OptionsDialog::createToolbarPage( item5, true, true );
    item2->AddPage( item5, wxT("Toolbar") );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxGridSizer *item6 = new wxGridSizer( 2, 0, 0 );

    wxButton *item7 = new wxButton( parent, ID_HELP, wxT("&Help..."), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add( item7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item8 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item9 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item10 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    item6->Add( item8, 0, wxALIGN_RIGHT|wxALL, 5 );

    item0->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}


/**
 * Creates the 'Keyboard Shotcut' page for the dialog.
 * 
 * This function was initially generated by wxDesigner.
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
OptionsDialog::createKBShortcutPage(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 0 );

    wxListCtrl *item1 = new wxListCtrl( parent, ID_KB_SC_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxSUNKEN_BORDER );
    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item2 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item3 = new wxButton( parent, ID_EDIT_KB_SC, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item4 = new wxButton( parent, ID_DELETE_KB_SC, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}


/**
 * Creates the 'Toolbar' page for the dialog.
 * 
 * This function was generated by wxDesigner.
 *
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */ 
wxSizer*
OptionsDialog::createToolbarPage(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 0 );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 3, 0, 0 );
    item1->AddGrowableCol( 0 );
    item1->AddGrowableCol( 2 );
    item1->AddGrowableRow( 0 );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 1, 0, 0 );
    item2->AddGrowableCol( 0 );
    item2->AddGrowableRow( 0 );

    wxListCtrl *item3 = new wxListCtrl( parent, ID_TOOLBAR_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item2->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item4 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item5 = new wxButton( parent, ID_TOOL_UP, wxT("&Up"), wxDefaultPosition, wxSize(50,-1), 0 );
    item4->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item6 = new wxButton( parent, ID_TOOL_DOWN, wxT("&Down"), wxDefaultPosition, wxSize(50,-1), 0 );
    item4->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item2, 0, wxGROW|wxALL, 5 );

    wxBoxSizer *item7 = new wxBoxSizer( wxVERTICAL );

    wxButton *item8 = new wxButton( parent, ID_TOOL_INSERT, wxT("&Insert"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item9 = new wxButton( parent, ID_TOOL_REMOVE, wxT("&Remove"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    wxFlexGridSizer *item10 = new wxFlexGridSizer( 1, 0, 0 );
    item10->AddGrowableCol( 0 );
    item10->AddGrowableRow( 0 );

    wxListCtrl *item11 = new wxListCtrl( parent, ID_COMMAND_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item10->Add( item11, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item10, 0, wxGROW|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );

    wxBoxSizer *item12 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item13 = new wxStaticText( parent, ID_LABEL_TOOLBAR_CONTENTS, wxT("Contents:"), wxDefaultPosition, wxDefaultSize, 0 );
    item12->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString *strs14 = (wxString*) NULL;
    wxChoice *item14 = new wxChoice( parent, ID_TOOLBAR_CONTENTS, wxDefaultPosition, wxSize(150,-1), 0, strs14, 0 );
    item12->Add( item14, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    item0->Add( item12, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
