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
 * @file ModifyBridgeCmd.cc
 *
 * Definition of ModifyBridgeCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 * @note reviewed Jun 23 2004 by ml, jn, jm, vpj
 */

#include <string>

#include "Application.hh"
#include "ModifyBridgeCmd.hh"
#include "Bridge.hh"
#include "EditPart.hh"
#include "BridgeDialog.hh"
#include "ErrorDialog.hh"

using std::string;
using namespace TTAMachine;


/**
 * The Constructor.
 *
 * @param editPart EditPart of the component to modify.
 */
ModifyBridgeCmd::ModifyBridgeCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {

}


/**
 * The Destructor.
 */
ModifyBridgeCmd::~ModifyBridgeCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifyBridgeCmd::Do() {

    Bridge* bridge = NULL;
    Bridge* opposite = NULL;

    if (editPart_->childCount() == 0) {
        bridge = dynamic_cast<Bridge*>(editPart_->model());
	editPart_->setModel(NULL);
        assert (bridge != NULL);
    } else {
        // bidirectional bridge
        assert(editPart_->childCount() == 2);
        // find bridges of both direction
        bridge = dynamic_cast<Bridge*>(editPart_->child(0)->model());
        opposite = dynamic_cast<Bridge*>(editPart_->child(1)->model());
	editPart_->setModel(NULL);
    }

    assert(opposite != bridge);

    BridgeDialog dialog(parentWindow(), bridge, opposite);

    if (dialog.ShowModal() == wxID_OK) {
	return true;
    } else {
	// modification was cancelled
	return false;
    }

    return false;
}
