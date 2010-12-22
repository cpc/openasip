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
 * @file MachineCanvas.cc
 *
 * Definition of MachineCanvas class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <fstream>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/event.h>
#include "Segment.hh"
#include "MachineCanvas.hh"
#include "MachineCanvasTool.hh"
#include "MachineEditPartFactory.hh"
#include "RootEditPart.hh"
#include "Figure.hh"
#include "SelectionFigure.hh"
#include "SequenceTools.hh"
#include "Bus.hh"
#include "Port.hh"
#include "MoveFigure.hh"
#include "EPSDC.hh"
#include "WxConversion.hh"

BEGIN_EVENT_TABLE(MachineCanvas, wxScrolledWindow)
  EVT_MOUSE_EVENTS(MachineCanvas::onMouseEvent)
END_EVENT_TABLE()


using namespace TTAMachine;

/**
 * The Constructor.
 * 
 * @param parent Parent frame of the canvas.
 * @param policyFactory EditPolicyFactory for creating edit policies.
 */
MachineCanvas::MachineCanvas(
    wxWindow* parent,
    EditPolicyFactory* policyFactory) :
    wxScrolledWindow(parent),
    tool_(NULL), 
    machine_(NULL),
    editPolicyFactory_(policyFactory),
    zoomFactor_(1.0),
    dirty_(true),
    root_(NULL) ,
    selection_(NULL),
    toolBounds_(0, 0, 0, 0) {

    root_ = new RootEditPart();

    SetScrollRate(20,20);
    SetVirtualSize(1000, 800);
}

/**
 * The Destructor.
 */
MachineCanvas::~MachineCanvas() {
    if (root_ != NULL) {
	delete root_;
    }
    clearMoves();
}


/**
 * Called when the window needs to be drawn again.
 *
 * @param dc The device context where to do the actual drawing.
 */
void
MachineCanvas::OnDraw(wxDC& dc) {

    wxBrush backgroundBrush(*wxLIGHT_GREY, wxSOLID);
    dc.SetBackground(backgroundBrush);
    dc.Clear();

    // Set the canvas font.
    dc.SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxNORMAL));

    // Set scaling factor.
    dc.SetUserScale(zoomFactor_, zoomFactor_);

    // draw machine
    if (root_->contents() != NULL) {
	if (dirty_) {
	    root_->contents()->figure()->layout(&dc);
	    // Set Canvas virtual size according to the size needed by the
	    // figures.
	    wxRect bounds = root_->contents()->figure()->bounds();
	    SetVirtualSize(
		int(bounds.GetWidth() * zoomFactor()),
		int(bounds.GetHeight() * zoomFactor()));

	    dirty_ = false;
	}
	root_->contents()->figure()->draw(&dc);

    }

    // draw tool figure
    if (tool() != NULL) {
        Figure* toolFigure = tool()->figure();
        if (toolFigure != NULL) {
            toolFigure->layout(&dc);
            toolFigure->draw(&dc);
        }
    }

    // Draw moves.
    for (unsigned i = 0; i < moveFigures_.size(); i++) {
        moveFigures_[i]->draw(&dc);
    }

    // Draw selection rectangle.
    if (selection() != NULL) {
        SelectionFigure figure(selection()->figure());
        figure.layout(&dc);
        figure.draw(&dc);
    }
}

/**
 * Refreshes the tool figures on the canvas.
 */
void
MachineCanvas::refreshToolFigure() {

    if (toolBounds_.GetWidth() > 0 && toolBounds_.GetHeight() > 0) {
	// Clear previous tool figure.
        refreshLogicalRect(toolBounds_);
    }

    Figure* toolFigure = tool()->figure();
    if (toolFigure != NULL) {
	// Draw new tool figure.
        toolFigure->layout(NULL);
        toolBounds_ = toolFigure->bounds();
        refreshLogicalRect(toolBounds_);
    } else {
        toolBounds_ = wxRect(0, 0, 0, 0);
    }
}


/**
 * Sets the zoom factor of the canvas.
 *
 * @param factor New zoom factor of the canvas.
 */
