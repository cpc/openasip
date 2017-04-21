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
 * @file OutputSocketFigure.cc
 *
 * Definition of OutputSocketFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include "OutputSocketFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

const wxColour OutputSocketFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);
const wxColour OutputSocketFigure::DEFAULT_BG_COLOUR = wxColour(255, 255, 255);

/**
 * The Constructor.
 */
OutputSocketFigure::OutputSocketFigure(): Figure() {
    size_ = wxSize(
	MachineCanvasLayoutConstraints::SOCKET_WIDTH,
	MachineCanvasLayoutConstraints::SOCKET_HEIGHT);
}

/**
 * The Destructor.
 */
OutputSocketFigure::~OutputSocketFigure() {
}

/**
 * Draws the output socket's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
OutputSocketFigure::drawSelf(wxDC* dc) {
    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    dc->SetPen(pen);
    wxBrush brush = wxBrush(DEFAULT_BG_COLOUR, wxSOLID);
    if (highlighted_) {
        brush = wxBrush(highlight_, wxSOLID);
    }
    dc->SetBrush(brush);

    int triangleHeight = MachineCanvasLayoutConstraints::TRIANGLE_HEIGHT;

    wxPoint point1 = wxPoint(0, 0);
    wxPoint point2 = wxPoint(size_.GetWidth(), 0);
    wxPoint point3 = wxPoint(size_.GetWidth()/2, triangleHeight);
    wxPoint trianglePoints[3] = {point1, point2, point3};

    dc->DrawPolygon(3, trianglePoints, location_.x, location_.y);
    dc->DrawRectangle(
	location_.x, location_.y + triangleHeight, size_.GetWidth(),
	size_.GetHeight() - triangleHeight);
}
