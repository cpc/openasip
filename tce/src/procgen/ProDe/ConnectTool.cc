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
 * @file ConnectTool.cc
 *
 * Definition of ConnectTool class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/cursor.h>

#include "ConnectTool.hh"
#include "Request.hh"
#include "ConnectRequest.hh"
#include "ComponentCommand.hh"
#include "MDFView.hh"
#include "SocketPortConnToolFigure.hh"
#include "SocketBusConnToolFigure.hh"
#include "Socket.hh"
#include "Port.hh"
#include "Segment.hh"
#include "MDFDocument.hh"
#include "ProDeConstants.hh"
#include "CommandRegistry.hh"
#include "MachineCanvas.hh"

using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 */
ConnectTool::ConnectTool(ChildFrame* frame, MDFView* view):
    MachineCanvasTool(view->canvas()),
    frame_(frame),
    view_(view),
    active_(false),
    target_(NULL),
    figure_(NULL) {

}


/**
 * The Destructor.
 */
ConnectTool::~ConnectTool() {
    if (figure_ != NULL) {
        delete figure_;
        figure_ = NULL;
    }
}


/**
 * Activates the tool.
 */
void
ConnectTool::activate() {
    wxCursor connectCursor(wxCURSOR_CROSS);
    canvas_->SetCursor(connectCursor);
    active_ = true;
}


/**
 * Deactivates the tool.
 */
void
ConnectTool::deactivate() {
    canvas_->SetCursor(wxNullCursor);
    active_ = false;
}


/**
 * Handles mouse events on the Canvas.
 *
 * @param event Mouse event to handle.
 * @param dc Device context.
 */
void
ConnectTool::onMouseEvent(wxMouseEvent& event, wxDC& dc) {

    if (!active_) {
        return;
    }

    //wxCursor connectCursor(wxCURSOR_CROSS);
    //view_->canvas()->SetCursor(connectCursor);

    // Get event position and translate "raw" coordinates to logical ones.
    wxPoint position = event.GetPosition();
    long logicalX = dc.DeviceToLogicalX(position.x);
    long logicalY = dc.DeviceToLogicalY(position.y);

    // Check if there is an EditPart at the cursor position.
    EditPart* part = canvas_->findEditPart(logicalX, logicalY);

    if (part != NULL) {
        updateStatusline(part);
        EditPart* selection = canvas_->selection();
        ConnectRequest* request = new ConnectRequest(selection);

        if (selection != NULL && part->canHandle(request)) {
            if (target_ != part) {
                target_ = part;
                canvas_->refreshToolFigure();
            }
        } else {
            if (target_ != NULL) {
                target_ = NULL;
                canvas_->refreshToolFigure();
            }
        }
    } else {
        if (target_ != NULL) {
            target_ = NULL;
            canvas_->refreshToolFigure();
        }
    }

    if (event.LeftUp()) {
        leftClick(part);
    }

    if (event.RightUp()) {
        rightClick(event);
    }
}


/**
 * Sends a status request to the given EditPart and executes the returned
 * command.
 */
void
ConnectTool::updateStatusline(EditPart* part) {
    string status = "";
    Request* request = new Request(Request::STATUS_REQUEST);
    if (part != NULL && part->canHandle(request)) {
        ComponentCommand* command = part->performRequest(request);
        if (command != NULL )
        {
            command->Do();
        }
    } else {
        frame_->setStatus(_T(""));
    }
}


/**
 * Selects Component at cursor position, if there is a selectable
 * EditPart at the coordinates.
 *
 * @param part Component to select.
 */