void
MachineCanvas::setZoomFactor(double factor) {
    // when scrolling, try to:
    // * On X direction, keep the middle pointing to same position in machine
    // * On Y direction, keep the upper edge of the windows pointer to
    //   same position in the machine.
    // This code calculates this and does the scrolling.
    int x,y;
    int xSize, ySize;
    int scrollUnitX, scrollUnitY;
    GetScrollPixelsPerUnit(&scrollUnitX, &scrollUnitY);
    GetViewStart(&x, &y);
    x *= scrollUnitX;
    y *= scrollUnitY;
    GetSize(&xSize, &ySize);
    double xPos = (x + (xSize>>1)) / zoomFactor_;
    double yPos = y / zoomFactor_;
    x = std::max(int(xPos*factor) - (xSize>>1),0);
    y = yPos*factor;
    Scroll((x+(scrollUnitX>>1))/scrollUnitX,
           (y+(scrollUnitY>>1))/scrollUnitY);

    // then do the zooming
    zoomFactor_ = factor;
    dirty_ = true;
    Refresh();
}


/**
 * Returns the zoom factor of the canvas.
 *
 * @return Zoom factor of the canvas.
 */
double
MachineCanvas::zoomFactor() {
    return zoomFactor_;
}


/**
 * Handles mouse events on the canvas.
 *
 * Passes the mouse event to the active tool.
 * 
 * @param event Mouse event to pass to the active tool.
 */
void
MachineCanvas::onMouseEvent(wxMouseEvent& event) {

    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetUserScale(zoomFactor_, zoomFactor_);

    if (tool_ != NULL) {
	tool_->onMouseEvent(event, dc);
    }
}

/**
 * Sets the active tool.
 * 
 * @param tool MachineCanvasTool to activate.
 */
void
MachineCanvas::setTool(MachineCanvasTool* tool) {
    if (tool_ != NULL) {
	tool_->deactivate();
        delete(tool_);
    }
    tool_ = tool;
    tool_->activate();
}

/**
 * Returns pointer to the current tool of the canvas.
 */
MachineCanvasTool*
MachineCanvas::tool() {
    return tool_;
}


/**
 * Refreshes area of the MachineCanvas bound by a logical coordinate rectangle.
 *
 * @param rectangle Area to refresh.
 */
void
MachineCanvas::refreshLogicalRect(const wxRect& rectangle) {

    int x = 0;
    int y = 0;
    CalcScrolledPosition(int(rectangle.x * zoomFactor_),
                         int(rectangle.y * zoomFactor_),
                         &x, &y);
    wxRect zoomed(x, y,
                  int(rectangle.width * zoomFactor_),
                  int(rectangle.height * zoomFactor_));

    RefreshRect(zoomed);
}


/**
 * Updates the machine from the machine object model.
 */
void
MachineCanvas::updateMachine() {

    clearSelection();
    clearMoves();
    dirty_ = true;

    if (machine_ == NULL) {
        root_->setContents(NULL);
	return;
    }

    MachineEditPartFactory factory(*editPolicyFactory_);
    EditPart* contents = factory.createEditPart(machine_);
    assert(root_ != NULL);
    root_->setContents(contents);
    contents->figure()->setOptions(&options_);
    Refresh();
}


/**
 * Sets the machine which is dispalyed on the canvas.
 *
 * @param machine Machine to display on the canvas.
 */
void
MachineCanvas::setMachine(TTAMachine::Machine* machine) {
    machine_ = machine;
    updateMachine();
}



/**
 * Clears component selection.
 */
void
MachineCanvas::clearSelection() {
    if (selection_ != NULL) {
        selection_->setSelected(false);
    }
    selection_ = NULL;
    Refresh();
}


/**
 * Returns pointer to the EditPart of the selected component.
 *
 * @return Pointer to the selected EditPart, or null if selection is empty.
 */
EditPart*
MachineCanvas::selection() {
    return selection_;
}


/**
 * Finds an edit part at the given coordinates on the canvas.
 *
 * @param x X-coordinate of the position to search.
 * @param y Y-coordinate of the position to search.
 * @return Pointer to the found edit part, or NULL if no edit part was found.
 */
EditPart*
MachineCanvas::findEditPart(int x, int y) {
    EditPart* part = NULL;
    if (root_ != NULL && root_->contents() != NULL) {
        part = root_->contents()->find(wxPoint(x, y));
    }
    return part;
}


/**
 * Finds an edit part corresponding the given machine part.
 *
 * @param model MachinePart to find
 * @return Pointer to the found edit part, or NULL if no edit part was found.
 */
EditPart*
MachineCanvas::findEditPart(const TTAMachine::MachinePart* model) {
    EditPart* part = NULL;
    if (root_ != NULL && root_->contents() != NULL) {
        part = root_->contents()->find(model);
    }
    return part;
}

