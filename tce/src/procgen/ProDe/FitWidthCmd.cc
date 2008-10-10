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
 * @file FitWidthCmd.cc
 *
 * Definition of FitWidthCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <wx/gdicmn.h>
#include <wx/docview.h>

#include "FitWidthCmd.hh"
#include "MDFView.hh"
#include "MachineCanvas.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"

using std::string;

/**
 * The Constructor.
 */
FitWidthCmd::FitWidthCmd():
    EditorCommand(ProDeConstants::CMD_NAME_ZOOM_FIT_WID) {

}


/**
 * The Destructor.
 */
FitWidthCmd::~FitWidthCmd() {
}



/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
FitWidthCmd::Do() {

    MDFView* mView = dynamic_cast<MDFView*>(view());
    MachineCanvas* canvas = mView->canvas();

    int height;
    int width;
    canvas->GetVirtualSize(&width, &height);
    wxSize cSize = canvas->GetSize();

    // Decrease zoom factor from maximum until the figure width is smaller
    // than the window width.
    double factor = ProDeConstants::MAX_ZOOM_FACTOR;
    while (cSize.GetWidth() < factor*width) {
	factor = factor - ProDeConstants::ZOOM_STEP;
    }

    // if zoom factor is smaller than minimum, set as minimum
    if (factor < ProDeConstants::MIN_ZOOM_FACTOR) {
	factor = ProDeConstants::MIN_ZOOM_FACTOR;
    }

    canvas->setZoomFactor(factor);
    canvas->Refresh();
    return true;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
FitWidthCmd::id() const {
    return ProDeConstants::COMMAND_ZOOM_FIT_WID;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
FitWidthCmd*
FitWidthCmd::create() const {
    return new FitWidthCmd();
}


/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
FitWidthCmd::icon() const {
    return ProDeConstants::CMD_ICON_ZOOM_FIT_WID;
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
FitWidthCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ZOOM_FIT_WID;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
FitWidthCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
