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
 * @file HDBEditorQuitCmd.cc
 *
 * Implementation of HDBEditorQuitCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "HDBEditorQuitCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"

/**
 * The Constructor.
 */
HDBEditorQuitCmd::HDBEditorQuitCmd():
    GUICommand(HDBEditorConstants::COMMAND_NAME_QUIT, NULL) {
}


/**
 * The Destructor.
 */
HDBEditorQuitCmd::~HDBEditorQuitCmd() {
}


/**
 * Executes the command.
 */
bool
HDBEditorQuitCmd::Do() {

    wxGetApp().mainFrame().Close();

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
HDBEditorQuitCmd::icon() const {
    return "quit.png";
}


/**
 * Returns ID of this command.
 */
int
HDBEditorQuitCmd::id() const {
    return HDBEditorConstants::COMMAND_QUIT;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
HDBEditorQuitCmd*
HDBEditorQuitCmd::create() const {
    return new HDBEditorQuitCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
HDBEditorQuitCmd::isEnabled() {
    return true;
}
