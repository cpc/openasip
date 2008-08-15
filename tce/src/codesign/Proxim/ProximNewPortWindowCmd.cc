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
 * @file ProximNewPortWindowCmd.cc
 *
 * Implementation of ProximNewPortWindowCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximNewPortWindowCmd.hh"
#include "ProximConstants.hh"
#include "ProximPortWindow.hh"
#include "ProximToolbox.hh"

/**
 * The Constructor.
 */
ProximNewPortWindowCmd::ProximNewPortWindowCmd():
    GUICommand(ProximConstants::COMMAND_NAME_NEW_PORT_WINDOW, NULL) {

    }

/**
 * The Destructor.
 */
ProximNewPortWindowCmd::~ProximNewPortWindowCmd() {
}


/**
 * Executes the command.
 */
bool
ProximNewPortWindowCmd::Do() {

    ProximPortWindow* portWindow =
        new ProximPortWindow(ProximToolbox::mainFrame(), -1);

    ProximToolbox::addFramedWindow(
        portWindow, _T("Ports"), false, wxSize(270, 200));

    portWindow->GetParent()->SetSize(270, 300);

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximNewPortWindowCmd::icon() const {
    return "window_new.png";
}


/**
 * Returns ID of this command.
 */
int
ProximNewPortWindowCmd::id() const {
    return ProximConstants::COMMAND_NEW_PORT_WINDOW;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximNewPortWindowCmd*
ProximNewPortWindowCmd::create() const {
    return new ProximNewPortWindowCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximNewPortWindowCmd::isEnabled() {
    return true;
}
