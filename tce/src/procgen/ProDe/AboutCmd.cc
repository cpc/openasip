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
 * @file AboutCmd.cc
 *
 * Definition of AboutCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 */

#include "AboutCmd.hh"
#include "ProDeConstants.hh"
#include "AboutDialog.hh"

using std::string;

/**
 * The Constructor.
 */
AboutCmd::AboutCmd():
    EditorCommand(ProDeConstants::CMD_NAME_ABOUT) {

}


/**
 * The Destructor.
 */
AboutCmd::~AboutCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
AboutCmd::Do() {
    AboutDialog about(parentWindow());
    about.ShowModal();
    return true;
}


/**
 * Returns id of this command.
 */
int
AboutCmd::id() const {
    return ProDeConstants::COMMAND_ABOUT;
}


/**
 * Creates and returns a new instance of this command.
 */
AboutCmd*
AboutCmd::create() const {
    return new AboutCmd();
}


/**
 * This command is always executable.
 *
 * @return Always true.
 */
bool
AboutCmd::isEnabled() {
    return true;
}


/**
 * Returns path to the command's icon file.
 */
string
AboutCmd::icon() const {
    return ProDeConstants::CMD_ICON_ABOUT;
}

