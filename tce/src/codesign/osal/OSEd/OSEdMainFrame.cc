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
 * @file OSEdMainFrame.cc
 *
 * Definition of OSEdMainFrame class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <wx/bitmap.h>
#include <wx/imagpng.h>
#include <boost/format.hpp>
#include <string>

#include "OSEdMainFrame.hh"
#include "OSEdTreeView.hh"
#include "OSEdInfoView.hh"
#include "OSEdInformer.hh"
#include "OSEdConstants.hh"
#include "GUICommand.hh"
#include "OSEdQuitCmd.hh"
#include "OSEdAboutCmd.hh"
#include "OSEdPropertiesCmd.hh"
#include "OSEdAddModuleCmd.hh"
#include "OSEdAddOperationCmd.hh"
#include "OSEdOptionsCmd.hh"
#include "OSEdSimulateCmd.hh"
#include "OSEdBuildCmd.hh"
#include "OSEdBuildAllCmd.hh"
#include "OSEdUserManualCmd.hh"
#include "OSEdRemoveModuleCmd.hh"
#include "OSEdModifyBehaviorCmd.hh"
#include "OSEdRemoveOperationCmd.hh"
#include "OSEdMemoryCmd.hh"
#include "OSEd.hh"
#include "ErrorDialog.hh"
#include "Application.hh"
#include "OSEdTextGenerator.hh"
#include "WxConversion.hh"
#include "CommandRegistry.hh"
#include "Environment.hh"
#include "FileSystem.hh"

using boost::format;
using std::string;

BEGIN_EVENT_TABLE(OSEdMainFrame, wxFrame)
    EVT_MENU_RANGE(OSEdConstants::CMD_QUIT, OSEdConstants::CMD_MEMORY, OSEdMainFrame::onCommandEvent)
END_EVENT_TABLE()

/**
 * Constructor.
 *
 * @param title Window title.
 * @param pos Window position.
 * @param size Window size.
 */
OSEdMainFrame::OSEdMainFrame(
    const wxString& title,
    const wxPoint& pos,
    const wxSize& size) : wxFrame((wxFrame *)NULL, -1, title, pos, size),
                          registry_(NULL), treeView_(NULL), informer_(NULL) {

    wxImage::AddHandler(new wxPNGHandler);
    // set logo
    string logoPath = 
        Environment::bitmapsDirPath(
            WxConversion::toString(OSEdConstants::APPLICATION_NAME));
    logoPath += FileSystem::DIRECTORY_SEPARATOR + 
        OSEdConstants::LOGO_NAME;
    wxBitmap bmp;
    bmp.LoadFile(WxConversion::toWxString(logoPath), wxBITMAP_TYPE_PNG);
    wxIcon logo;
    logo.CopyFromBitmap(bmp);
    SetIcon(logo);

    OSEdTextGenerator& texts = OSEdTextGenerator::instance();

    // create default menu bar
    wxMenu* menu = new wxMenu;
    format fmt = texts.text(OSEdTextGenerator::TXT_MENU_OPTIONS);
    menu->Append(
        OSEdConstants::CMD_OPTIONS, WxConversion::toWxString(fmt.str()));
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_QUIT);
    menu->Append(OSEdConstants::CMD_QUIT, WxConversion::toWxString(fmt.str()));
    
    wxMenuBar* menuBar = new wxMenuBar;
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_FILE);
    menuBar->Append(menu, WxConversion::toWxString(fmt.str()));
    
    menu = new wxMenu;
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_ADD_OPERATION);
    menu->Append(
        OSEdConstants::CMD_ADD_OPERATION, WxConversion::toWxString(fmt.str()));
    
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_REMOVE_OPERATION);
    menu->Append(
        OSEdConstants::CMD_REMOVE_OPERATION, 
        WxConversion::toWxString(fmt.str()));
    
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_ADD_MODULE);
    menu->Append(
        OSEdConstants::CMD_ADD_MODULE, WxConversion::toWxString(fmt.str()));
    
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_REMOVE_MODULE);
    menu->Append(
        OSEdConstants::CMD_REMOVE_MODULE, WxConversion::toWxString(fmt.str()));
    
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_MODIFY_PROPERTIES);
    menu->Append(
        OSEdConstants::CMD_PROPERTIES, WxConversion::toWxString(fmt.str()));
    
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_MODIFY_BEHAVIOR);
    menu->Append(
        OSEdConstants::CMD_MODIFY_BEHAVIOR, 
        WxConversion::toWxString(fmt.str()));
    
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_BUILD);
    menu->Append(
        OSEdConstants::CMD_BUILD, WxConversion::toWxString(fmt.str()));
    
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_BUILD_ALL);
    menu->Append(
        OSEdConstants::CMD_BUILD_ALL, WxConversion::toWxString(fmt.str()));
    
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_SIMULATE);
    menu->Append(
        OSEdConstants::CMD_SIMULATE, WxConversion::toWxString(fmt.str()));
    
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_MEMORY);
    menu->Append(
        OSEdConstants::CMD_MEMORY, WxConversion::toWxString(fmt.str()));
    
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_TOOLS);
    menuBar->Append(menu, WxConversion::toWxString(fmt.str()));
    
    menu = new wxMenu;
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_USER_MANUAL);
    menu->Append(
        OSEdConstants::CMD_USER_MANUAL, WxConversion::toWxString(fmt.str()));
    
    fmt = texts.text(OSEdTextGenerator::TXT_MENU_ABOUT);
    menu->Append(
        OSEdConstants::CMD_ABOUT, WxConversion::toWxString(fmt.str()));

    fmt = texts.text(OSEdTextGenerator::TXT_MENU_HELP);
    menuBar->Append(menu, WxConversion::toWxString(fmt.str()));
    
    SetMenuBar(menuBar);
    statusBar_ = CreateStatusBar();
    
    // add all custom commands to command registry
    registry_ = new CommandRegistry();
    registry_->addCommand(new OSEdQuitCmd());
    registry_->addCommand(new OSEdAboutCmd());
    registry_->addCommand(new OSEdPropertiesCmd());
    registry_->addCommand(new OSEdAddModuleCmd());
    registry_->addCommand(new OSEdAddOperationCmd());
    registry_->addCommand(new OSEdSimulateCmd());
    registry_->addCommand(new OSEdOptionsCmd());
    registry_->addCommand(new OSEdBuildCmd());
    registry_->addCommand(new OSEdBuildAllCmd());
    registry_->addCommand(new OSEdUserManualCmd());
    registry_->addCommand(new OSEdRemoveModuleCmd());
    registry_->addCommand(new OSEdModifyBehaviorCmd());
    registry_->addCommand(new OSEdRemoveOperationCmd());
    registry_->addCommand(new OSEdMemoryCmd());

    // create splitted window
    wxSplitterWindow* splitter = new wxSplitterWindow(
        this, -1, wxPoint(0, 0), wxSize(900, 500), wxSP_3D);
    
    OSEdInfoView* infoView = new OSEdInfoView(splitter);
    treeView_ = new OSEdTreeView(splitter, infoView);
    
    splitter->SplitVertically(treeView_, infoView);

    informer_ = new OSEdInformer();
}

