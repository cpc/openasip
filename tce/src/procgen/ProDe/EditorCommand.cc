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
 * @file EditorCommand.cc
 *
 * Definition of EditorCommand class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <string>

#include "EditorCommand.hh"
#include "ProDeConstants.hh"

using std::string;

/**
 * The Constructor.
 */
EditorCommand::EditorCommand(std::string name, wxWindow* parent) :
    GUICommand(name, parent),
    view_(NULL) {

}



/**
 * The Destructor.
 */
EditorCommand::~EditorCommand() {
}




/**
 * Sets the MDFView of the command.
 *
 * @param view MDFView to be assigned for the command.
 */
void
EditorCommand::setView(wxView* view) {
    view_ = view;
}


/**
 * Returns MDFView of the command.
 *
 * @return MDFView of the command.
 */
wxView*
EditorCommand::view() const {
    return view_;
}


/**
 * Returns path to the icon of the command to be used on the toolbar.
 *
 * @return Base class implementation returns default icon's path.
 */
string
EditorCommand::icon() const {
    return ProDeConstants::CMD_ICON_DEFAULT;
}


/**
 * Returns true if command is currently executable, otherwise false.
 *
 * Base class implementation returns always false.
 *
 * @return True if command is currently executable, otherwise false.
 */
bool
EditorCommand::isEnabled() {
    return false;
}
