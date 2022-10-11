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
 * @file ProximOptionsCmd.cc
 * 
 * Implementation of ProximOptionsCmd class.
 * 
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProximOptionsCmd.hh"
#include "OptionsDialog.hh"
#include "ProximConstants.hh"
#include "GUIOptions.hh"
#include "Proxim.hh"
#include "ProximMainFrame.hh"
#include "ProximToolbox.hh"

using std::string;

/**
 * The Constructor.
 */
ProximOptionsCmd::ProximOptionsCmd():
    GUICommand(ProximConstants::COMMAND_NAME_EDIT_OPTIONS, NULL) {

}


/**
 * The Destructor.
 */
ProximOptionsCmd::~ProximOptionsCmd() {
}


/**
 * Executes the command.
 * 
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
ProximOptionsCmd::Do() {

    OptionsDialog dialog(
        parentWindow(), wxGetApp().options(), wxGetApp().commandRegistry());

    if (dialog.ShowModal() == wxID_OK) {
        if (wxGetApp().options().toolbarVisibility()) {
            ProximToolbox::mainFrame()->createToolbar();
        }
        ProximToolbox::mainFrame()->createMenubar();
    }
    return true;
}


/**
 * Returns id of this command.
 * 
 * @return ID for this command to be used in menus and toolbars.
 */
int
ProximOptionsCmd::id() const {
    return ProximConstants::COMMAND_EDIT_OPTIONS;
}
		  
		  
/**
 * Creates and returns a new instance of this command.
 * 
 * @return Newly created instance of this command.
 */
ProximOptionsCmd*
ProximOptionsCmd::create() const {
    return new ProximOptionsCmd();
}


/**
 * Returns path to the command's icon file.
 * 
 * @return Full path to the command's icon file.
 */
string
ProximOptionsCmd::icon() const {
    return "options.png";
}


/**
 * This command is always executable.
 * 
 * @return Always true.
 */
bool
ProximOptionsCmd::isEnabled() {
    return true;
}
