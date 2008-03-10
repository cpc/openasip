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
