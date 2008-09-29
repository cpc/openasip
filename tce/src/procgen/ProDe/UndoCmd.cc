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
 * @file UndoCmd.cc
 *
 * Definition of UndoCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
 */

#include <wx/docview.h>
#include "UndoCmd.hh"
#include "ProDe.hh"
#include "ProDeConstants.hh"
#include "MDFDocument.hh"


using std::string;

/**
 * The Constructor.
 */
UndoCmd::UndoCmd():
    EditorCommand(ProDeConstants::CMD_NAME_UNDO) {

}


/**
 * The Destructor.
 */
UndoCmd::~UndoCmd() {}


/**
 * Executes the command.
 *
 * @return Always false.
 */
bool
UndoCmd::Do() {
    dynamic_cast<MDFDocument*>(view()->GetDocument())->getModel()->undo();
    return false;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
UndoCmd::id() const {
    return ProDeConstants::COMMAND_UNDO;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
UndoCmd*
UndoCmd::create() const {
    return new UndoCmd();
}



/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
UndoCmd::icon() const {
    return ProDeConstants::CMD_ICON_UNDO;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when the model's undo stack is not empty.
 *
 * @return True, if the model's undo stack is not empty.
 */
bool
UndoCmd::isEnabled() {

    wxDocManager* manager = wxGetApp().docManager();

    wxView* view = manager->GetCurrentView();
    if (view == NULL) {
        return false;
    }

    Model* model =
        dynamic_cast<MDFDocument*>(view->GetDocument())->getModel();

    if (model != NULL && model->canUndo()) {
        return true;
    }
    return false;
}

