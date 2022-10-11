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
 * @file ProximCDCmd.cc
 *
 * Implementation of ProximCDCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProximCDCmd.hh"
#include "WxConversion.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "FileSystem.hh"
#include "ProximSimulationThread.hh"

/**
 * The Constructor.
 */
ProximCDCmd::ProximCDCmd():
    GUICommand("Change Directory", NULL) {

    }

/**
 * The Destructor.
 */
ProximCDCmd::~ProximCDCmd() {
}


/**
 * Executes the command.
 */
bool
ProximCDCmd::Do() {

    assert(parentWindow() != NULL);
    wxDirDialog dialog(parentWindow(), _T("Choose a directory"), wxGetCwd());

    if (dialog.ShowModal() == wxID_CANCEL) {
	return false;
    }

    std::string command = 
        "cd \"" + WxConversion::toString(dialog.GetPath()) + "\"";

    wxGetApp().simulation()->lineReader().input(command);
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximCDCmd::icon() const {
    return "cd.png";
}


/**
 * Returns ID of this command.
 */
int
ProximCDCmd::id() const {
    return ProximConstants::COMMAND_CD;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximCDCmd*
ProximCDCmd::create() const {
    return new ProximCDCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximCDCmd::isEnabled() {
    return true;
}
