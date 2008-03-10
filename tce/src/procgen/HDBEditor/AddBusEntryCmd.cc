/**
 * @file AddBusEntryCmd.cc
 *
 * Implementation of AddBusEntryCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "AddBusEntryCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"
#include "DBTypes.hh"
#include "HDBManager.hh"
#include "HDBBrowserWindow.hh"

/**
 * The Constructor.
 */
AddBusEntryCmd::AddBusEntryCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_ADD_BUS_ENTRY, NULL) {
}


/**
 * The Destructor.
 */
AddBusEntryCmd::~AddBusEntryCmd() {
}

/**
 * Executes the command.
 */
bool
AddBusEntryCmd::Do() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    RowID id = manager->addBusEntry();

    wxGetApp().mainFrame().update();
    wxGetApp().mainFrame().browser()->selectBusEntry(id);

    return true;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
AddBusEntryCmd::icon() const {
    return "";
}


/**
 * Returns command identifier for this command.
 *
 * @return Command identifier of this command.
 */
int
AddBusEntryCmd::id() const {
    return HDBEditorConstants::COMMAND_ADD_BUS_ENTRY;
}


/**
 * Creates and returns new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddBusEntryCmd*
AddBusEntryCmd::create() const {
    return new AddBusEntryCmd();
}


/**
 * Returns true, if the command should be enabled in the menu/toolbar.
 *
 * @return True, if the command is enabled, false if not.
 */
bool
AddBusEntryCmd::isEnabled() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    return true;
}