/**
 * Destructor.
 */
OSEdMainFrame::~OSEdMainFrame() {
    delete registry_;
    registry_ = NULL;
    delete informer_;
    informer_ = NULL;
}

/**
 * Handles menu events.
 *
 * @param event Event to be handled. 
 */
void
OSEdMainFrame::onCommandEvent(wxCommandEvent& event) {
    GUICommand* command = registry_->createCommand(event.GetId());
    if (command == NULL) {
        OSEdTextGenerator& texts = OSEdTextGenerator::instance();
        format fmt = texts.text(OSEdTextGenerator::TXT_ERROR_NO_HANDLER);
        ErrorDialog dialog(this, WxConversion::toWxString(fmt.str()));
        dialog.ShowModal();
        return;
    }
    command->setParentWindow(this);
    command->Do();
    delete command;
}

/**
 * Returns pointer to the tree view.
 *
 * @return Pointer to the tree view.
 */
OSEdTreeView*
OSEdMainFrame::treeView() const {
    return treeView_;
}

/**
 * Returns the command registry.
 *
 * @return the command registry.
 */
CommandRegistry*
OSEdMainFrame::registry() const {
    return registry_;
}

/**
 * Returns informer instance.
 *
 * @return The informer instance.
 */
OSEdInformer*
OSEdMainFrame::informer() const {
    return informer_;
}

/**
 * Returns status bar of the frame.
 *
 * @return Status bar.
 */
wxStatusBar*
OSEdMainFrame::statusBar() const {
    return statusBar_;
}

/**
 * Updates the menu bar disabled/enabled states.
 */
void
OSEdMainFrame::updateMenuBar() {
    wxMenuBar* menubar = GetMenuBar();
    GUICommand* command = registry_->firstCommand();
    while (command != NULL) {
        if (menubar->FindItem(command->id()) != NULL) {
            if (command->isEnabled()) {
                menubar->Enable(command->id(), true);
            } else {
                menubar->Enable(command->id(), false);
            }
        }
        command = registry_->nextCommand();
    }
}
