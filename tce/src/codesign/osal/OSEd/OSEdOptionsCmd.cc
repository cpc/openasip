/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file OSEdOptionsCmd.cc 
 *
 * Definition of OSEdOptionsCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "OSEdOptionsCmd.hh"
#include "OSEdConstants.hh"
#include "OSEdOptionsDialog.hh"

using std::string;

/**
 * Constructor.
 */
OSEdOptionsCmd::OSEdOptionsCmd() : 
	GUICommand(OSEdConstants::CMD_NAME_OPTIONS, NULL) {
}

/**
 * Destructor.
 */
OSEdOptionsCmd::~OSEdOptionsCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdOptionsCmd::id() const {
	return OSEdConstants::CMD_OPTIONS;
}

/**
 * Creates a new command.
 *
 * @return New command.
 */
GUICommand*
OSEdOptionsCmd::create() const {
	return new OSEdOptionsCmd();
}

/**
 * Executes the command.
 *
 * @return True if execution is successful.
 */
bool
OSEdOptionsCmd::Do() {
	OSEdOptionsDialog dialog(parentWindow());
	dialog.ShowModal();
	return true;
}

/**
 * Return true if command is enabled.
 *
 * @return Always true.
 */
bool
OSEdOptionsCmd::isEnabled() {
	return true;
}

/**
 * Return icon path.
 *
 * @return Empty string (icons not used).
 */
string
OSEdOptionsCmd::icon() const {
	return "";
}
