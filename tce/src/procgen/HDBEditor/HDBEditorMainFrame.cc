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
 * @file HDBEditorMainFrame.cc
 *
 * Definition of HDBEditorMainFrame class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */


#include <wx/sizer.h>
#include <wx/statusbr.h>

#include "HDBEditorMainFrame.hh"
#include "CommandRegistry.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"

#include "HDBEditorConstants.hh"
#include "GUICommand.hh"
#include "CachedHDBManager.hh"
#include "HDBRegistry.hh"
#include "HDBBrowserWindow.hh"

#include "OpenHDBCmd.hh"
#include "CreateHDBCmd.hh"
#include "HDBEditorDeleteCmd.hh"
#include "HDBEditorModifyCmd.hh"
#include "AddFUArchFromADFCmd.hh"
#include "HDBEditorQuitCmd.hh"
#include "AddFUImplementationCmd.hh"
#include "AddRFArchitectureCmd.hh"
#include "AddRFImplementationCmd.hh"
#include "AddCostFunctionPluginCmd.hh"
#include "AddFUEntryCmd.hh"
#include "AddRFEntryCmd.hh"
#include "AddBusEntryCmd.hh"
#include "AddSocketEntryCmd.hh"
#include "SetCostFunctionPluginCmd.hh"
#include "HDBEditorAboutCmd.hh"
#include "UserManualCmd.hh"
#include "FileSystem.hh"

BEGIN_EVENT_TABLE(HDBEditorMainFrame, wxFrame)
    //    EVT_KEY_DOWN(MainFrame::onKeyEvent)
    EVT_MENU(-1, HDBEditorMainFrame::onCommandEvent)
    EVT_UPDATE_UI(-1,  HDBEditorMainFrame::onUpdateUI)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param title Title of the MainFrame.
 * @param position Position of the MainFrame on the screen.
 * @param size Size of the MainFrame on the screen.
 */
HDBEditorMainFrame::HDBEditorMainFrame(
    const wxString& title,
    const wxPoint& position,
    const wxSize& size):
    wxFrame(NULL, -1, title, position, size),
    hdb_(NULL) {

    commandRegistry_ = new CommandRegistry();

    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    browser_ = new HDBBrowserWindow(this, -1);
    sizer->Add(browser_, 1, wxGROW);
    Fit();

    // Create the default menubar.
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(HDBEditorConstants::COMMAND_OPEN_HDB, _T("&Open HDB..."));
    fileMenu->Append(
        HDBEditorConstants::COMMAND_CREATE_HDB, _T("&Create HDB..."));
    fileMenu->AppendSeparator();
    fileMenu->Append(HDBEditorConstants::COMMAND_QUIT, _T("&Quit"));

    wxMenu* editMenu = new wxMenu;
    wxMenu* addSubMenu = new wxMenu;

    addSubMenu->Append(
        HDBEditorConstants::COMMAND_ADD_FU_FROM_ADF,
        _T("FU Architecture From ADF..."));
    addSubMenu->Append(
        HDBEditorConstants::COMMAND_ADD_RF_ARCHITECTURE,
        _T("RF Architecture..."));
    addSubMenu->AppendSeparator();

    addSubMenu->Append(
        HDBEditorConstants::COMMAND_ADD_FU_IMPLEMENTATION,
        _T("FU Entry Implementation..."));
    addSubMenu->Append(
        HDBEditorConstants::COMMAND_ADD_RF_IMPLEMENTATION,
        _T("RF Entry Implementation..."));

    addSubMenu->AppendSeparator();
    addSubMenu->Append(
        HDBEditorConstants::COMMAND_ADD_FU_ENTRY, _T("FU Entry"));

    addSubMenu->Append(
        HDBEditorConstants::COMMAND_ADD_RF_ENTRY, _T("RF Entry"));
    addSubMenu->Append(
        HDBEditorConstants::COMMAND_ADD_BUS_ENTRY, _T("Bus Entry"));

    addSubMenu->Append(
        HDBEditorConstants::COMMAND_ADD_SOCKET_ENTRY, _T("Socket Entry"));

    addSubMenu->AppendSeparator();
    addSubMenu->Append(
        HDBEditorConstants::COMMAND_ADD_COST_PLUGIN,
        _T("Cost Function Plugin..."));    
    editMenu->Append(-1, _T("Add"), addSubMenu);
    editMenu->AppendSeparator();
    editMenu->Append(
        HDBEditorConstants::COMMAND_SET_COST_PLUGIN,
        _T("Set Cost Function Plugin"));

    editMenu->AppendSeparator();
    editMenu->Append(HDBEditorConstants::COMMAND_DELETE, _T("Delete"));
    editMenu->Append(HDBEditorConstants::COMMAND_MODIFY, _T("Modify..."));

    wxMenu* helpMenu = new wxMenu;

    helpMenu->Append(UserManualCmd::COMMAND_ID, _T("User Manual..."));
    helpMenu->Append(HDBEditorConstants::COMMAND_ABOUT, _T("About..."));

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(editMenu, _T("&Edit"));
    menuBar->Append(helpMenu, _T("&Help"));
    SetMenuBar(menuBar);

    CreateStatusBar();
    SetSizer(sizer);

    commandRegistry_->addCommand(new OpenHDBCmd());
    commandRegistry_->addCommand(new CreateHDBCmd());
    commandRegistry_->addCommand(new HDBEditorDeleteCmd());
    commandRegistry_->addCommand(new HDBEditorModifyCmd());
    commandRegistry_->addCommand(new AddFUArchFromADFCmd());
    commandRegistry_->addCommand(new HDBEditorQuitCmd());
    commandRegistry_->addCommand(new AddFUImplementationCmd());
    commandRegistry_->addCommand(new AddRFArchitectureCmd());
    commandRegistry_->addCommand(new AddRFImplementationCmd());
    commandRegistry_->addCommand(new AddFUEntryCmd());
    commandRegistry_->addCommand(new AddRFEntryCmd());
    commandRegistry_->addCommand(new AddBusEntryCmd());
    commandRegistry_->addCommand(new AddSocketEntryCmd());
    commandRegistry_->addCommand(new AddCostFunctionPluginCmd());
    commandRegistry_->addCommand(new SetCostFunctionPluginCmd());
    commandRegistry_->addCommand(new HDBEditorAboutCmd());
    commandRegistry_->addCommand(new UserManualCmd());

    SetSizeHints(400,300);
    SetSize(600, 400);
}


