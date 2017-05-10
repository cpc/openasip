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
 * @file FitWindowCmd.cc
 *
 * Definition of FitWindowCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <wx/gdicmn.h>
#include <wx/docview.h>

#include "FitWindowCmd.hh"
#include "MDFView.hh"
#include "MachineCanvas.hh"
#include "ProDeConstants.hh"
#include "CanvasConstants.hh"

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

    wxSize figureSize = canvas->getFigureSize();
    wxSize cSize = canvas->GetSize();

    // decrease zoom factor from maximum until the figure fits in the window
    double factor = CanvasConstants::MAX_ZOOM_FACTOR;
    while (cSize.GetWidth() < factor*figureSize.GetWidth() ||
        cSize.GetHeight() < factor*figureSize.GetHeight()) {

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
