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
 * @file DeleteBridgeCmd.cc
 *
 * Definition of DeleteBridgeCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 * @note reviewed Jun 23 2004 by ml, jn, jm, vpj
 */

#include "Application.hh"
#include "DeleteBridgeCmd.hh"
#include "Bridge.hh"
#include "EditPart.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Bridge to delete.
 */
DeleteBridgeCmd::DeleteBridgeCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {
}


/**
 * The Destructor.
 */
DeleteBridgeCmd::~DeleteBridgeCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
DeleteBridgeCmd::Do() {

    if (editPart_->childCount() == 0) {
        Bridge* bridge = dynamic_cast<Bridge*>(editPart_->model());
        assert (bridge != NULL);
	delete bridge;
        bridge = NULL;
    } else {
        // bidirectional bridge
        assert(editPart_->childCount() == 2);
        // delete bridges of both direction
        for (int i = 0; i < editPart_->childCount(); i++) {
            Bridge* bridge =
                dynamic_cast<Bridge*>(editPart_->child(i)->model());
            assert (bridge != NULL);
	    delete bridge;
            bridge = NULL;
        }
    }

    return true;
}
