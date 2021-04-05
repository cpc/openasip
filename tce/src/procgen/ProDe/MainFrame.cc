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
 * @file MainFrame.cc
 *
 * Definition of MainFrame class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/sizer.h>
#include <wx/statusbr.h>

#include "MainFrame.hh"
#include "Environment.hh"
#include "ChildFrame.hh"
#include "CommandRegistry.hh"
#include "ProDe.hh"
#include "AddFUCmd.hh"
#include "AddSocketCmd.hh"
#include "AddBridgeCmd.hh"
#include "AddBusCmd.hh"
#include "AddASCmd.hh"
#include "AddGCUCmd.hh"
#include "AddIUCmd.hh"
#include "AddRFCmd.hh"
#include "CopyComponentCmd.hh"
#include "CutComponentCmd.hh"
#include "PasteComponentCmd.hh"
#include "DeleteComponentCmd.hh"
#include "ModifyComponentCmd.hh"
#include "EditOptionsCmd.hh"
#include "SaveOptionsCmd.hh"
#include "EditConnectionsCmd.hh"
#include "EditAddressSpacesCmd.hh"
#include "EditTemplatesCmd.hh"
#include "ZoomInCmd.hh"
#include "ZoomOutCmd.hh"
#include "FitWindowCmd.hh"
#include "FitHeightCmd.hh"
#include "FitWidthCmd.hh"
#include "VerifyMachineCmd.hh"
#include "ImplementMachineCmd.hh"
#include "CallExplorerPluginCmd.hh"
#include "UserManualCmd.hh"
#include "AboutCmd.hh"
#include "NewDocumentCmd.hh"
#include "OpenDocumentCmd.hh"
#include "CloseDocumentCmd.hh"
#include "SaveDocumentCmd.hh"
#include "SaveDocumentAsCmd.hh"
#include "QuitCmd.hh"
#include "PrintCmd.hh"
#include "PrintPreviewCmd.hh"
#include "SelectCmd.hh"
#include "UndoCmd.hh"
#include "RedoCmd.hh"
#include "EditImmediateSlotsCmd.hh"
#include "ProDeExportCmd.hh"
#include "EditBusOrderCmd.hh"
#include "FullyConnectBussesCmd.hh"
#include "AddRFFromHDBCmd.hh"
#include "AddIUFromHDBCmd.hh"
#include "AddFUFromHDBCmd.hh"
#include "ToggleUnitDetailsCmd.hh"
#include "EditMachineCmd.hh"
#include "VLIWConnectICCmd.hh"
#include "BlocksConnectICCmd.hh"

#include "KeyboardShortcut.hh"
#include "ProDeOptions.hh"
#include "WxConversion.hh"
#include "ProDeConstants.hh"
#include "ErrorDialog.hh"


BEGIN_EVENT_TABLE(MainFrame, wxDocMDIParentFrame)
    EVT_KEY_DOWN(MainFrame::onKeyEvent)
    EVT_MENU(-1, MainFrame::onCommandEvent)
    EVT_TOOL(-1, MainFrame::onCommandEvent)
    EVT_UPDATE_UI(-1, MainFrame::onUpdateUI)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param manager Pointer to the editor's document manager.
 * @param parent Parent frame of the MainFrame.
 * @param title Title of the MainFrame.
 * @param position Position of the MainFrame on the screen.
 * @param size Size of the MainFrame on the screen.
 * @param style Style of the MainFrame.
 */
