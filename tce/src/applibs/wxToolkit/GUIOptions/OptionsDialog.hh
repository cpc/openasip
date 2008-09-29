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
 * @file OptionsDialog.hh
 *
 * Declaration of OptionsDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPTIONS_DIALOG_HH
#define TTA_OPTIONS_DIALOG_HH

#include <string>
#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <vector>

class ToolbarButton;
class KeyboardShortcut;
class GUIOptions;
class wxListCtrl;
class CommandRegistry;

/**
 * Dialog for editing GUIOptions.
 *
 * This class allows modification of options in the GUIOptions class.
 * If an application uses custom options class derived from the GUIOptions
 * class, this class can be used as a base class for the application specific
 * options dialog. To customize the derived dialog, override readOptions and
 * writeOptions. Controls for modifying the application specific options can
 * be added by creating a wxPanel containing the controls, and adding it to
 * the dialog as a tab using the addPage method.
 */
class OptionsDialog : public wxDialog {
public:
    OptionsDialog(
        wxWindow* parent,
        GUIOptions& options,
        CommandRegistry& commandRegisry);

    virtual ~OptionsDialog();

protected:
    void addPage(wxPanel* page, const wxString& title);
    virtual void readOptions();
    virtual void writeOptions();

    /// Notebook containing dialog pages.
    wxNotebook* notebook_;


private:

    virtual wxSizer* createContents(
        wxWindow* parent, bool callFit, bool set_sizer);

    void readCommands();
    void initialize();

    void onShortcutSelection(wxListEvent& event);
    void onToolbarSelection(wxListEvent& event);
    void onCommandSelection(wxListEvent& event);
   
    void onDeleteShortcut(wxCommandEvent& event);
    void onActivateShortcut(wxListEvent& event);
    void onEditShortcut(wxCommandEvent& event);
    void onShortcutRightClick(wxListEvent& event);
    void onInsertTool(wxCommandEvent& event);
    void onRemoveTool(wxCommandEvent& event);
    void onMoveTool(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);
    void setTexts();
   
    virtual bool TransferDataToWindow();
    
    wxSizer* createKBShortcutPage(
        wxWindow* parent, bool callFit, bool set_sizer);
    wxSizer* createToolbarPage(
        wxWindow* parent, bool callFit, bool set_sizer);
    
    /**
     * Describes a keyboard shortcut.
     */
    struct Shortcut {
        std::string name;             ///< Name of the command.
        int id;                       ///< ID of the command.
        KeyboardShortcut* shortcut;   ///< A keyboard shortcut.
    };
 
    Shortcut* selectedShortcut();

    /// parent window of the dialog
    wxWindow* parent_;
    /// current editor options
    GUIOptions& options_;
    /// Keyboard shortcut list control.
    wxListCtrl* shortcutList_;
    /// Toolbar buttons list control.
    wxListCtrl* toolbarList_;
    /// List control for commands not in the toolbar.
    wxListCtrl* commandList_;
    // Toolbar contents choicer item variable
    int choice_;
  
    /// Keyboard shortcuts.
    std::vector<Shortcut*> shortcuts_;
    /// Toolbar buttons and separators.
    std::vector<std::string> toolbar_;

    /// Command registry containing available commands for shortcuts.
    CommandRegistry& commandRegistry_;

    // control IDs
    enum {
        ID_NOTEBOOK = 10000,
        ID_HELP,
        ID_BROWSE,
        ID_KB_SC_LIST,
        ID_EDIT_KB_SC,
        ID_DELETE_KB_SC,
        ID_TOOLBAR_LIST,
        ID_TOOL_DOWN,
        ID_TOOL_UP,
        ID_TOOL_INSERT,
        ID_TOOL_REMOVE,
        ID_COMMAND_LIST,
        ID_TOOLBAR_CONTENTS,
        ID_TEXT,
        ID_LABEL_TOOLBAR_CONTENTS
    };

    /// Title of the keyboard shortcut list command name column.
    static const wxString COMMAND_COLUMN_TITLE;
    /// Title of the keyboard shortcut list shortcut column.
    static const wxString SHORTCUT_COLUMN_TITLE;
    /// Title of the toolbar tab toolbar button list.
    static const wxString TOOLBAR_BUTTONS_COLUMN_TITLE;
    /// Title of the toolbar tab available commands list.
    static const wxString AVAILABLE_COMMANDS_COLUMN_TITLE;
    /// Text fot the toolbar contents choicer icon-mode item.
    static const wxString CONTENTS_ICONS;
    /// Text for the toolbar contents choicer icon&text mode item.
    static const wxString CONTENTS_BOTH;
    /// Text for the toolbar contents choicer text-mode item.
    static const wxString CONTENTS_TEXT;

    /// Label for Edit-button.
    static const wxString EDIT_BUTTON_LABEL;
    /// Label for delete-button.
    static const wxString DELETE_BUTTON_LABEL;

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};

#endif
