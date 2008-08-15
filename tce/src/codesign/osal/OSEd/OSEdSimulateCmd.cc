/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