MainFrame::MainFrame(
    wxDocManager* manager,
    wxFrame* parent,
    const wxString& title,
    const wxPoint& position,
    const wxSize& size,
    long style):
    wxDocMDIParentFrame(manager, parent, -1, title,
			position, size, style, _T("MainFrame")) {

    commandRegistry_ = wxGetApp().commandRegistry();
    assert(commandRegistry_ != NULL);
    commandRegistry_->addCommand(new NewDocumentCmd());
    commandRegistry_->addCommand(new OpenDocumentCmd());
    commandRegistry_->addCommand(new CloseDocumentCmd());
    commandRegistry_->addCommand(new SaveDocumentCmd());
    commandRegistry_->addCommand(new SaveDocumentAsCmd());
    commandRegistry_->addCommand(new PrintCmd());
    commandRegistry_->addCommand(new PrintPreviewCmd());
    commandRegistry_->addCommand(new ProDeExportCmd());
    commandRegistry_->addCommand(new QuitCmd());
    commandRegistry_->addCommand(new UndoCmd());
    commandRegistry_->addCommand(new RedoCmd());
    commandRegistry_->addCommand(new AddFUCmd());
    commandRegistry_->addCommand(new AddRFCmd());
    commandRegistry_->addCommand(new AddBusCmd());
    commandRegistry_->addCommand(new AddSocketCmd());
    commandRegistry_->addCommand(new AddBridgeCmd());
    commandRegistry_->addCommand(new AddIUCmd());
    commandRegistry_->addCommand(new AddGCUCmd());
    commandRegistry_->addCommand(new AddASCmd());
    commandRegistry_->addCommand(new AddRFFromHDBCmd());
    commandRegistry_->addCommand(new AddIUFromHDBCmd());
    commandRegistry_->addCommand(new AddFUFromHDBCmd());
    commandRegistry_->addCommand(new CopyComponentCmd());
    commandRegistry_->addCommand(new CutComponentCmd());
    commandRegistry_->addCommand(new PasteComponentCmd());
    commandRegistry_->addCommand(new DeleteComponentCmd());
    commandRegistry_->addCommand(new ModifyComponentCmd());
    commandRegistry_->addCommand(new EditConnectionsCmd());
    commandRegistry_->addCommand(new EditAddressSpacesCmd());
    commandRegistry_->addCommand(new EditBusOrderCmd());
    commandRegistry_->addCommand(new EditTemplatesCmd());
    commandRegistry_->addCommand(new EditMachineCmd());
    commandRegistry_->addCommand(new EditImmediateSlotsCmd());
    commandRegistry_->addCommand(new SelectCmd());
    commandRegistry_->addCommand(new FullyConnectBussesCmd());
    commandRegistry_->addCommand(new ZoomInCmd());
    commandRegistry_->addCommand(new ZoomOutCmd());
    commandRegistry_->addCommand(new FitWindowCmd());
    commandRegistry_->addCommand(new FitHeightCmd());
    commandRegistry_->addCommand(new FitWidthCmd());
    commandRegistry_->addCommand(new ToggleUnitDetailsCmd());
    commandRegistry_->addCommand(new VerifyMachineCmd());
    commandRegistry_->addCommand(new ImplementMachineCmd());
    commandRegistry_->addCommand(new CallExplorerPluginCmd());
    commandRegistry_->addCommand(new EditOptionsCmd());
    commandRegistry_->addCommand(new SaveOptionsCmd());
    commandRegistry_->addCommand(new UserManualCmd());
    commandRegistry_->addCommand(new AboutCmd());
    commandRegistry_->addCommand(new VLIWConnectICCmd());
    commandRegistry_->addCommand(new BlocksConnectICCmd());

    toolbar_ = NULL;
    CreateStatusBar(2);
    int widths[2] = {-2, -1};
    SetStatusWidths(2, widths);

    createMenubar();
    createToolbar();
}


/**
 * Toggles visibility of the toolbar.
 */
void
MainFrame::onToggleToolbar() {
    ProDeOptions* options = wxGetApp().options();
    if (toolbar_ != NULL) {
        options->setToolbarVisibility(false);
        SetToolBar(NULL);
        delete toolbar_;
        toolbar_ = NULL;
    } else {
        options->setToolbarVisibility(true);
        createToolbar();
    }
    Layout();
}


/**
 * Toggles visibility of the statusbar.
 */
