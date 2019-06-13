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
 * @file OSEdMemoryCmd.cc
 *
 * Definition of OSEdMemoryCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "OSEdMemoryCmd.hh"
#include "MemoryDialog.hh"
#include "OSEdConstants.hh"
#include "OSEdInformer.hh"
#include "OSEd.hh"

using std::string;

/**
 * Constructor.
 */
OSEdMemoryCmd::OSEdMemoryCmd() :
    GUICommand(OSEdConstants::CMD_NAME_MEMORY, NULL) {
}

/**
 * Destructor.
 */
OSEdMemoryCmd::~OSEdMemoryCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdMemoryCmd::id() const {
    return OSEdConstants::CMD_MEMORY;
}

/**
 * Creates a new command.
 *
 * @return The created command.
 */
GUICommand*
OSEdMemoryCmd::create() const {
    return new OSEdMemoryCmd();
}

/**
 * Executes the command.
 * 
 * @return Always true.
 */
bool
OSEdMemoryCmd::Do() {
    MemoryDialog* dialog = new MemoryDialog(parentWindow());
    dialog->Show();
    return true;
}

/**
 * Returns true if command is enabled.
 *
 * Memory command is always enabled.
 *
 * @return Always true.
 */
bool
OSEdMemoryCmd::isEnabled() {
    return true;
}

/**
 * Returns icon path.
 *
 * @return Empty string (icons not used).
 */
string
OSEdMemoryCmd::icon() const {
    return "";
}
