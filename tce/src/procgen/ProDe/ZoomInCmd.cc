/**
 * @file ZoomInCmd.cc
 *
 * Definition of ZoomInCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include <wx/docview.h>

#include "ZoomInCmd.hh"
#include "MDFView.hh"
#include "MachineCanvas.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"

using std::string;

/**
 * The Constructor.
 */
ZoomInCmd::ZoomInCmd() : EditorCommand(ProDeConstants::CMD_NAME_ZOOM_IN) {

}


/**
 * The Destructor.
 */
ZoomInCmd::~ZoomInCmd() {
}



/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ZoomInCmd::Do() {
    MDFView* mView = dynamic_cast<MDFView*>(view());
    MachineCanvas* canvas = mView->canvas();
    double factor = canvas->zoomFactor() + ProDeConstants::ZOOM_STEP;
    if (factor > ProDeConstants::MAX_ZOOM_FACTOR) {
	factor = ProDeConstants::MAX_ZOOM_FACTOR;
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
ZoomInCmd::id() const {
    return ProDeConstants::COMMAND_ZOOM_IN;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
ZoomInCmd*
ZoomInCmd::create() const {
    return new ZoomInCmd();
}


/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
ZoomInCmd::icon() const {
    return ProDeConstants::CMD_ICON_ZOOM_IN;
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
ZoomInCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ZOOM_IN;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
ZoomInCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