void
MainFrame::onToggleStatusbar() {
    wxStatusBar* statusbar = GetStatusBar();
    if (statusbar != NULL) {
        SetStatusBar(NULL);
        delete statusbar;
        statusbar = NULL;
    } else {
        CreateStatusBar(2);
        int widths[2] = {-4, -1};
        SetStatusWidths(2, widths);
    }
    Layout();
    Fit();
}


/**
 * Creates and returns a new options menu.
 *
 * @return Options menu.
 */
wxMenu*
MainFrame::createOptionsMenu() {
    wxMenu* optionsMenu = new wxMenu;
    optionsMenu->Append(
        ProDeConstants::COMMAND_EDIT_OPTIONS,
        menuAccelerator(ProDeConstants::COMMAND_EDIT_OPTIONS).Prepend(
            _T("&Edit...")));

    optionsMenu->AppendSeparator();
    optionsMenu->Append(
        ProDeConstants::COMMAND_SAVE_OPTIONS,
        menuAccelerator(ProDeConstants::COMMAND_SAVE_OPTIONS).Prepend(
            _T("&Save")));

    return optionsMenu;
}


/**
 * Creates and returns a new help menu.
 *
 * @return Help menu.
 */
wxMenu*
MainFrame::createHelpMenu() {
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(
        UserManualCmd::COMMAND_ID,
        menuAccelerator(UserManualCmd::COMMAND_ID).Prepend(
            _T("&User Manual...")));

    helpMenu->Append(
        ProDeConstants::COMMAND_ABOUT,
        menuAccelerator(ProDeConstants::COMMAND_ABOUT).Prepend(
            _T("&About...")));

    return helpMenu;
}



/**
 * Handles menu and toolbar events.
 *
 * @param event Event to handle.
 */
void
MainFrame::onCommandEvent(wxCommandEvent& event) {

    if (event.GetId() == ID_VIEW_TOOLBAR) {
        onToggleToolbar();
        return;
    }

    if (event.GetId() == ID_VIEW_STATUSBAR) {
        onToggleStatusbar();
        return;
    }

    GUICommand* command = wxGetApp().commandRegistry()
	->createCommand(event.GetId());

    if (command == NULL) {
	ErrorDialog dialog(this,
			   _T("No handler found for the command event!"));
	dialog.ShowModal();
	return;
    }

    if (!command->isEnabled()) {
        delete command;
        return;
    }

    wxView* view = wxGetApp().docManager()->GetCurrentView();
    if (view != NULL) {
        command->setParentWindow(this);
        EditorCommand* editorCmd = dynamic_cast<EditorCommand*>(command);
        if (editorCmd != NULL) {
            editorCmd->setView(view);
        }
        command->Do();
        updateUI();
        delete command;
        return;
    }

    // set command parent window and view
    command->setParentWindow(this);
    // execute the command
    command->Do();
    updateUI();
    delete command;
}


/**
 * Updates toolbar and menubar item enabled/disabled states.
 *
 * @param event Update event for the menu or toolbar item to be updated.
 */
void
MainFrame::onUpdateUI(wxUpdateUIEvent& event) {
    
    CommandRegistry* registry = wxGetApp().commandRegistry();
    GUICommand* command = registry->createCommand(event.GetId());

    if (command == NULL) {
        event.Skip();
        return;
    }

    wxView* view = wxGetApp().docManager()->GetCurrentView();
    if (view != NULL) {        
	EditorCommand* editorCmd = dynamic_cast<EditorCommand*>(command);
        if (editorCmd != NULL) {
            editorCmd->setView(view);
        }
    }

    if (command->isEnabled()) {
	event.Enable(true);
    } else {
	event.Enable(false);
    }

    if (command->isChecked()) {
        event.Check(command->isChecked());
    }

    delete command;
}

/**
 * Updates enabled/disabled states of toolbar buttons and menu items.
 */
