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
 * @file BusChainFigure.cc
 *
 * Definition of BusChainFigure class.
 *
 * @author Ari Metsähalme 2004 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include <vector>

#include "BusChainFigure.hh"
#include "BridgeFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"


using std::vector;

const wxColour BusChainFigure::DEFAULT_COLOUR = wxColour(255, 150, 150);

/**
 * The Constructor.
 */
BusChainFigure::BusChainFigure(): Figure() {
    minSize_ = wxSize(
	MachineCanvasLayoutConstraints::BUS_MIN_WIDTH,
	MachineCanvasLayoutConstraints::BUS_MIN_HEIGHT);
    size_ = minSize_;
}

/**
 * The Destructor.
 */
BusChainFigure::~BusChainFigure() {
}

/** 
 * Lays out the segments in a column, spaced evenly.
 *
 * @param dc Device context.
 */
void
BusChainFigure::layoutChildren(wxDC* dc) {

    int height = MachineCanvasLayoutConstraints::BRIDGE_SPACE;
    int y = location_.y + MachineCanvasLayoutConstraints::BRIDGE_SPACE;
    int x = location_.x;

    for(unsigned int i = 0; i < children_.size(); i++) {

        children_[i]->setLocation(wxPoint(x, y));
	children_[i]->setWidth(size_.GetWidth());

	// set bridge direction
	BridgeFigure* br = dynamic_cast<BridgeFigure*>(children_[i]);
	if (br != NULL && br->direction() != BridgeFigure::DIR_BIDIR &&
	    br->target() == children_[i-1]) {

	    br->setDirection(BridgeFigure::DIR_LEFT);
	}

	children_[i]->layout(dc);
        y += children_[i]->bounds().GetHeight() +
	    MachineCanvasLayoutConstraints::BRIDGE_SPACE;
	height += children_[i]->bounds().GetHeight() +
	    MachineCanvasLayoutConstraints::BRIDGE_SPACE;
    }

    size_.SetHeight(height);
}

/**
 * Fills the background of the bus chain with a solid color.
 *
 * @param dc The device context.
 */
void
BusChainFigure::drawSelf(wxDC* dc) {
    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    dc->SetPen(pen);
    wxBrush brush = wxBrush(DEFAULT_COLOUR, wxSOLID);
    dc->SetBrush(brush);
    dc->DrawRectangle(location_.x, location_.y, size_.GetWidth(),
                      size_.GetHeight());
}
