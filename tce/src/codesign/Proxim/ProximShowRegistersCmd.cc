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
 * @file ProximShowRegistersCmd.cc
 *
 * Implementation of ProximShowRegistersCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProximShowRegistersCmd.hh"
#include "EditPart.hh"
#include "ProximRegisterWindow.hh"
#include "MachinePart.hh"
#include "ProximToolbox.hh"

/**
 * The Constructor.
 */
ProximShowRegistersCmd::ProximShowRegistersCmd(
    EditPart* part) :
    ComponentCommand(),
    part_(part) {

}


/**
 * The Destructor.
 */
ProximShowRegistersCmd::~ProximShowRegistersCmd() {
}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 *
 */
bool
ProximShowRegistersCmd::Do() {

    std::string rfName =
        dynamic_cast<TTAMachine::Component*>(part_->model())->name();

    ProximRegisterWindow* regWindow = new ProximRegisterWindow(
        ProximToolbox::mainFrame(), -1);

    ProximToolbox::addFramedWindow(
        regWindow, _T("Registers"), false, wxSize(270, 200));

    regWindow->GetParent()->SetSize(270, 300);

    regWindow->showRegisterFile(rfName);

    return true;
}
