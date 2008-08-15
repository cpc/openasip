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
 * @file PasteComponentCmd.cc
 *
 * Implementation of PasteComponentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include <wx/docview.h>
#include <boost/format.hpp>

#include "Application.hh"
#include "PasteComponentCmd.hh"
#include "MDFView.hh"
#include "MDFDocument.hh"
#include "ProDeConstants.hh"
#include "ProDeClipboard.hh"
#include "Machine.hh"
#include "Conversion.hh"
#include "WxConversion.hh"
#include "ProDeTextGenerator.hh"
#include "InformationDialog.hh"
#include "ObjectState.hh"

#include "Socket.hh"
#include "FunctionUnit.hh"
#include "Bus.hh"
#include "RegisterFile.hh"
#include "ImmediateUnit.hh"
#include "ControlUnit.hh"

using std::string;
using boost::format;
using namespace TTAMachine;

/**
 * The Constructor.
 */
PasteComponentCmd::PasteComponentCmd():
    EditorCommand(ProDeConstants::CMD_NAME_PASTE) {
}



/**
 * Executes the command.
 *
 * Copies descriptor of a component from the clipboard and adds
 * corresponding component to the machine
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
PasteComponentCmd::Do() {

    ProDeClipboard* clipboard = ProDeClipboard::instance();
    ObjectState* contents = clipboard->copyContents();

    MDFDocument* document =
	dynamic_cast<MDFDocument*>(view()->GetDocument());
    assert(document != NULL);
    Machine* machine = document->getModel()->getMachine();
    assert(machine != NULL);

    document->getModel()->pushToStack();

    if (contents->name() == Socket::OSNAME_SOCKET) {
        // socket
        Machine::SocketNavigator navigator = machine->socketNavigator();
        Socket* socket = new Socket(contents);
        paste(*machine, socket, navigator);
        contents->setAttribute(Socket::OSKEY_NAME, socket->name());
        // Socket can be pasted only if the target machine has busses with
        // the same names as the busses where the copied socket was connected
        // to.
        try {
            socket->loadState(contents);
        } catch (ObjectStateLoadingException& e) {
            wxString message =
                _T("The socket cannot be pasted to this machine.\n\n");
            message.Append(WxConversion::toWxString(e.errorMessage()));
            InformationDialog dialog(parentWindow(), message);
            dialog.ShowModal();
            delete socket;
            return false;
        }


    } else if (contents->name() == FunctionUnit::OSNAME_FU) {
        // function unit
        Machine::FunctionUnitNavigator navigator =
            machine->functionUnitNavigator();
        paste(*machine, new FunctionUnit(contents), navigator);

    } else if (contents->name() == ImmediateUnit::OSNAME_IMMEDIATE_UNIT) {
        // immediate unit
        Machine::ImmediateUnitNavigator navigator =
            machine->immediateUnitNavigator();
        paste(*machine, new ImmediateUnit(contents), navigator);

    } else if (contents->name() == RegisterFile::OSNAME_REGISTER_FILE) {
        // register file
        Machine::RegisterFileNavigator navigator =
            machine->registerFileNavigator();
        paste(*machine, new RegisterFile(contents), navigator);

    } else if (contents->name() == Bus::OSNAME_BUS) {
        // register file
        Machine::BusNavigator navigator =
            machine->busNavigator();
        paste(*machine, new Bus(contents), navigator);

    } else if (contents->name() == ControlUnit::OSNAME_CONTROL_UNIT) {
        // control unit
        if (machine->controlUnit() != NULL) {
            // Target machine already contained a control unit.
            // Display error message and pop the machine from undo stack.
            ProDeTextGenerator* generator = ProDeTextGenerator::instance();
            format fmt =
                generator->text(ProDeTextGenerator::MSG_ERROR_ONE_GCU);
            string title = fmt.str();
            wxString message = WxConversion::toWxString(title);
            InformationDialog info(parentWindow(), message);
            info.ShowModal();
            document->getModel()->popFromStack();
            delete contents;
            return false;
        } else {
            // Paste control unit.
            ControlUnit* gcu = new ControlUnit(contents);
            gcu->setMachine(*machine);
        }
    } else {
        // Unknown component type.
        assert(false);
    }

    delete contents;

    document->getModel()->notifyObservers();

    return false;
}


/**
 * Returns command identifier of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
PasteComponentCmd::id() const {
    return ProDeConstants::COMMAND_PASTE;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
PasteComponentCmd*
PasteComponentCmd::create() const {
    return new PasteComponentCmd();
}



/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
PasteComponentCmd::icon() const {
    return ProDeConstants::CMD_ICON_PASTE;
}



/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open and the
 * clipboard is not empty.
 *
 * @return True, if the command is executable.
 */
bool
PasteComponentCmd::isEnabled() {

    // check that a document is open
    wxDocManager* manager = wxGetApp().docManager();
    MDFView* mdfView = dynamic_cast<MDFView*>(manager->GetCurrentView());
    if (mdfView == NULL) {
	return false;
    }

    // check that the clipboard is not empty
    ProDeClipboard* clipboard = ProDeClipboard::instance();
    if (clipboard->isEmpty()) {
	return false;
    }
    return true;
}
