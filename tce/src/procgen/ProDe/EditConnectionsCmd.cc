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
 * @file EditConnectionsCmd.cc
 *
 * Definition of EditConnectionsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <wx/docview.h>

#include "Application.hh"
#include "EditConnectionsCmd.hh"
#include "ProDeConstants.hh"
#include "ConnectTool.hh"
#include "ProDe.hh"
#include "MDFView.hh"
#include "MachineCanvas.hh"

using std::string;

/**
 * The Constructor.
 */
EditConnectionsCmd::EditConnectionsCmd():
    EditorCommand(ProDeConstants::CMD_NAME_EDIT_CONNECTIONS) {

}


/**
 * The Destructor.
 */
EditConnectionsCmd::~EditConnectionsCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
EditConnectionsCmd::Do() {
    MDFView* mdfView = dynamic_cast<MDFView*>(view());
    ChildFrame* frame = dynamic_cast<ChildFrame*>(mdfView->GetFrame());
    assert(frame != NULL);
    mdfView->clearSelection();
    ConnectTool* tool = new ConnectTool(frame, mdfView);
    mdfView->canvas()->setTool(tool);
    return true;
}


/**
 * Returns id of this command.
 */
int
EditConnectionsCmd::id() const {
    return ProDeConstants::COMMAND_EDIT_CONNECTIONS;
}


/**
 * Creates and returns a new instance of this command.
 */
EditConnectionsCmd*
EditConnectionsCmd::create() const {
    return new EditConnectionsCmd();
}


/**
 * Returns path to the command's icon file.
 */
string
EditConnectionsCmd::icon() const {
    return ProDeConstants::CMD_ICON_EDIT_CONNECTIONS;
}


/**
 * Returns short version of the command name.
 */
string
EditConnectionsCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_EDIT_CONNECTIONS;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
EditConnectionsCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
