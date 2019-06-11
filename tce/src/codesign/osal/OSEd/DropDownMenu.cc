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
