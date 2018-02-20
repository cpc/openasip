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
 * @file FitHeightCmd.cc
 *
 * Definition of FitHeightCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <wx/gdicmn.h>
#include <wx/docview.h>

#include "FitHeightCmd.hh"
#include "MDFView.hh"
#include "MachineCanvas.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"
#include "CanvasConstants.hh"

using std::string;

/**
 * The Constructor.
 */
FitHeightCmd::FitHeightCmd():
    EditorCommand(ProDeConstants::CMD_NAME_ZOOM_FIT_HEI) {

}


/**
 * The Destructor.
 */
FitHeightCmd::~FitHeightCmd() {
}



/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
FitHeightCmd::Do() {

    MDFView* mView = dynamic_cast<MDFView*>(view());
    MachineCanvas* canvas = mView->canvas();

    wxSize figureSize = canvas->getFigureSize();
    wxSize cSize = canvas->GetSize();

    // Decrease zoom factor from maximum until the figure height is smaller
    // than the window height.
    double factor = CanvasConstants::MAX_ZOOM_FACTOR;
    while (cSize.GetHeight() < factor*figureSize.GetHeight()) {
        factor = factor - CanvasConstants::AUTOZOOM_STEP;
    }

    canvas->setZoomFactor(factor);
    return true;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
FitHeightCmd::id() const {
    return ProDeConstants::COMMAND_ZOOM_FIT_HEI;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
FitHeightCmd*
FitHeightCmd::create() const {
    return new FitHeightCmd();
}


/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
FitHeightCmd::icon() const {
    return ProDeConstants::CMD_ICON_ZOOM_FIT_HEI;
}

/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
FitHeightCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ZOOM_FIT_HEI;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
FitHeightCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
        return true;
    }
    return false;
}