void
MainFrame::updateUI() {

    wxMenuBar* menubar = GetMenuBar();
    for (unsigned i = 0; i < menubar->GetMenuCount(); i++) {
        menubar->GetMenu(i)->UpdateUI();
    }

    if (toolbar_ == NULL) {
        return;
    }

    CommandRegistry* registry = wxGetApp().commandRegistry();
    ProDeOptions* options = wxGetApp().options();
    ToolbarButton* tool = options->firstToolbarButton();

    while (tool != NULL) {
        int id = registry->commandId(tool->action());
        if (!registry->isEnabled(tool->action())) {
            toolbar_->EnableTool(id, false);
        } else {
            toolbar_->EnableTool(id, true);
        }
        tool = options->nextToolbarButton();
    }

}

/**
 * Handles key events on the canvas.
 *
 * @param event Key event to handle.
 */
void
MainFrame::onKeyEvent(wxKeyEvent&) {
}


/**
 * Creates the default menubar.
 */
void
MainFrame::createMenubar() {
    // File menu
    wxMenu* fileMenu = new wxMenu;

    fileMenu->Append(
        ProDeConstants::COMMAND_NEW_DOC,
        menuAccelerator(ProDeConstants::COMMAND_NEW_DOC).Prepend(_T("New")));

    fileMenu->Append(
        ProDeConstants::COMMAND_OPEN_DOC,
        menuAccelerator(
            ProDeConstants::COMMAND_OPEN_DOC).Prepend(_T("Open...")));

    fileMenu->Append(
        ProDeConstants::COMMAND_SAVE_DOC,
        menuAccelerator(
            ProDeConstants::COMMAND_SAVE_DOC).Prepend(_T("&Save")));

    fileMenu->Append(
        ProDeConstants::COMMAND_SAVEAS_DOC,
        menuAccelerator(
            ProDeConstants::COMMAND_SAVEAS_DOC).Prepend(_T("Save &As...")));

    fileMenu->Append(
        ProDeConstants::COMMAND_CLOSE_DOC,
        menuAccelerator(
            ProDeConstants::COMMAND_CLOSE_DOC).Prepend(_T("&Close")));

    fileMenu->AppendSeparator();

    fileMenu->Append(
        ProDeConstants::COMMAND_PRINT,
        menuAccelerator(
            ProDeConstants::COMMAND_PRINT).Prepend(_T("&Print...")));

    fileMenu->Append(
        ProDeConstants::COMMAND_PRINT_PREVIEW,
        menuAccelerator(ProDeConstants::COMMAND_PRINT_PREVIEW).Prepend(
            _T("Print Pre&view...")));

    fileMenu->Append(
        ProDeConstants::COMMAND_EXPORT,
        menuAccelerator(ProDeConstants::COMMAND_EXPORT).Prepend(
            _T("Export...")));

    fileMenu->AppendSeparator();

    fileMenu->Append(
        ProDeConstants::COMMAND_QUIT,
        menuAccelerator(ProDeConstants::COMMAND_QUIT).Prepend(_T("&Quit")));

    // Edit menu
    wxMenu* editMenu = new wxMenu;
    wxMenu* addSubMenu = new wxMenu;
    wxMenu* addFromHDBSubMenu = new wxMenu;
        
    editMenu->Append(
        ProDeConstants::COMMAND_UNDO,
        menuAccelerator(ProDeConstants::COMMAND_UNDO).Prepend(
            _T("&Undo")));

    editMenu->Append(
        ProDeConstants::COMMAND_REDO,
        menuAccelerator(ProDeConstants::COMMAND_REDO).Prepend(
            _T("&Redo")));

    editMenu->AppendSeparator();
        editMenu->Append(
        ProDeConstants::COMMAND_CUT,
        menuAccelerator(ProDeConstants::COMMAND_CUT).Prepend(
            _T("Cu&t")));

    editMenu->Append(
        ProDeConstants::COMMAND_COPY,
        menuAccelerator(ProDeConstants::COMMAND_COPY).Prepend(
            _T("&Copy")));
        
    editMenu->Append(
        ProDeConstants::COMMAND_PASTE,
        menuAccelerator(ProDeConstants::COMMAND_PASTE).Prepend(
            _T("&Paste")));

    editMenu->AppendSeparator();

    addSubMenu->Append(
        ProDeConstants::COMMAND_ADD_FU,
        menuAccelerator(ProDeConstants::COMMAND_ADD_FU).Prepend(
            _T("&Function Unit...")));

    addSubMenu->Append(
        ProDeConstants::COMMAND_ADD_RF,
        menuAccelerator(ProDeConstants::COMMAND_ADD_RF).Prepend(
            _T("&Register File...")));

    addSubMenu->Append(
        ProDeConstants::COMMAND_ADD_BUS,
        menuAccelerator(ProDeConstants::COMMAND_ADD_BUS).Prepend(
            _T("&Transport Bus...")));

    addSubMenu->Append(
        ProDeConstants::COMMAND_ADD_SOCKET,
        menuAccelerator(ProDeConstants::COMMAND_ADD_SOCKET).Prepend(
            _T("&Socket...")));

    addSubMenu->Append(
        ProDeConstants::COMMAND_ADD_BRIDGE,
        menuAccelerator(ProDeConstants::COMMAND_ADD_BRIDGE).Prepend(
            _T("&Bridge...")));

    addSubMenu->Append(
        ProDeConstants::COMMAND_ADD_IU,
        menuAccelerator(ProDeConstants::COMMAND_ADD_IU).Prepend(
            _T("&Immediate Unit...")));

    addSubMenu->Append(
        ProDeConstants::COMMAND_ADD_GCU,
        menuAccelerator(ProDeConstants::COMMAND_ADD_GCU).Prepend(
            _T("&Global Control Unit...")));

    addSubMenu->Append(
        ProDeConstants::COMMAND_ADD_AS,
        menuAccelerator(ProDeConstants::COMMAND_ADD_AS).Prepend(
            _T("&Address Space...")));

    editMenu->Append(ID_ADD_SUBMENU, _T("&Add"), addSubMenu);

    addFromHDBSubMenu->Append(
        ProDeConstants::COMMAND_ADD_FU_FROM_HDB,
        menuAccelerator(ProDeConstants::COMMAND_ADD_FU_FROM_HDB).Prepend(
            _T("&Function Unit...")));

    addFromHDBSubMenu->Append(
        ProDeConstants::COMMAND_ADD_IU_FROM_HDB,
        menuAccelerator(ProDeConstants::COMMAND_ADD_IU_FROM_HDB).Prepend(
            _T("&Immediate Unit...")));

    addFromHDBSubMenu->Append(
        ProDeConstants::COMMAND_ADD_RF_FROM_HDB,
        menuAccelerator(ProDeConstants::COMMAND_ADD_RF_FROM_HDB).Prepend(
            _T("&Register File...")));

    editMenu->Append(
        ID_ADD_FROM_HDB_SUBMENU,
        _T("Add From &HDB"), addFromHDBSubMenu);

    editMenu->Append(
        ProDeConstants::COMMAND_DELETE_COMP,
        menuAccelerator(ProDeConstants::COMMAND_DELETE_COMP).Prepend(
            _T("&Delete")));

    editMenu->Append(
        ProDeConstants::COMMAND_MODIFY_COMP,
        menuAccelerator(ProDeConstants::COMMAND_MODIFY_COMP).Prepend(
            _T("&Modify...")));

    editMenu->AppendSeparator();
    editMenu->Append(
        ProDeConstants::COMMAND_EDIT_ADDRESS_SPACES,
        menuAccelerator(ProDeConstants::COMMAND_EDIT_ADDRESS_SPACES).Prepend(
            _T("Address &Spaces...")));

    editMenu->Append(
        ProDeConstants::COMMAND_EDIT_TEMPLATES,
        menuAccelerator(ProDeConstants::COMMAND_EDIT_TEMPLATES).Prepend(
            _T("Instruction &Templates...")));

    editMenu->Append(
        ProDeConstants::COMMAND_EDIT_IMMEDIATE_SLOTS,
        menuAccelerator(ProDeConstants::COMMAND_EDIT_IMMEDIATE_SLOTS).Prepend(
            _T("&Immediate Slots...")));

    editMenu->Append(
        ProDeConstants::COMMAND_EDIT_BUS_ORDER,
        menuAccelerator(ProDeConstants::COMMAND_EDIT_BUS_ORDER).Prepend(
            _T("Transport Bus &Order...")));

    editMenu->Append(
        ProDeConstants::COMMAND_EDIT_MACHINE,
        menuAccelerator(ProDeConstants::COMMAND_EDIT_MACHINE).Prepend(
            _T("Architecture Features...")));

    // View menu
    wxMenu* viewMenu = new wxMenu;
    wxMenu* zoomSubMenu = new wxMenu;
    zoomSubMenu->Append(
        ProDeConstants::COMMAND_ZOOM_IN,
        menuAccelerator(ProDeConstants::COMMAND_ZOOM_IN).Prepend(
            _T("Zoom &In")));

    zoomSubMenu->Append(
        ProDeConstants::COMMAND_ZOOM_OUT,
        menuAccelerator(ProDeConstants::COMMAND_ZOOM_OUT).Prepend(
            _T("Zoom &Out")));

    zoomSubMenu->AppendSeparator();
    zoomSubMenu->Append(
        ProDeConstants::COMMAND_ZOOM_FIT_WIN,
        menuAccelerator(ProDeConstants::COMMAND_ZOOM_FIT_WIN).Prepend(
            _T("&Fit Window")));

    zoomSubMenu->Append(
        ProDeConstants::COMMAND_ZOOM_FIT_HEI,
        menuAccelerator(ProDeConstants::COMMAND_ZOOM_FIT_HEI).Prepend(
            _T("Fit &Height")));

    zoomSubMenu->Append(
        ProDeConstants::COMMAND_ZOOM_FIT_WID,
        menuAccelerator(ProDeConstants::COMMAND_ZOOM_FIT_WID).Prepend(
            _T("Fit &Width")));

    viewMenu->Append(ID_ZOOM_SUBMENU, _T("&Zoom"), zoomSubMenu);
    viewMenu->AppendSeparator();
    viewMenu->AppendCheckItem(
        ProDeConstants::COMMAND_TOGGLE_UNIT_DETAILS,
        menuAccelerator(ProDeConstants::COMMAND_TOGGLE_UNIT_DETAILS).Prepend(
            _T("Unit &Details")));

    viewMenu->AppendSeparator();
    viewMenu->AppendCheckItem(ID_VIEW_TOOLBAR, _T("&Toolbar"));
    viewMenu->AppendCheckItem(ID_VIEW_STATUSBAR, _T("&Statusbar"));
    viewMenu->Check(ID_VIEW_STATUSBAR, true);
    viewMenu->Check(ID_VIEW_TOOLBAR, true);

    // Tool menu
    wxMenu* toolMenu = new wxMenu;
    toolMenu->Append(
        ProDeConstants::COMMAND_SELECT,
        menuAccelerator(ProDeConstants::COMMAND_SELECT).Prepend(
            _T("&Select")));

    toolMenu->Append(
        ProDeConstants::COMMAND_EDIT_CONNECTIONS,
        menuAccelerator(ProDeConstants::COMMAND_EDIT_CONNECTIONS).Prepend(
            _T("&Edit Connections")));

    toolMenu->AppendSeparator();
    toolMenu->Append(
        ProDeConstants::COMMAND_FULLY_CONNECT_BUSSES,
        menuAccelerator(ProDeConstants::COMMAND_FULLY_CONNECT_BUSSES).Prepend(
            _T("&Fully Connect IC")));

    toolMenu->Append(
        ProDeConstants::COMMAND_VLIW_CONNECT_IC,
        menuAccelerator(ProDeConstants::COMMAND_VLIW_CONNECT_IC).Prepend(
            _T("&VLIW Connect IC")));

    toolMenu->Append(
        ProDeConstants::COMMAND_BLOCKS_CONNECT_IC,
        menuAccelerator(ProDeConstants::COMMAND_BLOCKS_CONNECT_IC)
            .Prepend(_T("&Blocks Connect IC")));

    toolMenu->Append(
        ProDeConstants::COMMAND_VERIFY_MACHINE,
        menuAccelerator(ProDeConstants::COMMAND_VERIFY_MACHINE).Prepend(
            _T("&Verify...")));

    toolMenu->Append(
        ProDeConstants::COMMAND_IMPLEMENTATION,
        menuAccelerator(ProDeConstants::COMMAND_IMPLEMENTATION).Prepend(
            _T("&Processor Implementation...")));
    
    toolMenu->Append(
        ProDeConstants::COMMAND_CALL_EXPLORER_PLUGIN,
        menuAccelerator(ProDeConstants::COMMAND_CALL_EXPLORER_PLUGIN).Prepend(
            _T("&Call Explorer Plugin...")));

    // Options menu
    wxMenu* optionsMenu = createOptionsMenu();

    // Help menu
    wxMenu* helpMenu = createHelpMenu();

    // add menus to the menubar
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(editMenu, _T("&Edit"));
    menuBar->Append(viewMenu, _T("&View"));
    menuBar->Append(toolMenu, _T("&Tools"));
    menuBar->Append(optionsMenu, _T("&Options"));
    menuBar->Append(helpMenu, _T("&Help"));

    SetMenuBar(menuBar);
}

