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
 * @file SegmentFigure.cc
 *
 * Definition of SegmentFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include "SegmentFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

const wxColour SegmentFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);


/**
 * The Constructor.
 */
SegmentFigure::SegmentFigure(): Figure(), last_(false) {
    minSize_ = wxSize(
	MachineCanvasLayoutConstraints::BUS_MIN_WIDTH,
	MachineCanvasLayoutConstraints::SEGMENT_HEIGHT);
    size_ = minSize_;
}

/**
 * The Destructor.
 */
SegmentFigure::~SegmentFigure() {
}

/**
 * Draws the segment's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
SegmentFigure::drawSelf(wxDC* dc) {

    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    wxBrush brush = wxBrush(DEFAULT_COLOUR, wxSOLID);
    if (highlighted_) {
	pen = wxPen(highlight_, 1, wxSOLID);
	brush = wxBrush(highlight_, wxSOLID);
    }

    dc->SetPen(pen);
    dc->SetBrush(brush);

    if (!last_) {

	// draw arrow 
	const int arrowSize = 8;
	wxPoint arrowPoints[3];
	arrowPoints[0] = wxPoint(
	    location_.x + size_.GetWidth() - arrowSize,
	    location_.y - arrowSize / 2 + size_.GetHeight() / 2);
	arrowPoints[1] = wxPoint(
	    arrowPoints[0].x + arrowSize, arrowPoints[0].y + arrowSize / 2);
	arrowPoints[2] = wxPoint(
	    arrowPoints[0].x, arrowPoints[0].y + arrowSize);
	dc->DrawPolygon(3, arrowPoints);	
	dc->DrawRectangle(
	    location_.x, location_.y, size_.GetWidth() - arrowSize,
	    size_.GetHeight());

    } else {
	dc->DrawRectangle(
	    location_.x, location_.y, size_.GetWidth(), size_.GetHeight());
    }
}