/**
 * The Destructor.
 */
HDBEditorMainFrame::~HDBEditorMainFrame() {
}



/**
 * Handles menu and toolbar events.
 *
 * @param event Event to handle.
 */
void
HDBEditorMainFrame::onCommandEvent(wxCommandEvent& event) {
    
    GUICommand* command = commandRegistry_->createCommand(event.GetId());

    if (command == NULL) {
	ErrorDialog dialog(
            this, _T("No handler found for the command event!"));
	dialog.ShowModal();
	return;
    }

    command->setParentWindow(this);
    command->Do();
}

/**
 * Updates toolbar and menubar item enabled/disabled states.
 *
 * @param event Update event for the menu or toolbar item to be updated.
 */
void
HDBEditorMainFrame::onUpdateUI(wxUpdateUIEvent& event) {

    GUICommand* command = commandRegistry_->createCommand(event.GetId());

    if (command == NULL || command->isEnabled()) {
	event.Enable(true);
    } else {
	event.Enable(false);
    }

    delete command;
}


/**
 * Sets the HDB to be displayed and modified with the main frame.
 *
 * @param HDB to be opened in the main frame.
 * @return True, if the HDB was succesfully opened, false if not.
 */
bool
HDBEditorMainFrame::setHDB(const std::string& hdbFile) {

    if (hdb_ != NULL) {
        hdb_ = NULL;
    }

    try {
        hdb_ = &HDB::HDBRegistry::instance().hdb(hdbFile);
    } catch (Exception& e) {
        wxString message = _T("Error opening HDB ");
        message.Append(WxConversion::toWxString(hdbFile));
        message.Append(_T(":\n\n"));
        message.Append(WxConversion::toWxString(e.errorMessage()));
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return false;
    }

    browser_->setHDBManager(*hdb_);
    std::string title = "HDB Editor - " + FileSystem::fileOfPath(hdbFile);
    SetTitle(WxConversion::toWxString(title));
    return true;
}

/**
 * Creates a new HDB and opens it in the main frame.
 *
 * @param filePath Full path of the HDB to be created.
 * @return True, if the HDB was succesfully created and opened, false if not.
 */
bool
HDBEditorMainFrame::createHDB(const std::string& filePath) {

    if (hdb_ != NULL) {
        hdb_ = NULL;
    }

    try {
        hdb_ = &HDB::CachedHDBManager::createNew(filePath);
    } catch (Exception& e) {
        wxString message = _T("Error creating HDB ");
        message.Append(WxConversion::toWxString(filePath));
        message.Append(_T(":\n\n"));
        message.Append(WxConversion::toWxString(e.errorMessage()));
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return false;
    }

    browser_->setHDBManager(*hdb_);
    std::string title = "HDB Editor - " + FileSystem::fileOfPath(filePath);
    SetTitle(WxConversion::toWxString(title));

    return true;

}

/**
 * Returns pointer to the HDBManager managing the current HDB.
 *
 * @return Pointer to the HDBManager of the main frame.
 */
HDB::HDBManager*
HDBEditorMainFrame::hdbManager() {
    return hdb_;
}


/**
 * Updates the HDB window.
 */
void
HDBEditorMainFrame::update() {
    browser_->update();
}


/**
 * Returns pointer to the HDB browser window.
 *
 * @return HDB Browser window of the application.
 */
HDBBrowserWindow*
HDBEditorMainFrame::browser() const {
    return browser_;
}
