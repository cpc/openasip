/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file CallExplorerPluginCmd.cc
 *
 * Definition of CallExplorerPluginCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/docview.h>

#include "ProDe.hh"
#include "CallExplorerPluginCmd.hh"
#include "CallExplorerPlugin.hh"
#include "ProDeConstants.hh"
#include "MDFDocument.hh"

using std::string;

/**
 * The Constructor.
 */
CallExplorerPluginCmd::CallExplorerPluginCmd():
    EditorCommand(ProDeConstants::CMD_CALL_EXPLORER_PLUGIN) {

}


/**
 * The Destructor.
 */
CallExplorerPluginCmd::~CallExplorerPluginCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
CallExplorerPluginCmd::Do() {

    TTAMachine::Machine* machine = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel()->getMachine();
    
    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();

    model->pushToStack();

    CallExplorerPluginWindow dialog(parentWindow(), *machine, *model);

    if (dialog.ShowModal() == wxID_OK) {
        model->notifyObservers();
        return true;
    } else {
        model->popFromStack();
        return false;
    }

    return true;
}


/**
 * Returns id of this command.
 */
int
CallExplorerPluginCmd::id() const {
    return ProDeConstants::COMMAND_CALL_EXPLORER_PLUGIN;
}


/**
 * Creates and returns a new instance of this command.
 */
CallExplorerPluginCmd*
CallExplorerPluginCmd::create() const {
    return new CallExplorerPluginCmd();
}



/**
 * Returns path to the command's icon file.
 */
string
CallExplorerPluginCmd::icon() const {
    return ProDeConstants::CMD_ICON_EXPLORER_PLUGIN;
}


/**
 * Returns short version of the command name.
 */
string
CallExplorerPluginCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_EXPLORER_PLUGIN;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
CallExplorerPluginCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
    return true;
    }
    return false;
}
