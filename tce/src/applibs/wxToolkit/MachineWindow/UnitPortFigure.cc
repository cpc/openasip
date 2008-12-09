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
 * @file UnitPortFigure.cc
 *
 * Definition of UnitPortFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#include <vector>

#include "UnitPortFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

using std::vector;

const wxColour UnitPortFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);
const wxColour UnitPortFigure::DEFAULT_BG_COLOUR = wxColour(255, 255, 255);

/**
 * The Constructor.
 */
UnitPortFigure::UnitPortFigure(std::string name): Figure(), name_(name) {
    size_ = wxSize(
	MachineCanvasLayoutConstraints::PORT_WIDTH,
	MachineCanvasLayoutConstraints::PORT_WIDTH);
}

/**
 * The Destructor.
 */
UnitPortFigure::~UnitPortFigure() {
}

/**
 * Sets the x-coordinate for its child sockets.
 */
void
UnitPortFigure::layoutChildren(wxDC*) {

    int socketX = location_.x;
    bool oneSet = false;

    for (unsigned int i = 0; i < children_.size(); i++) {

	if (!oneSet && !children_[i]->xSet()) {
	    children_[i]->setX(location_.x);
	    oneSet = true;
	    socketX += MachineCanvasLayoutConstraints::SOCKET_WIDTH +
		       MachineCanvasLayoutConstraints::SOCKET_MIN_SPACE;

	} else if (children_[i]->location().x == 0) {
	    children_[i]->setPreferredX(socketX);
	}
    }
}

/**
 * Draws the port's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
UnitPortFigure::drawSelf(wxDC* dc) {
    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    dc->SetPen(pen);
    wxBrush brush = wxBrush(DEFAULT_BG_COLOUR, wxSOLID);
    if (highlighted_) {
        brush = wxBrush(highlight_, wxSOLID);
    }
    dc->SetBrush(brush);
    dc->DrawRectangle(location_.x, location_.y, size_.GetWidth(),
		      size_.GetHeight());
}

/**
 * Returns name of the port.
 *
 * @return Name of the port.
 */
std::string
UnitPortFigure::name() const {
    return name_;
}
