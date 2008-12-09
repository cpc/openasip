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
 * @file ProximMachineCanvasTool.cc
 *
 * Implementation of ProximMachineCanvasTool class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
