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
 * @file DropDownMenu.cc
 *
 * Definition of DropDownMenu class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "DropDownMenu.hh"
#include "OSEdConstants.hh"
#include "OSEdTextGenerator.hh"
#include "OSEd.hh"
#include "WxConversion.hh"
#include "GUICommand.hh"
#include "CommandRegistry.hh"

using boost::format;

BEGIN_EVENT_TABLE(DropDownMenu, wxMenu)
    EVT_MENU_RANGE(OSEdConstants::CMD_ADD_MODULE, OSEdConstants::CMD_SIMULATE, DropDownMenu::onMenuEvent)
END_EVENT_TABLE()

/**
 * Constructor.
 *
 * param type The type of the drop down menu.
 */
DropDownMenu::DropDownMenu(MenuType type) : wxMenu() {

    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    
    switch (type) {
    case MENU_PATH: {
        format fmt = texts.text(OSEdTextGenerator::TXT_MENU_ADD_MODULE);
        Append(
            OSEdConstants::CMD_ADD_MODULE, 
            WxConversion::toWxString(fmt.str()));
    }
        break;
    case MENU_MODULE: {
        format fmt = texts.text(OSEdTextGenerator::TXT_MENU_ADD_OPERATION);
        Append(
            OSEdConstants::CMD_ADD_OPERATION, 
            WxConversion::toWxString(fmt.str()));

        fmt = texts.text(OSEdTextGenerator::TXT_MENU_REMOVE_MODULE);
        Append(
            OSEdConstants::CMD_REMOVE_MODULE, 
            WxConversion::toWxString(fmt.str()));
        
        fmt = texts.text(OSEdTextGenerator::TXT_MENU_BUILD);
        Append(
            OSEdConstants::CMD_BUILD, WxConversion::toWxString(fmt.str()));
    }
        break;
    case MENU_OPERATION: {
        format fmt = texts.text(OSEdTextGenerator::TXT_MENU_MODIFY_PROPERTIES);
        Append(
            OSEdConstants::CMD_PROPERTIES, 
            WxConversion::toWxString(fmt.str()));
        
        fmt = texts.text(OSEdTextGenerator::TXT_MENU_MODIFY_BEHAVIOR);
        Append(
            OSEdConstants::CMD_MODIFY_BEHAVIOR,
            WxConversion::toWxString(fmt.str()));
        
        fmt = texts.text(OSEdTextGenerator::TXT_MENU_REMOVE_OPERATION);
        Append(
            OSEdConstants::CMD_REMOVE_OPERATION, 
            WxConversion::toWxString(fmt.str()));

        fmt = texts.text(OSEdTextGenerator::TXT_MENU_SIMULATE);
        Append(
            OSEdConstants::CMD_SIMULATE,
            WxConversion::toWxString(fmt.str()));
    }
        break;
    default:
        break;
    }
    
    updateMenu();
}

/**
 * Destructor.
 */
DropDownMenu::~DropDownMenu() {
}

/**
 * Handles the menu commands.
 *
 * @param event The event to be handled.
 */
void
DropDownMenu::onMenuEvent(wxCommandEvent& event) {
    wxGetApp().mainFrame()->onCommandEvent(event);
}

/**
 * Updates the menus enabled/disabled status.
 */
void
DropDownMenu::updateMenu() {
    CommandRegistry* registry = wxGetApp().mainFrame()->registry();
    GUICommand* command = registry->firstCommand();
    while (command != NULL) {
        if (FindItem(command->id()) != NULL) {
            if (command->isEnabled()) {
                Enable(command->id(), true);
            } else {
                Enable(command->id(), false);
            }
        }
        command = registry->nextCommand();
    }
}
