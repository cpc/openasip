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
 * @file AddBridgeCmd.cc
 *
 * Definition of AddBridgeCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 * @note reviewed Jun 23 2004 by ml, jn, jm, vpj
 */

#include <wx/wx.h>
#include <wx/docview.h>
#include <boost/format.hpp>

#include "AddBridgeCmd.hh"
#include "BridgeDialog.hh"
#include "Machine.hh"
#include "Model.hh"
#include "Conversion.hh"
#include "MDFDocument.hh"
#include "ProDeConstants.hh"
#include "InformationDialog.hh"
#include "Bus.hh"
#include "Bridge.hh"
#include "Segment.hh"
#include "ProDe.hh"
#include "MainFrame.hh"
#include "MachineTester.hh"
#include "ProDeTextGenerator.hh"
#include "WxConversion.hh"

using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 */
AddBridgeCmd::AddBridgeCmd() :
    EditorCommand(ProDeConstants::CMD_NAME_ADD_BRIDGE) {
}


/**
 * The Destructor.
 */
AddBridgeCmd::~AddBridgeCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
AddBridgeCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model =
        dynamic_cast<MDFDocument*>(view()->GetDocument())->getModel();
    Machine* machine = model->getMachine();

    Machine::BusNavigator navigator = machine->busNavigator();

    Bus* source = NULL;
    Bus* destination = NULL;
    MachineTester tester(*machine);

    // Check that two buses in the machine can be bridged.
    int i = 0;
    while (i < navigator.count() && source == NULL) {
	for (int j = 0; j < navigator.count(); j++) {
	    if (tester.canBridge(*navigator.item(i), *navigator.item(j))) {
                source = navigator.item(i);
                destination = navigator.item(j);
                break;
            }
        }
        i++;
    }

    if (source == NULL) {
        // It's not possible to create a legal bridge to the machine,
        // display an error message.
        ProDeTextGenerator* generator = ProDeTextGenerator::instance();

        boost::format message =
            generator->text(ProDeTextGenerator::MSG_ERROR_CANNOT_BRIDGE);

        InformationDialog info(parentWindow(),
                               WxConversion::toWxString(message.str()));
        info.ShowModal();
        return false;
    }


    // Generate name for the new FU.
    Machine::BridgeNavigator bridgeNavigator =
        model->getMachine()->bridgeNavigator();
    int suffix = 1;
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_BRIDGE +
        Conversion::toString(suffix);
    while (bridgeNavigator.hasItem(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_BRIDGE +
            Conversion::toString(suffix);
        suffix++;
    }

    // Create and show bridge dialog.
    model->pushToStack();
    Bridge* bridge = new Bridge(newName, *source, *destination);
    BridgeDialog dialog(parentWindow(), bridge, NULL);

    if (dialog.ShowModal() == wxID_OK) {
	model->notifyObservers();
    } else {
	// bridge creation was cancelled
	model->popFromStack();
    }
    return false;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
AddBridgeCmd::id() const {
    return ProDeConstants::COMMAND_ADD_BRIDGE;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddBridgeCmd*
AddBridgeCmd::create() const {
    return new AddBridgeCmd();
}



/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
AddBridgeCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ADD_BRIDGE;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
AddBridgeCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
