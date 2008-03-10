/**
 * @file AddFUEntryCmd.cc
 *
 * Implementation of AddFUEntryCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "AddFUEntryCmd.hh"
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
AddFUEntryCmd::AddFUEntryCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_ADD_FU_ENTRY, NULL) {
}


/**
 * The Destructor.
 */
AddFUEntryCmd::~AddFUEntryCmd() {
}

/**
 * Executes the command.
 */
bool
AddFUEntryCmd::Do() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    RowID id = manager->addFUEntry();

    wxGetApp().mainFrame().update();
    wxGetApp().mainFrame().browser()->selectFUEntry(id);

    return true;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
AddFUEntryCmd::icon() const {
    return "";
}

/**
 * Returns command id.
 *
 * @return Command identifier.
 */
int
AddFUEntryCmd::id() const {
    return HDBEditorConstants::COMMAND_ADD_FU_ENTRY;
}


/**
 * Creates a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddFUEntryCmd*
AddFUEntryCmd::create() const {
    return new AddFUEntryCmd();
}


/**
 * Returns true if the command should be enabled in the menu/toolbar.
 *
 * @return True if the command is enabled, false if not.
 */
bool
AddFUEntryCmd::isEnabled() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    return true;
}
