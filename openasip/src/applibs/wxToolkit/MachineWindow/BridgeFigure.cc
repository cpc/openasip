/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file BridgeFigure.cc
 *
 * Definition of BridgeFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include "BridgeFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

const wxColour BridgeFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);
const wxColour BridgeFigure::DEFAULT_BG_COLOUR = wxColour(150, 200, 255);

/**
 * The Constructor.
 */
BridgeFigure::BridgeFigure():
    ConnectionFigure(), direction_(BridgeFigure::DIR_RIGHT) {
    size_ = wxSize(
	MachineCanvasLayoutConstraints::BRIDGE_WIDTH,
	MachineCanvasLayoutConstraints::BRIDGE_HEIGHT);
}

/**
 * The Destructor.
 */
BridgeFigure::~BridgeFigure() {
}

/**
 * Draws the bridge's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
BridgeFigure::drawSelf(wxDC* dc) {

    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    dc->SetPen(pen);
    wxBrush brush = wxBrush(DEFAULT_BG_COLOUR, wxSOLID);
    dc->SetBrush(brush);

    const unsigned int triangleWidth = 200;
    
    wxPoint trianglePoints[3];

    if (direction_ == DIR_RIGHT) {
	trianglePoints[0] = wxPoint(
	    location_.x + size_.GetWidth() / 2 - triangleWidth / 2,
	    location_.y );
	trianglePoints[1] = wxPoint(
	    trianglePoints[0].x + triangleWidth, location_.y);
	trianglePoints[2] = wxPoint(
	    trianglePoints[0].x + triangleWidth / 2,
	    location_.y + size_.GetHeight());

    } else {
	trianglePoints[0] = wxPoint(
	    location_.x + size_.GetWidth() / 2 - triangleWidth / 2,
	    location_.y + size_.GetHeight());
	trianglePoints[1] = wxPoint(
	    trianglePoints[0].x + triangleWidth, trianglePoints[0].y);
	trianglePoints[2] = wxPoint(
	    location_.x + size_.GetWidth() / 2, location_.y);
    }

    dc->DrawPolygon(3, trianglePoints);
}
