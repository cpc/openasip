/**
 * @file AddRFEntryCmd.cc
 *
 * Implementation of AddRFEntryCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "AddRFEntryCmd.hh"
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
AddRFEntryCmd::AddRFEntryCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_ADD_RF_ENTRY, NULL) {
}


/**
 * The Destructor.
 */
AddRFEntryCmd::~AddRFEntryCmd() {
}

/**
 * Executes the command.
 */
bool
AddRFEntryCmd::Do() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    RowID id = manager->addRFEntry();

    wxGetApp().mainFrame().update();
    wxGetApp().mainFrame().browser()->selectRFEntry(id);

    return true;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
AddRFEntryCmd::icon() const {
    return "";
}


/**
 * Returns command identifier for this command.
 *
 * @return Command identifier of this command.
 */
int
AddRFEntryCmd::id() const {
    return HDBEditorConstants::COMMAND_ADD_RF_ENTRY;
}


/**
 * Creates and returns new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddRFEntryCmd*
AddRFEntryCmd::create() const {
    return new AddRFEntryCmd();
}


/**
 * Returns true, if the command should be enabled in the menu/toolbar.
 *
 * @return True, if the command is enabled, false if not.
 */
bool
AddRFEntryCmd::isEnabled() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    return true;
}

