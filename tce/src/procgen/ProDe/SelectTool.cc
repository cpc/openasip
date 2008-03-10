/**
 * @file SelectTool.cc
 *
 * Definition of SelectTool class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <wx/event.h>
#include <wx/gdicmn.h>

#include "SelectTool.hh"
#include "Conversion.hh"
#include "WxConversion.hh"
#include "MDFView.hh"
#include "EditPart.hh"
#include "Request.hh"
#include "ComponentCommand.hh"
#include "ModifyComponentCmd.hh"
#include "CommandRegistry.hh"
#include "ProDeConstants.hh"
#include "SelectionFigure.hh"
#include "MachineCanvas.hh"
#include "ProDe.hh"
#include "MainFrame.hh"

using std::string;


/**
 * The Constructor.
 */
SelectTool::SelectTool(
    ChildFrame* frame, MDFView* view):
    MachineCanvasTool(view->canvas()),
    frame_(frame),
    view_(view),
    active_(false) {

    figure_ = new SelectionFigure(NULL);
}


/**
 * The Destructor.
 */
SelectTool::~SelectTool() {
    delete figure_;
}


/**
 * Activates the tool.
 */
void
SelectTool::activate() {
    active_ = true;
}


/**
 * Deactivates the tool.
 */
void
SelectTool::deactivate() {
    active_ = false;
}

/**
 * Returns figure of the selection.
 *
 * @return Figure of the selection.
 */
Figure*
SelectTool::figure() {
    return NULL;
}

/**
 * Handles mouse events on the Canvas.
 *
 * @param event Mouse event to handle.
 * @param dc Device context.
 */
void
SelectTool::onMouseEvent(wxMouseEvent& event, wxDC& dc) {

    if (!active_) {
        return;
    }

    // Get event position and translate "raw" coordinates to logical ones.
    wxPoint position = event.GetPosition();
    int x = position.x;
    int y = position.y;
    long logicalX = dc.DeviceToLogicalX(position.x);
    long logicalY = dc.DeviceToLogicalY(position.y);

    // Check if there is an EditPart at the cursor position.
    EditPart* part = canvas_->findEditPart(logicalX, logicalY);

    // If an EditPart was found at the cursor position, get name of the
    // Component related to the EditPart.
    string status = "";
    Request* request = new Request(Request::STATUS_REQUEST);
    if (part != NULL && part->canHandle(request)) {
        ComponentCommand* command = part->performRequest(request);
        command->Do();
    } else {
        frame_->setStatus(_T(""));
    }

    // If left mouse button was released, select the EditPart at cursor
    // position. (if there is a selectable EditPart)
    if (event.LeftUp()) {
	leftClick(part);
    }

    if (event.RightDown()) {
	// select component
	leftClick(part);
	// popup context menu
	popContextMenu(part, x, y);
    }

    // left mouse button double click executes ModifyComponentCmd
    if (event.ButtonDClick(1)) {
	ModifyComponentCmd* command = new ModifyComponentCmd();
	command->setParentWindow(frame_);
	command->setView(view_);
	command->Do();
    }

    wxGetApp().mainFrame()->updateUI();
}


/**
 * Selects Component at cursor position, if there is a selectable
 * EditPart at the coordinates.
 *
 * @param part Component to select.
 */
void
SelectTool::leftClick(EditPart* part) {
    if (part != NULL && part->selectable()) {
        canvas_->select(part);
        figure_->setSelection(part->figure());
    } else {
	canvas_->clearSelection();
    }
}


/**
 * Pops a context menu for the EditPart at the coordinates;
 *
 * @param part Context component.
 * @param x X-coordinate for the menu.
 * @param y Y-coordinate for the menu.
 */
