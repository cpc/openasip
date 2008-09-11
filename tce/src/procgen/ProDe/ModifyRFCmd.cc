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
 * @file ModifyRFCmd.cc
 *
 * Definition of ModifyRFCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include "Application.hh"
#include "ModifyRFCmd.hh"
#include "RegisterFile.hh"
#include "EditPart.hh"
#include "Model.hh"
#include "RFDialog.hh"
#include "ErrorDialog.hh"
#include "ProDe.hh"
#include "MDFDocument.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
ModifyRFCmd::ModifyRFCmd(EditPart* editPart):
    editPart_(editPart) {

}


/**
 * The Destructor.
 */
ModifyRFCmd::~ModifyRFCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifyRFCmd::Do() {

    RegisterFile* rf = dynamic_cast<RegisterFile*>(editPart_->model());
    assert (rf != NULL);
    assert (parentWindow() != NULL);

    RFDialog dialog(parentWindow(), rf);

    if (dialog.ShowModal() == wxID_OK) {
	// register file was modified
	return true;
    } else {
	// modification was cancelled
	return false;
    }

    return false;
}
