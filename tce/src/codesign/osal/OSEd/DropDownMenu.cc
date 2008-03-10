/**
 * @file DropDownMenu.cc
 *
 * Definition of DropDownMenu class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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
