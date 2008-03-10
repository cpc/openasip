/**
 * @file AddFUArchFromADFCmd.cc
 *
 * Implementation of AddFUArchFromADFCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "AddFUArchFromADFCmd.hh"
#include "WxConversion.hh"

#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"
#include "HDBBrowserWindow.hh"

#include "HDBManager.hh"
#include "ADFSerializer.hh"
#include "Machine.hh"
#include "ErrorDialog.hh"
#include "FUArchitecture.hh"
#include "FUArchitectureDialog.hh"

/**
 * The Constructor.
 */
AddFUArchFromADFCmd::AddFUArchFromADFCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_ADD_FU_FROM_ADF, NULL) {
}


/**
 * The Destructor.
 */
AddFUArchFromADFCmd::~AddFUArchFromADFCmd() {
}

/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed.
 */
bool
AddFUArchFromADFCmd::Do() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }
    
    wxString wildcard = _T("Architecture Definition Files (*.adf)|*.adf");
    wildcard.Append(_T("|All files|*.*"));

    wxFileDialog dialog(
	parentWindow(), _T("Choose a file."), _T(""), _T(""),
	wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_CANCEL) {
	return false;
    }

    wxString fileName = dialog.GetPath();

    ADFSerializer reader;
    reader.setSourceFile(WxConversion::toString(fileName));
    TTAMachine::Machine* machine = NULL;

    try {
        machine = reader.readMachine();
    } catch (Exception& e) {
        wxString message = _T("Error opening ");
        message.Append(fileName);
        message.Append(_T(":\n\n"));
        message.Append(WxConversion::toWxString(e.errorMessage()));
        ErrorDialog dialog(parentWindow(), message);
        dialog.ShowModal();
        return false;
    }

    const TTAMachine::Machine::FunctionUnitNavigator& nav =
        machine->functionUnitNavigator();

    if (nav.count() == 0) {
        wxString message = _T("The file ");
        message.Append(fileName);
        message.Append(_T(" doesn't contain any function units."));
        ErrorDialog dialog(parentWindow(), message);
        dialog.ShowModal();
        delete machine;
        return false;
    }

    wxString fus[1000];

    for (int i = 0; i < nav.count(); i++) {
        fus[i] = WxConversion::toWxString(nav.item(i)->name());
    }

    wxSingleChoiceDialog choicer(
        parentWindow(), _T("Choose Function Unit"), _T("Choose Function Unit"),
        nav.count(), fus);

    if (choicer.ShowModal() != wxID_OK) {
        delete machine;
        return false;
    }


    TTAMachine::FunctionUnit* fu = nav.item(choicer.GetSelection());
    fu->unsetMachine();
    delete machine;

    HDB::FUArchitecture fuArch(fu);

    FUArchitectureDialog paramDialog(parentWindow(), -1 , fuArch);

    if (paramDialog.ShowModal() != wxID_OK) {
        return false;
    }

    int id = -1;
    try {
        id = manager->addFUArchitecture(fuArch);
    } catch (Exception& e) {
        wxString message = _T("Erroneous function unit architecture:\n\n");
        message.Append(WxConversion::toWxString(e.errorMessage()));
        ErrorDialog dialog(parentWindow(), message);
        dialog.ShowModal();
        return false;
    }

    wxGetApp().mainFrame().update();
    wxGetApp().mainFrame().browser()->selectFUArchitecture(id);

    return true;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
AddFUArchFromADFCmd::icon() const {
    return "";
}

/**
 * Returns the command id.
 *
 * @return Command identifier.
 */
int
AddFUArchFromADFCmd::id() const {
    return HDBEditorConstants::COMMAND_ADD_FU_FROM_ADF;
}


/**
 * Creates a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddFUArchFromADFCmd*
AddFUArchFromADFCmd::create() const {
    return new AddFUArchFromADFCmd();
}


/**
 * Returns true if the command should be enabled in the menu/toolbar.
 *
 * @return True, if the command is enabled, false if not.
 */
bool
AddFUArchFromADFCmd::isEnabled() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    return true;
}

