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
 * @file ModifyFUPortCmd.cc
 *
 * Definition of ModifyFUPortCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "ModifyFUPortCmd.hh"
#include "ModelConstants.hh"
#include "EditPart.hh"
#include "FUPortDialog.hh"
#include "FUPort.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Port to modify.
 */
ModifyFUPortCmd::ModifyFUPortCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {
}


/**
 * The Destructor.
 */
ModifyFUPortCmd::~ModifyFUPortCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifyFUPortCmd::Do() {

    FUPort* port = dynamic_cast<FUPort*>(editPart_->model());

    assert (port != NULL);
    assert (parentWindow() != NULL);

    FUPortDialog dialog(parentWindow(), port);

    if (dialog.ShowModal() == wxID_OK) {
	// port was modified
	return true;
    } else {
	return false;
    }
}
