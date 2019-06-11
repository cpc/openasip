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
 * @file ProximAboutCmd.cc
 *
 * Implementation of ProximAboutCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProximAboutCmd.hh"
#include "ProximConstants.hh"
#include "ProximAboutDialog.hh"


/**
 * The Constructor.
 */
ProximAboutCmd::ProximAboutCmd():
    GUICommand(ProximConstants::COMMAND_NAME_ABOUT, NULL) {

}

/**
 * The Destructor.
 */
ProximAboutCmd::~ProximAboutCmd() {
}


/**
 * Executes the command.
 */
bool
ProximAboutCmd::Do() {
    assert(parentWindow() != NULL);
    ProximAboutDialog dialog(parentWindow());
    dialog.ShowModal();
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximAboutCmd::icon() const {
    return "about.png";
}


/**
 * Returns ID of this command.
 */
int
ProximAboutCmd::id() const {
    return ProximConstants::COMMAND_ABOUT;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximAboutCmd*
ProximAboutCmd::create() const {
    return new ProximAboutCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximAboutCmd::isEnabled() {
    return true;
}
