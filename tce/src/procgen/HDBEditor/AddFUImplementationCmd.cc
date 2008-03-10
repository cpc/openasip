/**
 * @file AddFUImplementationCmd.cc
 *
 * Implementation of AddFUImplementationCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "AddFUImplementationCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"
#include "HDBBrowserWindow.hh"
#include "HDBManager.hh"
#include "FUImplementationDialog.hh"
#include "FUImplementation.hh"
#include "FUEntry.hh"
#include "FUArchitecture.hh"
#include "ErrorDialog.hh"
#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "FUPortImplementation.hh"


/**
 * The Constructor.
 */
AddFUImplementationCmd::AddFUImplementationCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_ADD_FU_IMPLEMENTATION, NULL) {
}


/**
 * The Destructor.
 */
AddFUImplementationCmd::~AddFUImplementationCmd() {
}

/**
 * Executes the command.
 */
bool
AddFUImplementationCmd::Do() {

    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();
    if (manager == NULL) {
        return false;
    }

    HDBBrowserWindow* browser = wxGetApp().mainFrame().browser();

    HDB::FUImplementation* implementation =
        new HDB::FUImplementation("", "", "","", "", "");

    HDB::FUEntry* entry = NULL;

    if (browser->isFUEntrySelected()) {
        entry = manager->fuByEntryID(browser->selectedFUEntry());
    } else if (browser->isFUArchitectureSelected()) {
        int id = manager->addFUEntry();
        manager->setArchitectureForFU(id, browser->selectedFUArchitecture());
        entry = manager->fuByEntryID(id);
    } else {
        return false;
    }

    const TTAMachine::FunctionUnit& arch =
        entry->architecture().architecture();

    for (int i = 0; i < arch.portCount(); i++) {
        new HDB::FUPortImplementation(
            "", arch.port(i)->name(), "", "", "", *implementation);
    }

    FUImplementationDialog dialog(parentWindow(), -1, *implementation, arch);

    if (dialog.ShowModal() == wxID_OK) {
        try {
            entry->setImplementation(implementation);
            int id = manager->addFUImplementation(*entry);
            wxGetApp().mainFrame().update();
            wxGetApp().mainFrame().browser()->selectFUImplementation(id);
        } catch (Exception& e) {
            wxString message = _T("Error:\n");
            message.Append(WxConversion::toWxString(e.errorMessage()));
            message.Append(_T("\n"));
            message.Append(WxConversion::toWxString(e.lineNum()));
             message.Append(_T(": "));
            message.Append(WxConversion::toWxString(e.fileName()));
            ErrorDialog dialog(parentWindow(), message);
            dialog.ShowModal();
            delete entry;
            delete implementation;
            return false;
        }
    } else {
        delete implementation;
    }
    delete entry;
    return true;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
AddFUImplementationCmd::icon() const {
    return "";
}

/**
 * Returns the command id.
 *
 * @return Command identifier.
 */
int
AddFUImplementationCmd::id() const {
    return HDBEditorConstants::COMMAND_ADD_FU_IMPLEMENTATION;
}


/**
 * Creates a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddFUImplementationCmd*
AddFUImplementationCmd::create() const {
    return new AddFUImplementationCmd();
}


/**
 * Returns true if the command should be enabled in the tool/menubar.
 *
 * @return True if the command is enabled, false if not.
 */
bool
AddFUImplementationCmd::isEnabled() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    HDBBrowserWindow* browser = wxGetApp().mainFrame().browser();

    if (browser->isFUArchitectureSelected()) {
        return true;
    } else if (browser->isFUEntrySelected()) {
        HDB::FUEntry* entry = manager->fuByEntryID(browser->selectedFUEntry());
        if (entry->hasArchitecture() && !entry->hasImplementation()) {
            delete entry;
            return true;
        }
        delete entry;
    }
    return false;
}
