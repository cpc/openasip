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
 * @file SelectCmd.cc
 *
 * Definition of SelectCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "SelectCmd.hh"
#include "SelectTool.hh"
#include "MDFView.hh"
#include "ProDe.hh"
#include "ProDeConstants.hh"
#include "MachineCanvas.hh"

using std::string;

/**
 * The Constructor.
 */
SelectCmd::SelectCmd():
    EditorCommand(ProDeConstants::CMD_NAME_SELECT) {

}


/**
 * The Destructor.
 */
SelectCmd::~SelectCmd() {
}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
SelectCmd::Do() {
    MDFView* mdfView = dynamic_cast<MDFView*>(view());
    ChildFrame* frame = dynamic_cast<ChildFrame*>(mdfView->GetFrame());
    assert(frame != NULL);
    mdfView->clearSelection();
    SelectTool* tool = new SelectTool(frame, mdfView);
    mdfView->canvas()->setTool(tool);
    return true;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
SelectCmd::id() const {
    return ProDeConstants::COMMAND_SELECT;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
SelectCmd*
SelectCmd::create() const {
    return new SelectCmd();
}


/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
SelectCmd::icon() const {
    return ProDeConstants::CMD_ICON_SELECT;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
SelectCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
        return true;
    }
    return false;
}