/**
 * Marks an edit part selected.
 *
 * Previous selection is cleared.
 *
 * @param part Edit part to select.
 */
void
MachineCanvas::select(EditPart* part) {
    if (selection_ != NULL) {
        selection_->setSelected(false);
    }

    // segments are not supported so we should select the parent
    // bus instead the segment (a very long standing irritating bug)
    TTAMachine::Segment* segment = 
        dynamic_cast<TTAMachine::Segment*>(part->model());
    if (segment != NULL) {
        // clicked segment, selecting parent bus instead
        part = part->parent();
    }

    selection_ = part;
    if (selection_ != NULL) {
        selection_->setSelected(true);
    }
    Refresh();
}

/**
 * Highlights figure of a machine component.
 */
void
MachineCanvas::highlight(
    TTAMachine::MachinePart* component, const wxColour& colour) {

    if (root_->contents() == NULL) {
        return;
    }

    EditPart* editPart = root_->contents()->find(component);
    if (editPart != NULL && editPart->figure() != NULL) {
        editPart->figure()->highlight(colour);
    }
}

/**
 * Clears highlighting of all machine components.
 */
void
MachineCanvas::clearHighlights() {
    if (root_ != NULL && root_->contents() != NULL) {
	root_->contents()->figure()->clearHighlight();
    }
}

/**
 * Adds a move to be drawn on the canvas.
 *
 * @param bus Transport bus of the move.
 * @param source Source port of the move.
 * @param target Target port of the move.
 */
void
MachineCanvas::addMove(const Bus* bus, const Port* source, const Port* target)
    throw (InstanceNotFound) {

    std::string procName = "MachineCanvas::addMove";
    EditPart* busEditPart = findEditPart(bus);

    if (busEditPart == NULL) {
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }

    Figure* busFigure = busEditPart->figure();
    Figure* sourceFigure = NULL;
    Figure* targetFigure = NULL;

    if (source != NULL) {
        EditPart* sourceEditPart = findEditPart(source);
        if (sourceEditPart == NULL) {
            throw InstanceNotFound(__FILE__, __LINE__, procName);
        }
        sourceFigure = sourceEditPart->figure();
    }


    if (target != NULL) {
        EditPart* targetEditPart = findEditPart(target);
        if (targetEditPart == NULL) {
            throw InstanceNotFound(__FILE__, __LINE__, procName);
        }
        targetFigure = targetEditPart->figure();
    }

    moveFigures_.push_back(
        new MoveFigure(busFigure, sourceFigure, targetFigure));
}


/**
 * Clears list of moves to be drawn on the canvas.
 */
void
MachineCanvas::clearMoves() {
    SequenceTools::deleteAllItems(moveFigures_);
}


/**
 * Saves the machine figure to an eps file.
 *
 * @param filename Name of the eps file.
 * @param title Title of the eps document.
 * @param creator Creator of the eps document.
 * @return True, if the eps was succesfully saved.
 */
bool
MachineCanvas::saveEPS(
    const std::string& filename, const std::string& title,
    const std::string& creator) {

    EPSDC dc;
    dc.setCreator(creator);

    dc.setTitle(title);
    dc.StartPage();
    OnDraw(dc);
    dc.EndPage();

    std::ofstream file(filename.c_str());

    if (file.bad()) {
        return false;
    }

    dc.writeToStream(file);
    file.close();

    return true;
}

/**
 * Saves the machine figure to a .png file.*
 *
 * @param filename Name of the .png file.
 * @return True, if the png was succesfully saved.
 */
bool
MachineCanvas::savePNG(const std::string& filename) {

    // Refresh machine figure to get the canvas size.
    wxClientDC clientDC(this);
    OnDraw(clientDC);

    // Add minimum coordinates to maximum coordinates to create margins
    // of equal width.
    int width = clientDC.MaxX() + clientDC.MinX();
    int height = clientDC.MaxY() + clientDC.MinY();

    // Create bitmap and a MemoryDC which writes the bitmap.
    wxMemoryDC bitmapDC;
    wxBitmap bitmap(width, height);
    bitmapDC.SelectObject(bitmap);
    OnDraw(bitmapDC);

    wxString file = WxConversion::toWxString(filename);

    // Save bitmap.
    if (bitmap.SaveFile(file, wxBITMAP_TYPE_PNG)) {
        return true;
    }

    return false;
}

/**
 * Returns the current options used by machine canvas figures.
 *
 * @return Current machine canvas options.
 */
MachineCanvasOptions&
MachineCanvas::options() {
    return options_;
}
