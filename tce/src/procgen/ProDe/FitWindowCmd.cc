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
