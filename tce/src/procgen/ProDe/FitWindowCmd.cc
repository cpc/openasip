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
 * @file FitWindowCmd.cc
 *
 * Definition of FitWindowCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include <wx/gdicmn.h>
#include <wx/docview.h>

#include "FitWindowCmd.hh"
#include "MDFView.hh"
#include "MachineCanvas.hh"
#include "ProDeConstants.hh"

using std::string;

/**
 * The Constructor.
 */
FitWindowCmd::FitWindowCmd():
    EditorCommand(ProDeConstants::CMD_NAME_ZOOM_FIT_WIN) {

}


/**
 * The Destructor.
 */
FitWindowCmd::~FitWindowCmd() {
}



/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
FitWindowCmd::Do() {

    MDFView* mView = dynamic_cast<MDFView*>(view());
    MachineCanvas* canvas = mView->canvas();

    int width;
    int height;
    canvas->GetVirtualSize(&width, &height);
    wxSize cSize = canvas->GetSize();

    // decrease zoom factor from maximum until the figure fits in the window
    double factor = ProDeConstants::MAX_ZOOM_FACTOR;
    while (cSize.GetWidth() < factor*width ||
	   cSize.GetHeight() < factor*height) {

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
FitWindowCmd::id() const {
    return ProDeConstants::COMMAND_ZOOM_FIT_WIN;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
FitWindowCmd*
FitWindowCmd::create() const {
    return new FitWindowCmd();
}


/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
FitWindowCmd::icon() const {
    return ProDeConstants::CMD_ICON_ZOOM_FIT_WIN;
}



/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
FitWindowCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ZOOM_FIT_WIN;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
FitWindowCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
