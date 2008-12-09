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
