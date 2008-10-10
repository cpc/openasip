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
 * @file EditTemplatesCmd.cc
 *
 * Definition of EditTemplatesCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <vector>
#include <wx/docview.h>

#include "EditTemplatesCmd.hh"
#include "ProDeConstants.hh"
#include "TemplateListDialog.hh"
#include "ProDe.hh"
#include "Model.hh"
#include "MDFDocument.hh"
#include "Machine.hh"
#include "ModelConstants.hh"

using std::string;
using std::vector;


/**
 * The Constructor.
 */
EditTemplatesCmd::EditTemplatesCmd():
    EditorCommand(ProDeConstants::CMD_NAME_EDIT_TEMPLATES) {
}


/**
 * The Destructor.
 */
EditTemplatesCmd::~EditTemplatesCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
EditTemplatesCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();

    model->pushToStack();

    TemplateListDialog dialog(parentWindow(), model->getMachine());

    if (dialog.ShowModal() == wxID_OK) {
	model->notifyObservers();
        return true;
    } else {
        // Cancel button was pressed, templates are not modified.
	model->popFromStack();
        return false;
    }
}


/**
 * Returns id of this command.
 *
 * @return Id of this command.
 */
int
EditTemplatesCmd::id() const {
    return ProDeConstants::COMMAND_EDIT_TEMPLATES;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return A new instance of this command.
 */
EditTemplatesCmd*
EditTemplatesCmd::create() const {
    return new EditTemplatesCmd();
}



/**
 * Returns path to the command's icon file.
 *
 * @return path to the command's icon file.
 */
string
EditTemplatesCmd::icon() const {
    return ProDeConstants::CMD_ICON_EDIT_TEMPLATES;
}


/**
 * Returns short version of the command name.
 *
 * @return Short version of the command name.
 */
string
EditTemplatesCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_EDIT_TEMPLATES;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
EditTemplatesCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
