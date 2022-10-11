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
 * @file FitWidthCmd.cc
 *
 * Definition of FitWidthCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <wx/gdicmn.h>
#include <wx/docview.h>

#include "FitWidthCmd.hh"
#include "MDFView.hh"
#include "MachineCanvas.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"
#include "CanvasConstants.hh"

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

    wxSize figureSize = canvas->getFigureSize();
    wxSize cSize = canvas->GetSize();

    // Decrease zoom factor from maximum until the figure width is smaller
    // than the window width.
    double factor = CanvasConstants::MAX_ZOOM_FACTOR;

    while (cSize.GetWidth() < factor*figureSize.GetWidth()) {
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
