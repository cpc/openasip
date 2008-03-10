/**
 * @file AddSocketEntryCmd.cc
 *
 * Implementation of AddSocketEntryCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "AddSocketEntryCmd.hh"
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
AddSocketEntryCmd::AddSocketEntryCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_ADD_SOCKET_ENTRY, NULL) {
}


/**
 * The Destructor.
 */
AddSocketEntryCmd::~AddSocketEntryCmd() {
}

/**
 * Executes the command.
 */
bool
AddSocketEntryCmd::Do() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    RowID id = manager->addSocketEntry();

    wxGetApp().mainFrame().update();
    wxGetApp().mainFrame().browser()->selectSocketEntry(id);

    return true;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
AddSocketEntryCmd::icon() const {
    return "";
}


/**
 * Returns command identifier for this command.
 *
 * @return Command identifier of this command.
 */
int
AddSocketEntryCmd::id() const {
    return HDBEditorConstants::COMMAND_ADD_SOCKET_ENTRY;
}


/**
 * Creates and returns new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddSocketEntryCmd*
AddSocketEntryCmd::create() const {
    return new AddSocketEntryCmd();
}


/**
 * Returns true, if the command should be enabled in the menu/toolbar.
 *
 * @return True, if the command is enabled, false if not.
 */
bool
AddSocketEntryCmd::isEnabled() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    return true;
}