/**
 * Returns a menu accelerator string for a command.
 *
 * The returned string can be appended to the menu item text to set the
 * keyboard shortcut defined in the options for the command.
 *
 * @param id Command ID.
 * @return Accelerator string.
 */
wxString
MainFrame::menuAccelerator(int id) {

    const CommandRegistry* reg = wxGetApp().commandRegistry();
    const GUIOptions* options = wxGetApp().options();
    std::string cmdName = reg->commandName(id);
    const KeyboardShortcut* sc = options->keyboardShortcut(cmdName);

    if (sc != NULL) {
        wxString accel = _T("\t");
        if (sc->ctrl()) {
            accel.Append(_T("Ctrl-"));
            accel.Append(WxConversion::toWxString(sc->key()));
            return accel;
        } else if (sc->alt()) {
            accel.Append(_T("Alt-"));
            accel.Append(WxConversion::toWxString(sc->key()));
            return accel;
        } else if (sc->fKey() > 0) {
            accel.Append(_T("F"));
            accel.Append(WxConversion::toWxString(sc->fKey()));
            return accel;
        } else {
            // Invalid shortcut.
            return _T("");
        }
    }
    return _T("");
}

/**
 * Creates the toolbar.
 */
void
MainFrame::createToolbar() {

    // delete old toolbar
    if (toolbar_ != NULL) {
        delete toolbar_;
        toolbar_ = NULL;
    }

    CommandRegistry* registry = wxGetApp().commandRegistry();
    ProDeOptions* options = wxGetApp().options();
    wxString iconPath = WxConversion::toWxString(Environment::iconDirPath());

    toolbar_ = options->createToolbar(this, *registry, iconPath);

    // set new toolbar as the MainFrame toolbar
    SetToolBar(toolbar_);
    toolbar_->Show(true);
}

/**
 * Performs routines before exiting a program
 *
 * @param event Command event to handle.
 */
void
MainFrame::OnExit(wxCommandEvent& WXUNUSED(event)) {
    Close();
}
