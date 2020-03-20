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
 * @file GUICommand.cc
 *
 * Definition of GUICommand class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>

#include "GUICommand.hh"

using std::string;

/**
 * The Constructor.
 */
GUICommand::GUICommand(std::string name, wxWindow* parent = NULL) :
    parent_(parent), name_(name) {

}



/**
 * The Destructor.
 */
GUICommand::~GUICommand() {
}


/**
 * Sets the parent window of the command.
 *
 * Window will be used as parent for the dialogs created by this command.
 *
 * @param parent Parent window for the command dialogs.
 */
void
GUICommand::setParentWindow(wxWindow* parent) {
    parent_ = parent;
}


/**
 * Returns parent window of the command.
 *
 * @return Parent window of the command.
 */
wxWindow*
GUICommand::parentWindow() const {
    return parent_;
}


/**
 * Returns short version of the command name.
 *
 * Base class implementation returns the command normal name.
 *
 * @return Short version of the command name.
 */
std::string
GUICommand::shortName() const {
    return name_;
}


/**
 * Returns name of the command.
 *
 * @return Long name of the command.
 */
std::string
GUICommand::name() const {
    return name_;
}


/**
 * Returns true if a checkbox related to the command is checked.
 *
 * This is meaningful only for toggle-type commands.
 *
 * @return True, if the command feature is toggled on.
 */
bool
GUICommand::isChecked() const {
    return false;
}
