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
 * @file InputSocketFigure.cc
 *
 * Definition of InputSocketFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include "InputSocketFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

const wxColour InputSocketFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);
const wxColour InputSocketFigure::DEFAULT_BG_COLOUR = wxColour(255, 255, 255);

/**
 * The Constructor.
 */
InputSocketFigure::InputSocketFigure(): Figure() {
    size_ = wxSize(
	MachineCanvasLayoutConstraints::SOCKET_WIDTH,
	MachineCanvasLayoutConstraints::SOCKET_HEIGHT);
}

/**
 * The Destructor.
 */
InputSocketFigure::~InputSocketFigure() {
}

/**
 * Draws the input socket's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
InputSocketFigure::drawSelf(wxDC* dc) {

    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    dc->SetPen(pen);
    wxBrush brush = wxBrush(DEFAULT_BG_COLOUR, wxSOLID);
    if (highlighted_) {
        brush = wxBrush(highlight_, wxSOLID);
    }
    dc->SetBrush(brush);

    int triangleHeight = MachineCanvasLayoutConstraints::TRIANGLE_HEIGHT;

    wxPoint point1 = wxPoint(location_.x + size_.GetWidth()/2, location_.y);
    wxPoint point2 = wxPoint(location_.x, location_.y + triangleHeight);
    wxPoint point3 = wxPoint(
	location_.x + size_.GetWidth(), location_.y + triangleHeight);
    wxPoint trianglePoints[3] = {point1, point2, point3};

    dc->DrawPolygon(3, trianglePoints);
    dc->DrawRectangle(
	location_.x, location_.y + triangleHeight, size_.GetWidth(),
	size_.GetHeight() - triangleHeight);
}
