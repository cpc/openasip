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
 * @file HDBEditorAboutCmd.cc
 *
 * Implementation of HDBEditorAboutCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "HDBEditorAboutCmd.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditorAboutDialog.hh"


/**
 * The Constructor.
 */
HDBEditorAboutCmd::HDBEditorAboutCmd():
    GUICommand(HDBEditorConstants::COMMAND_NAME_ABOUT, NULL) {

}

/**
 * The Destructor.
 */
HDBEditorAboutCmd::~HDBEditorAboutCmd() {
}


/**
 * Executes the command.
 */
bool
HDBEditorAboutCmd::Do() {
    assert(parentWindow() != NULL);
    HDBEditorAboutDialog dialog(parentWindow());
    dialog.ShowModal();
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
HDBEditorAboutCmd::icon() const {
    return "about.png";
}


/**
 * Returns ID of this command.
 */
int
HDBEditorAboutCmd::id() const {
    return HDBEditorConstants::COMMAND_ABOUT;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
HDBEditorAboutCmd*
HDBEditorAboutCmd::create() const {
    return new HDBEditorAboutCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
HDBEditorAboutCmd::isEnabled() {
    return true;
}
