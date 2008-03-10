/**
 * @file SetFUArchitectureCmd.cc
 *
 * Implementation of SetFUArchitectureCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "SetFUArchitectureCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"
#include "HDBManager.hh"
#include "FUEntry.hh"
#include "HDBBrowserWindow.hh"
#include "Conversion.hh"

using namespace HDB;

/**
 * The Constructor.
 */
SetFUArchitectureCmd::SetFUArchitectureCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_SET_FU_ARCH, NULL) {
}


/**
 * The Destructor.
 */
SetFUArchitectureCmd::~SetFUArchitectureCmd() {
}

/**
 * Executes the command.
 */
bool
SetFUArchitectureCmd::Do() {
    HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    const FUEntry* entry = manager->fuByID(
        wxGetApp().mainFrame().browser()->selectedFUEntry());

    wxString fus[1000];
    const std::set<RowID> fuIDs = manager->fuArchitectureIDs();
    std::set<RowID>::iterator iter = fuIDs.begin();
    int i = 0;
    for (;iter != fuIDs.end(); iter++) {
        fus[i] = WxConversion::toWxString(*iter);
        i++;
    }
    
    wxSingleChoiceDialog choicer(
        parentWindow(), _T("Choose Architecture"), _T("Choose Architecture"),
        i, fus);

    if (choicer.ShowModal() != wxID_OK) {
        return false;
    }


    std::string archID = WxConversion::toString(choicer.GetStringSelection());
    RowID rowID = Conversion::toInt(archID);
    manager->unsetArchitectureForFU(entry->id());
    manager->setArchitectureForFU(entry->id(), rowID);

    wxGetApp().mainFrame().update();
    wxGetApp().mainFrame().browser()->selectFUEntry(entry->id());

    return true;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
SetFUArchitectureCmd::icon() const {
    return "";

}


/**
 * Returns the command id.
 *
 * @return Command identifier for this command.
 */
int
SetFUArchitectureCmd::id() const {
    return HDBEditorConstants::COMMAND_SET_FU_ARCH;
}


/**
 * Creates and returns a new instance of the command.
 *
 * @return Newly created instance of this command.
 */
SetFUArchitectureCmd*
SetFUArchitectureCmd::create() const {
    return new SetFUArchitectureCmd();
}

/**
 * Returns true if the command should be enabled in the menu/toolbar.
 *
 * @return True if the command is enabled, false if not.
 */
bool
SetFUArchitectureCmd::isEnabled() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    HDBBrowserWindow* browser = wxGetApp().mainFrame().browser();

    if (!browser->isFUEntrySelected() ||
        manager->fuByID(browser->selectedFUEntry())->hasArchitecture()) {

        return false;
    }
    return true;
}

