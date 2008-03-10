/**
 * @file FitHeightCmd.cc
 *
 * Definition of FitHeightCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include <wx/gdicmn.h>
#include <wx/docview.h>

#include "FitHeightCmd.hh"
#include "MDFView.hh"
#include "MachineCanvas.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"

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

    int height;
    int width;
    canvas->GetVirtualSize(&width, &height);
    wxSize cSize = canvas->GetSize();

    // Decrease zoom factor from maximum until the figure height is smaller
    // than the window height.
    double factor = ProDeConstants::MAX_ZOOM_FACTOR;
    while (cSize.GetHeight() < factor*height) {
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
