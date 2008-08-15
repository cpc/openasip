/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ProximMachineCanvasTool.cc
 *
 * Implementation of ProximMachineCanvasTool class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/tipwin.h>
#include "Application.hh"
#include "ProximMachineCanvasTool.hh"
#include "MachineCanvas.hh"
#include "ProximMachineStateWindow.hh"
#include "ComponentCommand.hh"
#include "Request.hh"
#include "EditPart.hh"
#include "Proxim.hh"
#include "MachinePart.hh"
#include "WxConversion.hh"
#include "ProximToolbox.hh"

/**
 * The Constructor.
 *
 * @param canvas MachineCanvas where the tool is used.
 */
ProximMachineCanvasTool::ProximMachineCanvasTool(MachineCanvas* canvas) :
    MachineCanvasTool(canvas),
    canvas_(canvas),
    toolTip_(NULL) {

}


/**
 * The Destructor.
 */
ProximMachineCanvasTool::~ProximMachineCanvasTool() {
}


/**
 * Handles mouse events on the canvas.
 */
void
ProximMachineCanvasTool::onMouseEvent(wxMouseEvent& event, wxDC& dc) {

    // Get event position and translate "raw" coordinates to logical ones.
    wxPoint position = event.GetPosition();
    int x = position.x;
    int y = position.y;
    long logicalX = dc.DeviceToLogicalX(position.x);
    long logicalY = dc.DeviceToLogicalY(position.y);

    // Check if there is an EditPart at the cursor position.
    EditPart* part = canvas_->findEditPart(logicalX, logicalY);
    

    Request request(Request::STATUS_REQUEST);
    if (part != NULL && part->canHandle(&request)) {
        ComponentCommand* command = part->performRequest(&request);
        command->Do();
        delete command;
    } else {
        ProximToolbox::machineStateWindow()->setStatusText("");
    }

    if (event.LeftDClick()) {
        Request request(Request::MODIFY_REQUEST);
        if (part != NULL && part->canHandle(&request)) {
            ComponentCommand* command = part->performRequest(&request);
            command->setParentWindow(wxGetApp().GetTopWindow());
            command->Do();
            delete command;
        }
    }

    if (event.LeftDown()) {
        canvas_->clearSelection();
        ProximToolbox::machineStateWindow()->clearDetails();
        Request statusRequest(Request::DETAILS_REQUEST);
        if (part != NULL && part->canHandle(&statusRequest)) {
            canvas_->select(part);
            ComponentCommand* command = part->performRequest(&statusRequest);
            command->Do();
            delete command;
        }
    }

    if (event.RightDown()) {
	popupMenu(x, y);
    }
}


/**
 * Pops a menu at the given position.
 *
 * @param x X-coordinate of the popup location.
 * @param y Y-coordinate of the popup location.
 */
void
ProximMachineCanvasTool::popupMenu(int x, int y) {
    wxMenu* popupMenu = new wxMenu();
    popupMenu->Append(
	ProximMachineStateWindow::COMMAND_ZOOM_IN, _T("Zoom In"));
    popupMenu->Append(
	ProximMachineStateWindow::COMMAND_ZOOM_OUT, _T("Zoom Out"));
    popupMenu->AppendSeparator();
    popupMenu->AppendCheckItem(
        ProximMachineStateWindow::COMMAND_TOGGLE_UNIT_INFO,
        _T("Display Unit Info"));
    popupMenu->AppendSeparator();
    popupMenu->AppendCheckItem(
        ProximMachineStateWindow::COMMAND_TOGGLE_MOVES, _T("Display moves"));
    popupMenu->AppendCheckItem(
        ProximMachineStateWindow::COMMAND_TOGGLE_UTILIZATIONS,
        _T("Display utilizations"));

    popupMenu->AppendSeparator();
    popupMenu->Append(
	ProximMachineStateWindow::COMMAND_EXPORT, _T("Export figure..."));

    canvas_->PopupMenu(popupMenu, wxPoint(x, y));
}

/**
 * Called when the tool is activated.
 */
void
ProximMachineCanvasTool::activate() {
    // do nothing
}


/**
 * Called when the tool is deactivated.
 */
void
ProximMachineCanvasTool::deactivate() {
    // do nothing
}
