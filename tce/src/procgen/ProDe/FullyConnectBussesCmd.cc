/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file FullyConnectBussesCmd.cc
 *
 * Definition of FullyConnectBussesCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */


#include <wx/docview.h>

#include "FullyConnectBussesCmd.hh"
#include "ProDeConstants.hh"
#include "Model.hh"
#include "MDFDocument.hh"
#include "Machine.hh"
#include "MachineTester.hh"
#include "ProDe.hh"
#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "Socket.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"
#include "FullyConnectedCheck.hh"
#include "MachineCheckResults.hh"


using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 */
FullyConnectBussesCmd::FullyConnectBussesCmd():
    EditorCommand(ProDeConstants::CMD_NAME_FULLY_CONNECT_BUSSES) {
}


/**
 * The Destructor.
 */
FullyConnectBussesCmd::~FullyConnectBussesCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
FullyConnectBussesCmd::Do() {

    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();

    model->pushToStack();
    
    Machine& machine = *model->getMachine();

    FullyConnectedCheck checker;
    checker.fix(machine);

    model->notifyObservers();
    return true;
}

/**
 * Returns id of this command.
 */
int
FullyConnectBussesCmd::id() const {
    return ProDeConstants::COMMAND_FULLY_CONNECT_BUSSES;
}


/**
 * Creates and returns a new instance of this command.
 */
FullyConnectBussesCmd*
FullyConnectBussesCmd::create() const {
    return new FullyConnectBussesCmd();
}


/**
 * Returns short version of the command name.
 */
string
FullyConnectBussesCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_FULLY_CONNECT_BUSSES;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
FullyConnectBussesCmd::isEnabled() {

    wxDocManager* manager = wxGetApp().docManager();
    wxView* view = manager->GetCurrentView();

    if (view == NULL) {
	return false;
    }

    Model* model = dynamic_cast<MDFDocument*>(
        view->GetDocument())->getModel();

    if (model == NULL) {
        return false;
    }

    FullyConnectedCheck checker;
    MachineCheckResults results;
    if (!checker.check(*model->getMachine(), results)) {
        return checker.canFix(*model->getMachine());
    } else {
        return false;
    }
}
