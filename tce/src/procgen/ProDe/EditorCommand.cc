/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file EditorCommand.cc
 *
 * Definition of EditorCommand class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
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