void
SelectTool::popContextMenu(EditPart* part, int x, int y) {

    wxMenu* contextMenu = NULL;

    if (part == NULL) {
	contextMenu = createDefaultMenu();

    } else {

	CommandRegistry* registry = wxGetApp().commandRegistry();

	contextMenu = new wxMenu();
	contextMenu->Append(ProDeConstants::COMMAND_COPY,
			    _T("&Copy"));
	contextMenu->Append(ProDeConstants::COMMAND_CUT,
			    _T("Cu&t"));
	contextMenu->AppendSeparator();
	contextMenu->Append(ProDeConstants::COMMAND_MODIFY_COMP,
			    _T("&Modify..."));
	contextMenu->Append(ProDeConstants::COMMAND_DELETE_COMP,
			    _T("&Delete"));

	contextMenu->Enable(
	    ProDeConstants::COMMAND_COPY,
	    registry->isEnabled(ProDeConstants::CMD_NAME_COPY));

	contextMenu->Enable(
            ProDeConstants::COMMAND_CUT,
            registry->isEnabled(ProDeConstants::CMD_NAME_CUT));

        contextMenu->Enable(
            ProDeConstants::COMMAND_MODIFY_COMP,
            registry->isEnabled(ProDeConstants::CMD_NAME_MODIFY_COMP));

        contextMenu->Enable(
            ProDeConstants::COMMAND_DELETE_COMP,
            registry->isEnabled(ProDeConstants::CMD_NAME_MODIFY_COMP));
    }

    view_->canvas()->PopupMenu(contextMenu, wxPoint(x,y));
}


/**
 * Returns a popup menu to be used as a default context menu on the canvas.
 */
wxMenu*
SelectTool::createDefaultMenu() {

    wxMenu* popupMenu = new wxMenu();
    CommandRegistry* registry = wxGetApp().commandRegistry();
    popupMenu->Append(ProDeConstants::COMMAND_UNDO, _T("&Undo"));
    popupMenu->Append(ProDeConstants::COMMAND_REDO, _T("&Redo"));
    popupMenu->Append(ProDeConstants::COMMAND_PASTE, _T("&Paste"));

    popupMenu->Enable(ProDeConstants::COMMAND_UNDO,
		      registry->isEnabled(ProDeConstants::CMD_NAME_UNDO));
    popupMenu->Enable(ProDeConstants::COMMAND_REDO,
		      registry->isEnabled(ProDeConstants::CMD_NAME_REDO));
    popupMenu->Enable(ProDeConstants::COMMAND_PASTE,
		      registry->isEnabled(ProDeConstants::CMD_NAME_PASTE));


    popupMenu->AppendSeparator();

    wxMenu* addSubMenu = new wxMenu;
    addSubMenu->Append(ProDeConstants::COMMAND_ADD_FU,
		       _T("&Function Unit..."));
    addSubMenu->Append(ProDeConstants::COMMAND_ADD_RF,
		       _T("&Register File..."));
    addSubMenu->Append(ProDeConstants::COMMAND_ADD_BUS,
		       _T("&Transport Bus..."));
    addSubMenu->Append(ProDeConstants::COMMAND_ADD_SOCKET,
		       _T("&Socket..."));
    addSubMenu->Append(ProDeConstants::COMMAND_ADD_BRIDGE,
		       _T("&Bridge..."));
    addSubMenu->Append(ProDeConstants::COMMAND_ADD_IU,
		       _T("&Immediate Unit..."));
    addSubMenu->Append(ProDeConstants::COMMAND_ADD_GCU,
		       _T("&Global Control Unit..."));
    addSubMenu->Append(ProDeConstants::COMMAND_ADD_AS,
		       _T("&Address Space..."));
    popupMenu->Append(ID_ADD_SUBMENU, _T("&Add"), addSubMenu);

    popupMenu->Append(ProDeConstants::COMMAND_EDIT_CONNECTIONS,
		      _T("Edit &Connections"));
    popupMenu->Append(ProDeConstants::COMMAND_SELECT,
		      _T("&Select"));
    popupMenu->AppendSeparator();
    popupMenu->Append(ProDeConstants::COMMAND_VERIFY_MACHINE,
		      _T("&Verify"));

    return popupMenu;
}
