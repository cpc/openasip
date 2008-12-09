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
 * @file RedoCmd.cc
 *
 * Definition of RedoCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
 */

#include <wx/docview.h>
#include "RedoCmd.hh"
#include "ProDe.hh"
#include "ProDeConstants.hh"
#include "MDFDocument.hh"


using std::string;

/**
 * The Constructor.
 */
RedoCmd::RedoCmd():
    EditorCommand(ProDeConstants::CMD_NAME_REDO) {

}


/**
 * The Destructor.
 */
RedoCmd::~RedoCmd() {}


/**
 * Executes the command.
 *
 * @return Always false.
 */
bool
RedoCmd::Do() {
    dynamic_cast<MDFDocument*>(view()->GetDocument())->getModel()->redo();
    return false;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
RedoCmd::id() const {
    return ProDeConstants::COMMAND_REDO;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
RedoCmd*
RedoCmd::create() const {
    return new RedoCmd();
}



/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
RedoCmd::icon() const {
    return ProDeConstants::CMD_ICON_REDO;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when the model has an undone modification
 * cached.
 *
 * @return True, if the model's redo cache is not empty.
 */
bool
RedoCmd::isEnabled() {

    wxDocManager* manager = wxGetApp().docManager();

    wxView* view = manager->GetCurrentView();
    if (view == NULL) {
        return false;
    }

    Model* model =
        dynamic_cast<MDFDocument*>(view->GetDocument())->getModel();

    if (model != NULL && model->canRedo()) {
	return true;
    }
    return false;
}

