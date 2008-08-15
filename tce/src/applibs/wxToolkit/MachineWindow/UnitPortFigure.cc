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
 * @file UnitPortFigure.cc
 *
 * Definition of UnitPortFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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
