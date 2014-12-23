/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file SegmentFigure.cc
 *
 * Definition of SegmentFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2010
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include "SegmentFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"
#include "MathTools.hh"

const wxColour SegmentFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);


/**
 * The Constructor.
 */
SegmentFigure::SegmentFigure(int bitWidth): Figure(), last_(false) {
    minSize_ = wxSize(
	MachineCanvasLayoutConstraints::BUS_MIN_WIDTH,
	MachineCanvasLayoutConstraints::SEGMENT_HEIGHT_BASE+
    std::max(0,MathTools::requiredBits(bitWidth)-2) +
    bitWidth/63);
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
