/**
 * @file OSEdSimulateCmd.cc
 *
 * Declaration of OSEdSimulateCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include "OSEdSimulateCmd.hh"
#include "OperationContainer.hh"
#include "OSEd.hh"
#include "ErrorDialog.hh"
#include "WxConversion.hh"
#include "OSEdConstants.hh"
#include "SimulateDialog.hh"
#include "OperationBehaviorLoader.hh"
#include "OperationModule.hh"
#include "OSEdTreeView.hh"
#include "Operation.hh"

using std::string;

/**
 * Constructor.
 */
OSEdSimulateCmd::OSEdSimulateCmd() :
    GUICommand(OSEdConstants::CMD_NAME_SIMULATE, NULL) {
}

/**
 * Destructor.
 */
OSEdSimulateCmd::~OSEdSimulateCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdSimulateCmd::id() const {
    return OSEdConstants::CMD_SIMULATE;
}

/**
 * Creates a new command.
 *
 * @return The created command.
 */
GUICommand*
OSEdSimulateCmd::create() const {
    return new OSEdSimulateCmd();
}

/**
 * Executes the command.
 *
 * @return Always true.
 */
bool
OSEdSimulateCmd::Do() {

    OSEdMainFrame* mainFrame = wxGetApp().mainFrame();
    OSEdTreeView* treeView = mainFrame->treeView();
    Operation* op = treeView->selectedOperation();
    wxTreeItemId opId = treeView->selectedOperationId();
    string modName = treeView->moduleOfOperation(opId);
    wxTreeItemId modId = treeView->moduleIdOfOperation(opId);
    string pathName = treeView->pathOfModule(modId);

    //OperationModule& module = OperationContainer::module(pathName, modName);

    SimulateDialog* dialog = 
        new SimulateDialog(parentWindow(), op, pathName, modName);
    dialog->Show();
    return true;
}

/**
 * Returns true if command is enabled.
 *
 * Command is enabled, if selected operation has behavior module.
 * 
 * @return True if command is enabled.
 */
bool
OSEdSimulateCmd::isEnabled() {
    OSEdTreeView* treeView = wxGetApp().mainFrame()->treeView();
    Operation* op = treeView->selectedOperation();

    if (treeView->isOperationSelected() &&  op != NULL) {
        return op->canBeSimulated();
    }
    return false;
}

/**
 * Return icon path.
 *
 * @return Empty string (icons not used).
 */
string
OSEdSimulateCmd::icon() const {
    return "";
}