void
ConnectTool::leftClick(EditPart* part) {
    EditPart* selection = view_->selection();
    ConnectRequest* request = new ConnectRequest(selection);
    if (part != NULL && part->canHandle(request)) {
        if (selection == NULL) {
            canvas_->select(part);
        } else {
            ComponentCommand* cmd = part->performRequest(request);

            if (cmd != NULL) {
		Model* model = dynamic_cast<MDFDocument*>(
		    wxGetApp().docManager()->GetCurrentDocument())->getModel();
		model->pushToStack();
                if (cmd->Do()) {
		    // conenction was modified
		    model->notifyObservers();
		} else {
		    // Modification failed.
		    model->popFromStack();
		}
            }
            delete cmd;
        }
    } else {
        view_->clearSelection();
    }
    delete request;
}


/**
 * Pops a context menu when the right mouse button is clicked on the canvas.
 *
 * @param event Mouse event containing the cursor location where to pop the
 *              menu.
 */
void
ConnectTool::rightClick(wxMouseEvent& event) {
    wxMenu* contextMenu = new wxMenu();

    // Create a context menu.
    CommandRegistry* registry = wxGetApp().commandRegistry();
    contextMenu->Append(ProDeConstants::COMMAND_UNDO, _T("&Undo"));
    contextMenu->Append(ProDeConstants::COMMAND_REDO, _T("&Redo"));
    contextMenu->Enable(ProDeConstants::COMMAND_UNDO,
                        registry->isEnabled(ProDeConstants::CMD_NAME_UNDO));
    contextMenu->Enable(ProDeConstants::COMMAND_REDO,
                        registry->isEnabled(ProDeConstants::CMD_NAME_REDO));
    contextMenu->Enable(ProDeConstants::COMMAND_PASTE,
                        registry->isEnabled(ProDeConstants::CMD_NAME_PASTE));
    contextMenu->AppendSeparator();
    contextMenu->Append(ProDeConstants::COMMAND_SELECT, _T("&Select Tool"));

    // Pop the menu at cursor position.
    wxPoint position = event.GetPosition();
    view_->canvas()->PopupMenu(contextMenu, wxPoint(position.x, position.y));
}


/**
 * Returns the tool Figure.
 *
 * @return Tool figure.
 */
Figure*
ConnectTool::figure() {

    // Delete old figure.
    if (figure_ != NULL) {
        delete figure_;
        figure_ = NULL;
    }

    EditPart* selection = view_->selection();

    if (target_ == NULL || selection == NULL) {
        return NULL;
    }

    Socket* socket = NULL;
    Port* port = NULL;
    Segment* segment = NULL;

    // socket
    socket = dynamic_cast<Socket*>(selection->model());
    if (socket == NULL) {
        socket = dynamic_cast<Socket*>(target_->model());
    }

    // port
    port = dynamic_cast<Port*>(selection->model());
    if (port == NULL) {
        port = dynamic_cast<Port*>(target_->model());
    }

    // segment
    segment = dynamic_cast<Segment*>(selection->model());
    if (segment == NULL) {
        segment = dynamic_cast<Segment*>(target_->model());
    }

    // socket - port connection
    if (socket != NULL && port != NULL) {
        SocketPortConnToolFigure* figure = NULL;
        if (port->isConnectedTo(*socket)) {
            figure = new SocketPortConnToolFigure(false);
        } else {
            figure = new SocketPortConnToolFigure(true);
        }
        if (selection->model() == socket) {
            figure->setSource(target_->figure());
            figure->setTarget(selection->figure());
        } else {
            figure->setSource(selection->figure());
            figure->setTarget(target_->figure());
        }
        figure_ = figure;
        return figure_;
    }

    if (socket != NULL && segment != NULL) {
        // return SocketBusConnToolFigure
        SocketBusConnToolFigure* figure = NULL;
        if (socket->isConnectedTo(*segment)) {
            figure = new SocketBusConnToolFigure(false);
        } else {
            figure = new SocketBusConnToolFigure(true);
        }
        if (selection->model() == segment) {
            figure->setSource(target_->figure());
            figure->setTarget(selection->figure());
        } else {
            figure->setSource(selection->figure());
            figure->setTarget(target_->figure());
        }
        figure_ = figure;
        return figure;
    }
    target_ = NULL;
    return NULL;
}
