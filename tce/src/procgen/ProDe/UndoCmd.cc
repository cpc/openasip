/**
 * @file UndoCmd.cc
 *
 * Definition of UndoCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#include <wx/docview.h>
#include "UndoCmd.hh"
#include "ProDe.hh"
#include "ProDeConstants.hh"
#include "MDFDocument.hh"


using std::string;

/**
 * The Constructor.
 */
UndoCmd::UndoCmd():
    EditorCommand(ProDeConstants::CMD_NAME_UNDO) {

}


/**
 * The Destructor.
 */
UndoCmd::~UndoCmd() {}


/**
 * Executes the command.
 *
 * @return Always false.
 */
bool
UndoCmd::Do() {
    dynamic_cast<MDFDocument*>(view()->GetDocument())->getModel()->undo();
    return false;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
UndoCmd::id() const {
    return ProDeConstants::COMMAND_UNDO;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
UndoCmd*
UndoCmd::create() const {
    return new UndoCmd();
}



/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
UndoCmd::icon() const {
    return ProDeConstants::CMD_ICON_UNDO;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when the model's undo stack is not empty.
 *
 * @return True, if the model's undo stack is not empty.
 */
bool
UndoCmd::isEnabled() {

    wxDocManager* manager = wxGetApp().docManager();

    wxView* view = manager->GetCurrentView();
    if (view == NULL) {
        return false;
    }

    Model* model =
        dynamic_cast<MDFDocument*>(view->GetDocument())->getModel();

    if (model != NULL && model->canUndo()) {
        return true;
    }
    return false;
}

